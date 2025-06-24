#include "Network/TCPServer.hpp"

#include <sstream>
#include <string>

#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2TCPip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>

#include "Network/ServerSessionData.hpp"
#include "Network/ThreadPool.hpp"
#include "Network/Address.hpp"
#include "Network/Timer.hpp"


std::string net::default_server_request_handler(std::string request)
{
    std::string response = "Response: " + request;

    return response;
}


net::TCPServer::TCPServer()
{
    last_requested_session_data = 0;
    session_data_counter = 0;
    inited = false;
    started = false;
}

net::TCPServer::~TCPServer()
{
    stop();
    closesocket(this->listen_socket);
    freeaddrinfo(this->addr);
    WSACleanup();
}


int net::TCPServer::init(int port, bool localhost)
{
    if (inited)
        return 1;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }

    this->addr = NULL;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (localhost)
    {
        self_address.port = port;
        result = getaddrinfo("127.0.0.1", std::to_string(port).c_str(), &hints, &this->addr);
    }
    else
    {
        initSelfAddress(port);
        result = getaddrinfo(self_address.ip.toString().c_str(), std::to_string(port).c_str(), &hints, &this->addr);
    }

    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << "\n";
        WSACleanup(); // выгрузка библиотеки Ws2_32.dll
        return 1;
    }

    this->listen_socket = socket(this->addr->ai_family, this->addr->ai_socktype, this->addr->ai_protocol);
    if (this->listen_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(this->addr);
        WSACleanup();
        return 1;
    }

    if (bind(this->listen_socket, addr->ai_addr, (int)addr->ai_addrlen) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(this->addr);
        closesocket(this->listen_socket);
        WSACleanup();
        return 1;
    }

    if (listen(this->listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(this->listen_socket);
        WSACleanup();
        return 1;
    }

    inited = true;
    return 0;
}

bool net::TCPServer::start(int threads_count)
{
    if (!inited || started)
        return false;

    started = true;
    listen_handler_thread = std::thread{&TCPServer::listen_handler, this};

    return true;
}

bool net::TCPServer::stop()
{
    if (!inited || !started)
        return false;

    started = false;
    listen_handler_thread.join();

    return true;
}


void net::TCPServer::setRequestHandler(std::string (*new_request_handler)(std::string request))
{
    request_handler = new_request_handler;
}


net::Address net::TCPServer::getSelfAddress()
{
    return self_address;
}


bool net::TCPServer::hasNewSessionData()
{
   return last_requested_session_data < session_data_counter;
}

net::ServerSessionData net::TCPServer::getNextSessionData()
{
    last_requested_session_data++;
    std::unique_lock<std::mutex> locker(session_data_mtx);
    ServerSessionData data = (!sessions_data.empty()) ? sessions_data.front() : ServerSessionData();
    sessions_data.pop();
    locker.unlock();
    return data;
}



void net::TCPServer::initSelfAddress(int port)
{
    char hostname[128];  
    hostent * host_info;
    in_addr self_addr;

    gethostname(hostname, 128);
    host_info = gethostbyname(hostname);

    if ((host_info == NULL) || (host_info->h_addr_list[0] == 0))
        self_address = Address();

    self_addr.s_addr = *(u_long*)host_info->h_addr_list[0];
    self_address = Address(IP(self_addr.S_un.S_un_b.s_b1, self_addr.S_un.S_un_b.s_b2, self_addr.S_un.S_un_b.s_b3, self_addr.S_un.S_un_b.s_b4), port);
}

void net::TCPServer::listen_handler()
{
    fd_set read_s;
    while (started)
    {
        FD_ZERO(&read_s);
        FD_SET(this->listen_socket, &read_s);
        int select_result = select(0, &read_s, NULL, NULL, NULL);
        if (select_result > 0)
        {
            int client_socket = accept(this->listen_socket, NULL, NULL);
            listen_pool.addTask(&TCPServer::client_handler, this, client_socket);
        }
    }
}

void net::TCPServer::client_handler(int client_socket)
{
    //std::cout << "Start " << client_socket << std::endl;
    fd_set read_s;
    timeval time_out;
    time_out.tv_sec = 5;
    FD_ZERO(&read_s);
    FD_SET(client_socket, &read_s);
    if (select(0, &read_s, NULL, NULL, &time_out) > 0)
    {
        u_long mode = 1; //1 для неблокирующего режима
        ioctlsocket(client_socket, FIONBIO, &mode);

        const int max_client_buffer_size = 1024;
        char buf[max_client_buffer_size];

        int total_bytes = 0;
        std::string request;

        int recv_result = 0;
        while ((recv_result = recv(client_socket, buf, max_client_buffer_size, 0)) > 0)
        {
            request += buf;
            total_bytes += recv_result;
            request.resize(total_bytes);
        }

        if (total_bytes > 0)
        {
            std::string response = request_handler(request);
            int send_result = send(client_socket, response.c_str(), response.length(), 0);
            
            std::unique_lock<std::mutex> locker(session_data_mtx);
            sessions_data.push(ServerSessionData(session_data_counter++, request, response));
            locker.unlock();

            if (send_result == SOCKET_ERROR)
                std::cerr << "send failed: " << WSAGetLastError() << "\n";
        }
    }

    closesocket(client_socket);
    //std::cout << "Finish " << client_socket << std::endl;
}
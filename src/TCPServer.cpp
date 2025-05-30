#include "Network/TCPServer.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2TCPip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Network/Address.hpp"
#include "Network/ServerSessionData.hpp"
#include "Network/Timer.hpp"


std::string Network::default_server_request_handler(std::string request)
{
    std::string response = "Response: " + request;

    return response;
}


Network::TCPServer::TCPServer()
{
    last_requested_session_data = 0;
    //available_threads_count = std::thread::hardware_concurrency();
    inited = false;
    started = false;
}

Network::TCPServer::~TCPServer()
{
    stop();
    closesocket(this->listen_socket);
    freeaddrinfo(this->addr);
    WSACleanup();
}


int Network::TCPServer::init(int port, bool localhost)
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

bool Network::TCPServer::start(int threads_count)
{
    if (!inited || started)
        return false;

    started = true;
    listen_handler_thread = std::thread{&TCPServer::listen_handler, this};

    return true;
}

bool Network::TCPServer::stop()
{
    if (!inited || !started)
        return false;

    started = false;
    listen_handler_thread.join();
    for (unsigned int i = 0; i < listen_threads.size(); i++)
        listen_threads[i].join();
    listen_threads.clear();

    return true;
}


void Network::TCPServer::setRequestHandler(std::string (*new_request_handler)(std::string request))
{
    request_handler = new_request_handler;
}


Network::Address Network::TCPServer::getSelfAddress()
{
    return self_address;
}


bool Network::TCPServer::hasNewSessionData()
{
   return last_requested_session_data < sessions_data.size();
}

Network::ServerSessionData Network::TCPServer::getNextSessionData()
{
    return (hasNewSessionData()) ? sessions_data[last_requested_session_data++] : ServerSessionData();
}



void Network::TCPServer::initSelfAddress(int port)
{
    char hostname[128];  
    hostent * host_info;
    in_addr self_addr;

    gethostname(hostname, 128);
    host_info = gethostbyname(hostname);

    if ((host_info == NULL) || (host_info->h_addr_list[0] == 0))
        self_address = Address();

    self_addr.s_addr = *(u_long*)host_info->h_addr_list[0];
    self_address = Address(IP(self_addr), port);
}

void Network::TCPServer::listen_handler()
{
    fd_set read_s;
    timeval time_out;
    time_out.tv_sec = 0;
    time_out.tv_usec = 200000; //Таймаут 0.2 секунды.
    while (started)
    {
        FD_ZERO(&read_s);
        FD_SET(this->listen_socket, &read_s);
        if (select(0, &read_s, NULL, NULL, &time_out) > 0)
        {
            int client_socket = accept(this->listen_socket, NULL, NULL);

            this->listen_threads.resize(this->listen_threads.size() + 1);
            this->listen_threads.back() = std::thread{&TCPServer::client_handler, this, client_socket};

            /*
            std::cout << "s: " << this->listen_threads.size() << std::endl;
            for (int i = this->listen_threads.size() - 1; i >= 0; i--)
            {
                if (this->listen_threads[i].joinable())
                {
                    this->listen_threads[i].join();
                    this->listen_threads.erase(this->listen_threads.begin() + i);
                }
            }*/
        }
    }
}

void Network::TCPServer::client_handler(int client_socket)
{
    //std::cout << "Start " << client_socket << std::endl;
    fd_set read_s;
    timeval time_out;
    time_out.tv_sec = 5;
    FD_ZERO(&read_s);
    FD_SET(client_socket, &read_s);
    if (select(0, &read_s, NULL, NULL, &time_out) > 0)
    {
        const int max_client_buffer_size = 1024;
        char buf[max_client_buffer_size];

        int result = recv(client_socket, buf, max_client_buffer_size, 0);
        buf[result] = '\0';
        std::string request = buf;
        //std::cout << buf << std::endl;

        if (result == SOCKET_ERROR)
            std::cerr << "recv failed: " << result << "\n";
        else if (result == 0)
            std::cerr << "connection closed...\n";
        else if (result > 0)
        {
            std::string response = request_handler(request);
            result = send(client_socket, response.c_str(), response.length(), 0);
            sessions_data.push_back(ServerSessionData(sessions_data.size(), request, response));
            //std::cout << sessions_data.size() << "\n";

            if (result == SOCKET_ERROR)
                std::cerr << "send failed: " << WSAGetLastError() << "\n";
        }
    }

    closesocket(client_socket);
    //std::cout << "Finish " << client_socket << std::endl;
}
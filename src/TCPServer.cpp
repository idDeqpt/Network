#include "Network/TCPServer.hpp"

#include <sstream>
#include <string>

#ifdef _WIN32
    #define _WIN32_WINNT 0x501
    #include <WinSock2.h>
    #include <WS2TCPip.h>
    #pragma comment(lib, "Ws2_32.lib")
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <arpa/inet.h>
#endif

#include "Network/ServerSessionData.hpp"
#include "Network/ThreadPool.hpp"
#include "Network/Address.hpp"
#include "Network/Timer.hpp"


#ifdef _WIN32
    typedef int SockLen_t;
    typedef SOCKADDR_IN SocketAddr_in;
    typedef SOCKET Socket;
    typedef u_long ka_prop_t;
#else
    typedef socklen_t SockLen_t;
    typedef struct sockaddr_in SocketAddr_in;
    typedef int Socket;
    typedef int ka_prop_t;
#endif

#ifdef _WIN32
    #define WIN(exp) exp
    #define NIX(exp)
#else
    #define WIN(exp)
    #define NIX(exp) exp
#endif


std::string net::default_server_request_handler(TCPServer* server, std::string request)
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
    WIN(
        closesocket(this->listen_socket);
        WSACleanup();
    )
    NIX(close(this->listen_socket);)
}


int net::TCPServer::init(int port)
{
    if (inited)
        return 1;

    WIN(
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << "\n";
            return result;
        }
    )

    SocketAddr_in addr;
    addr.sin_addr
        WIN(.S_un.S_addr)NIX(.s_addr) = INADDR_ANY;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    this->listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listen_socket WIN(== INVALID_SOCKET)NIX(== -1))
        return 2;

    int flag = true;
    if ((setsockopt(this->listen_socket, SOL_SOCKET, SO_REUSEADDR, WIN((char*))&flag, sizeof(flag)) == -1) ||
        (bind(this->listen_socket, (struct sockaddr*)&addr, sizeof(addr)) WIN(== INVALID_SOCKET)NIX(== -1)))
        return 3;

    if (listen(this->listen_socket, SOMAXCONN) WIN(== SOCKET_ERROR)NIX(< 0))
        return 4;

    inited = true;
    return 0;
}

bool net::TCPServer::start()
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


void net::TCPServer::setRequestHandler(std::string (*new_request_handler)(TCPServer* server, std::string request))
{
    request_handler = new_request_handler;
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



void net::TCPServer::listen_handler()
{
    fd_set read_s;
    while (started)
    {
        FD_ZERO(&read_s);
        FD_SET(this->listen_socket, &read_s);
        int select_result = select(this->listen_socket + 1, &read_s, NULL, NULL, NULL);
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
    time_out.tv_usec = 0;
    FD_ZERO(&read_s);
    FD_SET(client_socket, &read_s);
    if (select(client_socket + 1, &read_s, NULL, NULL, &time_out) > 0)
    {
        WIN(
            u_long mode = 1; //1 для неблокирующего режима
            ioctlsocket(client_socket, FIONBIO, &mode);
        )

        const int max_client_buffer_size = 1024;
        char buf[max_client_buffer_size];

        int total_bytes = 0;
        std::string request;

        int recv_result = 0;
        int content_length = -1;
        while ((recv_result = recv(client_socket, buf, max_client_buffer_size, WIN(0)NIX(MSG_DONTWAIT))) != 0)
        {
            if (recv_result > -1)
            {
                request += buf;
                total_bytes += recv_result;
                request.resize(total_bytes);
            }
            else
            {
                if (content_length == -1)
                {
                    int field_end = request.find("Content-Length:");
                    if (field_end == std::string::npos)
                        content_length = 0;
                    else
                    {
                        field_end += 15;
                        try 
                        {
                            int number_end = request.find("\r\n", field_end);
                            if (number_end != std::string::npos)
                                content_length = stoi(request.substr(field_end, number_end - field_end));
                        }catch(...){}
                    }
                }
                else if (content_length == 0)
                {
                    int end_i = request.find("\r\n\r\n");
                    if (end_i != std::string::npos)
                        break;
                }
                else
                {
                    if (content_length <= (request.size() - request.find("\r\n\r\n") - 4))
                        break;
                }
            }
        }

        if (total_bytes > 0)
        {
            std::string response = request_handler(this, request);
            int send_result = send(client_socket, response.c_str(), response.length(), 0);
            
            std::unique_lock<std::mutex> locker(session_data_mtx);
            sessions_data.push(ServerSessionData(session_data_counter++, request, response));
            locker.unlock();

            WIN(
                if (send_result == SOCKET_ERROR)
                    std::cerr << "send failed: " << WSAGetLastError() << "\n";
            )
        }
    }

    WIN(closesocket)NIX(close)(client_socket);
    //std::cout << "Finish " << client_socket << std::endl;
}
#include "Network/HTTPServer.hpp"

#include <iostream>

#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2TCPip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Network/TCPServer.hpp"
#include "Network/HTTP.hpp"
#include "Network/URL.hpp"


net::HTTPResponse net::default_404_handler()
{
    HTTPResponse response;

    response.body = "<p>404</p>";

    response.start_line[0] = "HTTP/1.1";
    response.start_line[1] = "404";
    response.start_line[2] = "NOT FOUND";

    response.headers["Version"] = "HTTP/1.1";
    response.headers["Content-Type"] = "text/html; charset=utf-8";
    response.headers["Version"] = "HTTP/1.1";
    response.headers["Content-Length"] = std::to_string(response.body.length());

    return response;
}

std::string net::default_http_handler(HTTPServer& server, std::string request)
{
    HTTPRequest req(request);
    URI uri(req.start_line[1]);
    std::string path = uri.toString(false);

    if (server.getHandlersPtr().count(path) == 0)
        return server.get404Handler()().toString();

    return server.getHandlersPtr()[path](req).toString();
}



net::HTTPServer::HTTPServer() : TCPServer()
{

}

net::HTTPServer::~HTTPServer()
{

}


void net::HTTPServer::set404Handler(HTTPResponse (*new_404_handler)(void))
{
    code_404_handler = new_404_handler;
}

void net::HTTPServer::setHTTPHandler(std::string (*new_http_handler)(HTTPServer&, std::string))
{
    http_handler = new_http_handler;
}

net::HTTPResponse(*net::HTTPServer::get404Handler())()
{
    return code_404_handler;
}


void net::HTTPServer::addHandler(std::string path, std::function<HTTPResponse(HTTPRequest)> handler)
{
    paths_handlers[path] = handler;
}

void net::HTTPServer::removeHandler(std::string path)
{
    paths_handlers.erase(path);
}

std::unordered_map<std::string, std::function<net::HTTPResponse(net::HTTPRequest)>>& net::HTTPServer::getHandlersPtr()
{
    return paths_handlers;
}


void net::HTTPServer::client_handler(int client_socket)
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
        //std::cout << "DATAAAAAA\n" << buf << "DATAAAAAAAA\n" << std::endl;

        if (result == SOCKET_ERROR)
            std::cerr << "recv failed: " << result << "\n";
        else if (result == 0)
            std::cerr << "connection closed...\n";
        else if (result > 0)
        {
            std::string response = http_handler(*this, request);
            result = send(client_socket, response.c_str(), response.length(), 0);
            
            std::unique_lock<std::mutex> locker(session_data_mtx);
            sessions_data.push_back(ServerSessionData(sessions_data.size(), request, response));
            if (sessions_data.size() > 20)
                sessions_data.erase(sessions_data.begin(), sessions_data.begin() + (sessions_data.size() - 20));
            locker.unlock();
            std::cout << "SIZE: " << sessions_data.size() << "\n";

            if (result == SOCKET_ERROR)
                std::cerr << "send failed: " << WSAGetLastError() << "\n";
        }
    }

    closesocket(client_socket);
    //std::cout << "Finish " << client_socket << std::endl;
}
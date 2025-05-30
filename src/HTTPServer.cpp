#include "Network/HTTPServer.hpp"

#include <iostream>

#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2TCPip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Network/TCPServer.hpp"
#include "Network/HTTP.hpp"
#include "Network/URL.hpp"


Network::HTTPResponse Network::default_404_handler()
{
    HTTPResponse response;

    response.body = "<p>404</p>";

    response.start_line["http-version"] = "HTTP/1.1";
    response.start_line["status-code"] = "404";
    response.start_line["status-comment"] = "NOT FOUND";

    response.headers["Version"] = "HTTP/1.1";
    response.headers["Content-Type"] = "text/html; charset=utf-8";
    response.headers["Version"] = "HTTP/1.1";
    response.headers["Content-Length"] = std::to_string(response.body.length());

    return response;
}



Network::HTTPServer::HTTPServer() : TCPServer()
{

}

Network::HTTPServer::~HTTPServer()
{

}


void Network::HTTPServer::set404Handler(HTTPResponse (*new_404_handler)(void))
{
    code_404_handler = new_404_handler;
}


void Network::HTTPServer::addHandler(std::string path, std::function<HTTPResponse(HTTPRequest)> handler)
{
    paths_handlers[path] = handler;
}

void Network::HTTPServer::removeHandler(std::string path)
{
    paths_handlers.erase(path);
}

std::unordered_map<std::string, std::function<Network::HTTPResponse(Network::HTTPRequest)>>& Network::HTTPServer::getHandlersPtr()
{
    return paths_handlers;
}


std::string Network::HTTPServer::http_handler(std::string request)
{
    HTTPRequest req(request);
    URI uri(req.start_line["uri"]);
    std::string path = uri.toString(false);

    if (paths_handlers.count(path) == 0)
        return code_404_handler().toString();

    return paths_handlers[path](req).toString();
}

void Network::HTTPServer::client_handler(int client_socket)
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
            std::string response = http_handler(request);
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
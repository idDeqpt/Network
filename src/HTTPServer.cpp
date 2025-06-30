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

std::string net::default_http_handler(TCPServer* server, std::string request)
{
    HTTPRequest req(request);
    URI uri(req.start_line[1]);
    std::string path = uri.toString(false);

    HTTPServer* http_server = dynamic_cast<HTTPServer*>(server);
    if (http_server->getHandlersPtr().count(path) == 0)
        return http_server->get404Handler()().toString();

    return http_server->getHandlersPtr()[path](req).toString();
}



net::HTTPServer::HTTPServer() : TCPServer()
{
    setRequestHandler(default_http_handler);
}

net::HTTPServer::~HTTPServer()
{

}


void net::HTTPServer::set404Handler(HTTPResponse (*new_404_handler)(void))
{
    code_404_handler = new_404_handler;
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
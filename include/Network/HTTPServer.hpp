#ifndef NETWORK_HTTP_SERVER
#define NETWORK_HTTP_SERVER

#include "TCPServer.hpp"
#include "HTTP.hpp"


namespace net
{
    class HTTPServer;

    HTTPResponse default_404_handler();
    std::string default_http_handler(TCPServer*, std::string);

    class HTTPServer : public TCPServer
    {
    public:
        HTTPServer();
        ~HTTPServer();

        void set404Handler(HTTPResponse (*new_404_handler)(void));
        HTTPResponse(*get404Handler())();

        void addHandler(std::string path, std::function<HTTPResponse(HTTPRequest)> handler);
        void removeHandler(std::string path);
        std::unordered_map<std::string, std::function<HTTPResponse(HTTPRequest)>>& getHandlersPtr();

    protected:
        std::unordered_map<std::string, std::function<HTTPResponse(HTTPRequest)>> paths_handlers;
        HTTPResponse (*code_404_handler)() = default_404_handler;
    };
}

#endif //NETWORK_HTTP_SERVER
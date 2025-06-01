#ifndef NETWORK_HTTP_SERVER
#define NETWORK_HTTP_SERVER

#include "TCPServer.hpp"
#include "HTTP.hpp"


namespace net
{
    class HTTPServer;

    HTTPResponse default_404_handler();
    std::string default_http_handler(HTTPServer&, std::string);

    class HTTPServer : public TCPServer
    {
    public:
        HTTPServer();
        ~HTTPServer();

        void set404Handler(HTTPResponse (*new_404_handler)(void));
        void setHTTPHandler(std::string (*new_http_handler)(HTTPServer&, std::string));
        HTTPResponse(*get404Handler())();

        void addHandler(std::string path, std::function<HTTPResponse(HTTPRequest)> handler);
        void removeHandler(std::string path);
        std::unordered_map<std::string, std::function<HTTPResponse(HTTPRequest)>>& getHandlersPtr();

    protected:
        std::unordered_map<std::string, std::function<HTTPResponse(HTTPRequest)>> paths_handlers;
        HTTPResponse (*code_404_handler)() = default_404_handler;
        std::string (*http_handler)(HTTPServer&, std::string) = default_http_handler;

        //std::string http_handler(std::string request);
        void client_handler(int client_socket);

    };
}

#endif //NETWORK_HTTP_SERVER
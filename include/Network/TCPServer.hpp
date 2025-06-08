#ifndef NETWORK_TCP_SERVER
#define NETWORK_TCP_SERVER

#include <string>
#include <thread>
#include <vector>

#include "ServerSessionData.hpp"
#include "ThreadPool.hpp"
#include "Address.hpp"

struct addrinfo;


namespace net
{
    std::string default_server_request_handler(std::string request);

    class TCPServer
    {
    public:
        TCPServer();
        ~TCPServer();

        int init(int port, bool localhost = false);
        bool start(int threads_count = -1);
        bool stop();

        void setRequestHandler(std::string (*new_request_handler)(std::string));

        Address getSelfAddress();

        bool hasNewSessionData();
        ServerSessionData getNextSessionData();

    protected:
        bool inited, started;
        int listen_socket;
        addrinfo* addr;
        Address self_address;
        int last_requested_session_data;
        std::vector<ServerSessionData> sessions_data;
        std::thread listen_handler_thread;
        ThreadPool listen_pool;
        std::string (*request_handler)(std::string) = default_server_request_handler;

        void initSelfAddress(int port);
        void listen_handler();
        virtual void client_handler(int client_socket);
    };
}

#endif //NETWORK_TCP_SERVER
#ifndef NETWORK_TCP_SERVER
#define NETWORK_TCP_SERVER

#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

#include "ServerSessionData.hpp"
#include "ThreadPool.hpp"
#include "Address.hpp"


namespace net
{
    class TCPServer;

    std::string default_server_request_handler(TCPServer*, std::string request);

    class TCPServer
    {
    public:
        TCPServer();
        ~TCPServer();

        int init(int port);
        bool start();
        bool stop();

        void setRequestHandler(std::string (*new_request_handler)(TCPServer*, std::string));

        bool hasNewSessionData();
        ServerSessionData getNextSessionData();

    protected:
        bool inited, started;
        int listen_socket;
        int session_data_counter, last_requested_session_data;
        std::queue<ServerSessionData> sessions_data;
        std::thread listen_handler_thread;
        ThreadPool listen_pool;
        std::mutex session_data_mtx;
        std::string (*request_handler)(TCPServer*, std::string) = default_server_request_handler;

        void initSelfAddress(int port);
        void listen_handler();
        virtual void client_handler(int client_socket);
    };
}

#endif //NETWORK_TCP_SERVER
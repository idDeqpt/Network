#ifndef NETWORK_TCP_SERVER
#define NETWORK_TCP_SERVER

#include <unordered_map>
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

    void default_server_request_handler(TCPServer*, int client_socket);

    class TCPServer
    {
    public:
        TCPServer();
        ~TCPServer();

        int init(int port);
        bool start();
        bool stop();

        void setRequestHandler(void (*new_request_handler)(TCPServer*, int));

        bool hasNewSessionData();
        ServerSessionData getNextSessionData();

        std::string recv(int socket);
        void send(int socket, const std::string& message);

    protected:
        bool inited, started;
        int listen_socket;
        int session_data_counter;
        std::queue<ServerSessionData> sessions_data;
        std::unordered_map<int, int> client_id_table;
        std::thread listen_handler_thread;
        ThreadPool listen_pool;
        std::mutex session_data_mtx;
        void (*request_handler)(TCPServer*, int) = default_server_request_handler;

        void initSelfAddress(int port);
        void listen_handler();
        virtual void client_handler(int client_socket);
    };
}

#endif //NETWORK_TCP_SERVER
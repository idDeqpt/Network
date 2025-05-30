#ifndef NETWORK_TCP_SERVER
#define NETWORK_TCP_SERVER
  
#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2TCPip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <thread>
#include <vector>

#include "Address.hpp"
#include "ServerSessionData.hpp"


namespace Network
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
        struct addrinfo* addr;
        Address self_address;
        int last_requested_session_data;
        std::vector<ServerSessionData> sessions_data;
        std::thread listen_handler_thread;
        std::vector<std::thread> listen_threads;
        std::string (*request_handler)(std::string) = default_server_request_handler;

        void initSelfAddress(int port);
        void listen_handler();
        virtual void client_handler(int client_socket);
    };
}

#endif //NETWORK_TCP_SERVER
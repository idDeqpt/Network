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
	class TCPServer
	{
	public:
		TCPServer();
		~TCPServer();

		int init(int port);
		bool start();
		bool stop();

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

		void initSelfAddress(int port);
		void listen_handler();

		virtual void client_handler(int client_socket);
		virtual void request_handler(int client_socket);

		virtual std::string recv_handler(int socket);
		virtual void send_handler(int socket, const std::string& message);
	};
}

#endif //NETWORK_TCP_SERVER
#ifndef NETWORK_TCP_CLIENT_CLASS
#define NETWORK_TCP_CLIENT_CLASS

#include <string>

#include "Address.hpp"


namespace Network
{
	class TCPClient
	{
	public:
		TCPClient();
		~TCPClient();

		int connect(std::string host, int port);
		int connect(Address address);
		bool close();

		void send(std::string message);
		std::string recv();

	protected:
		int server_socket;
	};
}

#endif //NETWORK_TCP_CLIENT_CLASS
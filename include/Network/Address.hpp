#ifndef NETWORK_ADDRESS
#define NETWORK_ADDRESS

#include <string>

#pragma comment(lib, "ws2_32.lib")
#include "winsock2.h"


namespace Network
{
	class IP
	{
	public:
		IP();
		IP(int v1, int v2, int v3, int v4);
		IP(in_addr addr);
		IP(std::string ip);

		std::string toString();

		int& operator[](int index);

	protected:
		int values[4];
	};

	class Address
	{
	public:
		IP ip;
		int port;

		Address();
		Address(IP ip, int port);
		Address(SOCKADDR_IN addr);
		Address(std::string address);

		std::string toString();
		SOCKADDR_IN toSockaddrIn();
	};
}

#endif //NETWORK_ADDRESS
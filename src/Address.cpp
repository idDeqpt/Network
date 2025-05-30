#include "Network/Address.hpp"

#include <string>

#pragma comment(lib, "ws2_32.lib")
#include "winsock2.h"


Network::IP::IP()
{
	values[0] = 127;
	values[1] = 0;
	values[2] = 0;
	values[3] = 1;
}

Network::IP::IP(int v1, int v2, int v3, int v4)
{
	values[0] = v1;
	values[1] = v2;
	values[2] = v3;
	values[3] = v4;
}

Network::IP::IP(in_addr addr)
{
	values[0] = int(addr.S_un.S_un_b.s_b1);
	values[1] = int(addr.S_un.S_un_b.s_b2);
	values[2] = int(addr.S_un.S_un_b.s_b3);
	values[3] = int(addr.S_un.S_un_b.s_b4);
}

Network::IP::IP(std::string ip)
{
	ip += ".";
	int last_point_pos = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		int points_pos = ip.find(".", last_point_pos + 1);
		values[i] = stoi(ip.substr(last_point_pos + 1, points_pos - last_point_pos - 1));
		last_point_pos = points_pos;
	}
}


std::string Network::IP::toString()
{
	return std::to_string(values[0]) + "." + std::to_string(values[1]) + "." + std::to_string(values[2]) + "." + std::to_string(values[3]);
}

int& Network::IP::operator[](int index)
{
	return values[index];
}



Network::Address::Address()
{
	port = 0;
}

Network::Address::Address(IP ip, int port)
{
	this->ip = ip;
	this->port = port;
}

Network::Address::Address(SOCKADDR_IN addr)
{
	ip = IP(addr.sin_addr);
	port = ntohs(addr.sin_port);
}

Network::Address::Address(std::string address)
{
	int points_pos = address.find(":");
	ip = IP(address.substr(0, points_pos));
	port = stoi(address.substr(points_pos + 1, address.length() - points_pos - 1));
}


std::string Network::Address::toString()
{
	return ip.toString() + ":" + std::to_string(port);
}


SOCKADDR_IN Network::Address::toSockaddrIn()
{
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(ip.toString().c_str());
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	return addr;
}
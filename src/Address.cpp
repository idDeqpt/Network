#include "Network/Address.hpp"

#include <string>


net::IP::IP()
{
	values[0] = 127;
	values[1] = 0;
	values[2] = 0;
	values[3] = 1;
}

net::IP::IP(int v1, int v2, int v3, int v4)
{
	values[0] = v1;
	values[1] = v2;
	values[2] = v3;
	values[3] = v4;
}

net::IP::IP(std::string ip)
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


std::string net::IP::toString()
{
	return std::to_string(values[0]) + "." + std::to_string(values[1]) + "." + std::to_string(values[2]) + "." + std::to_string(values[3]);
}

int& net::IP::operator[](int index)
{
	return values[index];
}



net::Address::Address()
{
	port = 0;
}

net::Address::Address(IP ip, int port)
{
	this->ip = ip;
	this->port = port;
}
net::Address::Address(std::string address)
{
	int points_pos = address.find(":");
	ip = IP(address.substr(0, points_pos));
	port = stoi(address.substr(points_pos + 1, address.length() - points_pos - 1));
}


std::string net::Address::toString()
{
	return ip.toString() + ":" + std::to_string(port);
}
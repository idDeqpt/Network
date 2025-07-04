#include "Network/TCPClient.hpp"

#include <string>
#include <iostream>

#ifdef _WIN32
	#define _WIN32_WINNT 0x501
	#include <WinSock2.h>
	#include <WS2TCPip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
#endif

#ifdef _WIN32
    #define WIN(exp) exp
    #define NIX(exp)
#else
    #define WIN(exp)
    #define NIX(exp) exp
#endif


net::TCPClient::TCPClient()
{
	server_socket = NULL;
}

net::TCPClient::~TCPClient()
{
	this->close();
	WIN(WSACleanup());
}


int net::TCPClient::connect(std::string host, int port)
{WIN(
	WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }

	struct hostent *host_info = gethostbyname(host.c_str());
	if (host_info == nullptr)
		return 1;

	in_addr addr = *(in_addr*)host_info->h_addr_list[0];
	return this->connect(Address(IP(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4), port));)
	return 0;
}

int net::TCPClient::connect(Address address)
{WIN(
	if (this->server_socket != NULL)
		this->close();
	std::cout << "connect to " << address.toString() << std::endl;

	WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }

    this->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->server_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    } std::cout << "socket created: " << server_socket << std::endl;

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address.ip.toString().c_str());
    server_addr.sin_port = htons(address.port);

    if (::connect(this->server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Error at connect: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    });

    return 0;
}

bool net::TCPClient::close()
{
	if (this->server_socket == NULL)
		return false;

	WIN(closesocket(this->server_socket);)
    this->server_socket == NULL;

    return true;
}


void net::TCPClient::send(std::string message)
{WIN(
	if (this->server_socket != NULL)
		::send(this->server_socket, message.c_str(), message.length(), NULL);)
}

std::string net::TCPClient::recv()
{
	if (this->server_socket == NULL)
		return "";

	/*
	const int max_buffer_size = 1024;
	char buf[max_buffer_size];
	int bytes = ::recv(this->server_socket, buf, max_buffer_size, NULL);
	buf[bytes] = '\0';
	std::cout << buf << std::endl;
	*/

	std::string return_data;WIN(
	const int max_buffer_size = 1024;
	char buf[max_buffer_size];
	buf[::recv(this->server_socket, buf, max_buffer_size, NULL)] = '\0';
	return_data = buf;
	/*while (::recv(this->server_socket, buf, max_buffer_size, NULL) > 0)
	{
        for (int i = 0; buf[i] >= 32 || buf[i] == '\n' || buf[i] == '\r'; i++)
            return_data += buf[i];
	}*/
	std::cout << "BUF: " << return_data << std::endl;)

	return return_data;
}
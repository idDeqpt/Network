#ifndef NETWORK_ADDRESS
#define NETWORK_ADDRESS

#include <string>


namespace net
{
	class IP
	{
	public:
		IP();
		IP(int v1, int v2, int v3, int v4);
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
		Address(std::string address);

		std::string toString();
	};
}

#endif //NETWORK_ADDRESS
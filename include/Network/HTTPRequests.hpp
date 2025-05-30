#ifndef NETWORK_HTTP_REQUESTS_FUNCTIONS
#define NETWORK_HTTP_REQUESTS_FUNCTIONS

#include <string>

#include "URL.hpp"
#include "HTTP.hpp"


namespace Network
{
	namespace Requests
	{
		std::string get(std::string url);
		HTTPResponse get(URL url);
	}
}

#endif //NETWORK_HTTP_REQUESTS_FUNCTIONS
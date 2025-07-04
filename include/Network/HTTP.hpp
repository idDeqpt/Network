#ifndef NETWORK_HTTP_CLASS
#define NETWORK_HTTP_CLASS

#include <string>
#include <unordered_map>


namespace net
{
	class HTTP
	{
	public:
		std::string start_line[3];
		std::unordered_map<std::string, std::string> headers;
		std::string body;

		HTTP();
		HTTP(std::string message);
		std::string toString();
	};

	class HTTPRequest : public HTTP
	{
	public:
		HTTPRequest();
		HTTPRequest(std::string request);
	};

	class HTTPResponse : public HTTP
	{
	public:
		HTTPResponse();
		HTTPResponse(std::string response);

	};
}

#endif //NETWORK_HTTP_CLASS
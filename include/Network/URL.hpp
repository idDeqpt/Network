#ifndef NETWORK_URL_CLASS
#define NETWORK_URL_CLASS

#include <string>
#include <vector>
#include <unordered_map>

#include "Address.hpp"


namespace net
{
	class URI
	{
	public:
		URI();
		URI(std::string uri);

		unsigned int getLength();
		std::unordered_map<std::string, std::string>& getParamsPtr();

		std::string toString(bool with_params = true);

		std::string& operator[](int index);

	protected:
		std::vector<std::string> path;
		std::unordered_map<std::string, std::string> params;
	};

	class URL
	{
	public:
		URL();
		URL(std::string scheme, std::string host);
		URL(std::string scheme, std::string host, URI uri);
		URL(std::string url);

		void setScheme(std::string new_scheme);
		void setHost(std::string new_host);
		void setURI(URI new_uri);

		std::string getScheme();
		std::string getHost();
		URI getURI();

		std::string toString();

	protected:
		std::string scheme;
		std::string host;
		URI uri;
	};
}

#endif //NETWORK_URL_CLASS
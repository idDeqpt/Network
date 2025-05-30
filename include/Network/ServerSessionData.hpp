#ifndef SERVER_SESSION_DATA_CALSS
#define SERVER_SESSION_DATA_CALSS

#include <string>


namespace Network
{
	class ServerSessionData
	{
	public:
		ServerSessionData();
		ServerSessionData(int id, std::string request, std::string response);

		int getId();
		std::string getRequest();
		std::string getResponse();

	protected:
		int id;
		std::string request, response;
	};
}

#endif //SERVER_SESSION_DATA_CALSS
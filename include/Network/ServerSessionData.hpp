#ifndef SERVER_SESSION_DATA_CALSS
#define SERVER_SESSION_DATA_CALSS

#include <string>


namespace net
{
	class ServerSessionData
	{
	public:
		enum Type
		{
			REQUEST,
			RESPONSE
		};
		ServerSessionData();
		ServerSessionData(int id, Type type, float time, std::string text);

		int getId();
		Type getType();
		float getTime();
		std::string getText();

	protected:
		int id;
		Type type;
		float time;
		std::string text;
	};
}

#endif //SERVER_SESSION_DATA_CALSS
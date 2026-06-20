#ifndef NETWORK_WEB_SERVER_CLASS_HEADER
#define NETWORK_WEB_SERVER_CLASS_HEADER

#include <Network/TCPServer.hpp>
#include <Network/HTTP.hpp>

#include <string>

namespace net
{
	class WebServer : public TCPServer
	{
	public:
		WebServer(const std::string& res_dir);
		virtual ~WebServer() = default;

	protected:
		std::string  m_resources_directory;

		virtual void request_handler(int client_socket) override;
		virtual void connection_handler(int client_socket);
		virtual void session_handler(net::HTTPRequest request, int client_socket);
	};
}

#endif //NETWORK_WEB_SERVER_CLASS_HEADER
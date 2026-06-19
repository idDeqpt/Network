#ifndef NETWORK_WEB_SERVER_SECURE_CLASS_HEADER
#define NETWORK_WEB_SERVER_SECURE_CLASS_HEADER

#include <Network/WebServer.hpp>

#include <unordered_map>
#include <string>

struct WOLFSSL_CTX;
struct WOLFSSL;

namespace net
{
	class WebServerSecure : public WebServer
	{
	public:
		WebServerSecure(const std::string& certs_dir, const std::string& res_dir);
		~WebServerSecure();

	protected:
		std::string  m_certificates_directory;
		WOLFSSL_CTX* m_ctx;
		std::unordered_map<int, WOLFSSL*> m_ssl_map;

		void request_handler(int client_socket) override;

		std::string recv_handler(int socket) override;
		void send_handler(int socket, const std::string& message) override;
	};
}

#endif //NETWORK_WEB_SERVER_SECURE_CLASS_HEADER
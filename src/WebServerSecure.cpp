#include <Network/WebServerSecure.hpp>

#include <Network/WebServer.hpp>

#include <unordered_map>
#include <string>

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>


#ifdef _WIN32
	#define WIN(exp) exp
	#define NIX(exp)
#else
	#define WIN(exp)
	#define NIX(exp) exp
#endif


namespace net
{

bool is_request_complete(const std::string& data, int& content_length);


WebServerSecure::WebServerSecure(const std::string& certs_dir, const std::string& res_dir):
	m_certificates_directory(certs_dir),
	WebServer(res_dir)
{
	wolfSSL_Init();

	m_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
	if (!m_ctx)
	{
		std::cerr << "Failed to create SSL_CTX" << std::endl;
		return;
	}

	if (wolfSSL_CTX_use_certificate_chain_file(m_ctx, std::string(m_certificates_directory + "full_certificate_chain.pem").c_str()) != SSL_SUCCESS)
		std::cerr << "Failed to load certificate PEM" << std::endl;
	if (wolfSSL_CTX_use_PrivateKey_file(m_ctx, std::string(m_certificates_directory + "private_key.pem").c_str(), SSL_FILETYPE_PEM) != SSL_SUCCESS)
		std::cerr << "Failed to load private key PEM" << std::endl;
}

WebServerSecure::~WebServerSecure()
{
	if (m_ctx)
	{
		wolfSSL_CTX_free(m_ctx);
		m_ctx = nullptr;
	}

	wolfSSL_Cleanup();
}



void WebServerSecure::request_handler(int client_socket)
{
	WOLFSSL* ssl = wolfSSL_new(m_ctx);
	if (!ssl)
	{
		std::cerr << "wolfSSL_new failed on socket " << client_socket << std::endl;
		return;
	}
	m_ssl_map[client_socket] = ssl;
	wolfSSL_set_fd(ssl, client_socket);
	WIN(
		u_long mode = 0;
		ioctlsocket(client_socket, FIONBIO, &mode);
	)
	NIX(
		int flags = fcntl(client_socket, F_GETFL, 0);
		fcntl(client_socket, F_SETFL, flags & ~O_NONBLOCK);
	)

	int accept_res = wolfSSL_accept(ssl);
	if (accept_res != SSL_SUCCESS)
	{
		std::cerr << "TLS handshake failed on socket " << client_socket << std::endl;
		wolfSSL_free(ssl);
		return;
	}

	session_handler(client_socket);

	wolfSSL_free(ssl);
	m_ssl_map.erase(client_socket);
}


std::string WebServerSecure::recv_handler(int socket)
{
    auto it = m_ssl_map.find(socket);
    if (it == m_ssl_map.end())
    {
        std::cerr << "No SSL object for socket " << socket << std::endl;
        return "";
    }
    WOLFSSL* ssl = it->second;

    static constexpr int max_buffer = 1024;
    char buf[max_buffer];
    std::string result;
    int content_length = -1;

    while (true)
    {
        int n = wolfSSL_read(ssl, buf, max_buffer);
        if (n > 0)
        {
            result.append(buf, n);
            if (is_request_complete(result, content_length))
                break;
        }
        else if (n == 0)
            break;
        else
        {
            int err = wolfSSL_get_error(ssl, n);
            std::cerr << "wolfSSL_read error: " << err << std::endl;
            return "";
        }
    }
    return result;
}

void WebServerSecure::send_handler(int socket, const std::string& message)
{
	auto it = m_ssl_map.find(socket);
	if (it == m_ssl_map.end())
	{
		std::cerr << "No SSL object for socket " << socket << std::endl;
		return;
	}
	WOLFSSL* ssl = it->second;

	int res = wolfSSL_write(ssl, message.c_str(), int(message.size()));
	if (res != int(message.size()))
		std::cerr << "wolfSSL_write failed on socket " << socket << std::endl;
}



bool is_request_complete(const std::string& data, int& content_length)
{
	if (content_length == -1)
	{
		unsigned int pos = data.find("Content-Length:");
		if (pos == std::string::npos)
			content_length = 0;
		else
		{
			pos += 15;
			unsigned int end = data.find("\r\n", pos);
			if (end != std::string::npos)
				content_length = std::stoi(data.substr(pos, end - pos));
		}
	}
	if (content_length == 0)
		return data.find("\r\n\r\n") != std::string::npos;
	else
	{
		unsigned int headers_end = data.find("\r\n\r\n");
		if (headers_end == std::string::npos)
			return false;
		unsigned int body_received = data.size() - headers_end - 4;
		return body_received >= (unsigned int)content_length;
	}
}

} //namespace net
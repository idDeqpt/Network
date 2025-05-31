#include "Network/HTTP.hpp"

#include <string>
#include <unordered_map>
#include <iostream>


net::HTTP::HTTP() : start_line{"", "", ""}
{
	//start_line = {"", "", ""};
	body = "";
}

net::HTTP::HTTP(std::string message) : HTTP()
{
	int body_pos = (message.find("\r\n\r\n\r\n") == std::string::npos) ? std::string::npos : (message.find("\r\n\r\n") + 6);
	int pointer_begin = 0;
	int pointer_end = message.find(" ");
	start_line[0] = message.substr(0, pointer_end);
	pointer_begin = pointer_end + 1;

	pointer_end = message.find(" ", pointer_begin + 1);
	start_line[1] = message.substr(pointer_begin, pointer_end - pointer_begin);
	pointer_begin = pointer_end + 1;

	pointer_end = message.find("\r\n", pointer_begin + 1);
	start_line[2] = message.substr(pointer_begin, pointer_end - pointer_begin);
	pointer_begin = pointer_end + 2;

	int stop_point = (body_pos == std::string::npos) ? message.length() : (body_pos - 6);
	for (pointer_end = message.find("\r\n", pointer_begin + 1); (pointer_end != std::string::npos) && (pointer_begin < stop_point); pointer_end = message.find("\r\n", pointer_begin + 1))
	{
		int sep_pos = message.find(": ", pointer_begin);
		headers[message.substr(pointer_begin, sep_pos - pointer_begin)] = message.substr(sep_pos + 2, pointer_end - sep_pos - 2);
		pointer_begin = pointer_end + 2;
	}

	body = (body_pos != std::string::npos) ? message.substr(body_pos, message.length() - body_pos) : "";
}

std::string net::HTTP::toString()
{
	std::string message = "";
	for (unsigned int i = 0; i < 3; i++)
		message += start_line[i] + " ";
	message.pop_back();
	message += "\r\n";
	for (auto&[key, value] : headers)
		message += key + ": " + value + "\r\n";
	return message + ((body.length()) ? ("\r\n" + body) : "");
}



net::HTTPRequest::HTTPRequest() : HTTP()
{

}

net::HTTPRequest::HTTPRequest(std::string request) : HTTP(request)
{ //method uri http-version

}



net::HTTPResponse::HTTPResponse() : HTTP()
{

}

net::HTTPResponse::HTTPResponse(std::string response) : HTTP(response)
{ //http-version status-code status-comment

}
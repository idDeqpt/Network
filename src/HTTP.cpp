#include "Network/HTTP.hpp"

#include <string>
#include <unordered_map>


net::HTTP::HTTP()
{
	body = "";
}

net::HTTP::HTTP(std::string message)
{
	start_line = {
		{"0", ""},
		{"1", ""},
		{"2", ""}
	};

	int body_pos = (message.find("\r\n\r\n\r\n") == std::string::npos) ? std::string::npos : (message.find("\r\n\r\n") + 6);
	int pointer_begin = 0;
	int pointer_end = message.find(" ");
	start_line["0"] = message.substr(0, pointer_end);
	pointer_begin = pointer_end + 1;

	pointer_end = message.find(" ", pointer_begin + 1);
	start_line["1"] = message.substr(pointer_begin, pointer_end - pointer_begin);
	pointer_begin = pointer_end + 1;

	pointer_end = message.find("\r\n", pointer_begin + 1);
	start_line["2"] = message.substr(pointer_begin, pointer_end - pointer_begin);
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
	for (auto&[key, value] : start_line)
		message += value + " ";
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
{
	start_line["method"] = start_line["0"];
	start_line.erase("0");
	start_line["uri"] = start_line["1"];
	start_line.erase("1");
	start_line["http-version"] = start_line["2"];
	start_line.erase("2");
}



net::HTTPResponse::HTTPResponse() : HTTP()
{
	
}

net::HTTPResponse::HTTPResponse(std::string response) : HTTP(response)
{
	start_line["http-version"] = start_line["0"];
	start_line.erase("0");
	start_line["status-code"] = start_line["1"];
	start_line.erase("1");
	start_line["status-comment"] = start_line["2"];
	start_line.erase("2");
}
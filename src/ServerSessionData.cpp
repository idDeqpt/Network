#include "Network/ServerSessionData.hpp"

#include <string>


net::ServerSessionData::ServerSessionData()
{
	id = -1;
}

net::ServerSessionData::ServerSessionData(int id, std::string request, std::string response)
{
	this->id = id;
	this->request = request;
	this->response = response;
}


int net::ServerSessionData::getId()
{
	return id;
}

std::string net::ServerSessionData::getRequest()
{
	return request;
}

std::string net::ServerSessionData::getResponse()
{
	return response;
}
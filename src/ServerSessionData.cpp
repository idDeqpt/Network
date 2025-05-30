#include "Network/ServerSessionData.hpp"

#include <string>


Network::ServerSessionData::ServerSessionData()
{
	id = -1;
}

Network::ServerSessionData::ServerSessionData(int id, std::string request, std::string response)
{
	this->id = id;
	this->request = request;
	this->response = response;
}


int Network::ServerSessionData::getId()
{
	return id;
}

std::string Network::ServerSessionData::getRequest()
{
	return request;
}

std::string Network::ServerSessionData::getResponse()
{
	return response;
}
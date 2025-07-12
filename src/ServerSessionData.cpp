#include "Network/ServerSessionData.hpp"

#include <string>


net::ServerSessionData::ServerSessionData()
{
	id = -1;
	type = Type::REQUEST;
	time = 0;
	text = "";
}

net::ServerSessionData::ServerSessionData(int id, Type type, float time, std::string text)
{
	this->id = id;
	this->type = type;
	this->time = time;
	this->text = text;
}


int net::ServerSessionData::getId()
{
	return id;
}

net::ServerSessionData::Type net::ServerSessionData::getType()
{
	return type;
}

float net::ServerSessionData::getTime()
{
	return time;
}

std::string net::ServerSessionData::getText()
{
	return text;
}
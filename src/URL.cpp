#include "Network/URL.hpp"

#include <string>
#include <vector>
#include <iostream>

#include "Network/Address.hpp"


Network::URI::URI()
{
	path.push_back("/");
}

Network::URI::URI(std::string uri)
{
	int params_i = uri.find("?");

	std::string path_str = (params_i == std::string::npos) ? uri : uri.substr(0, params_i);

	for (int pointer_begin = path_str.find("/"); pointer_begin < path_str.length();)
	{
		if (pointer_begin == (path_str.length() - 1))
		{
			path.push_back("");
			break;
		}

		int pointer_end = path_str.find("/", pointer_begin + 1);
		pointer_end = (pointer_end == std::string::npos) ? path_str.length() : pointer_end;
		
		path.push_back(path_str.substr(pointer_begin + 1, pointer_end - pointer_begin - 1));
		pointer_begin = pointer_end;
	}

	if (params_i == std::string::npos)
		return;

	uri += "&";
	int pointer_begin = params_i + 1;
	for (int pointer_end = uri.find("&", pointer_begin + 1); pointer_end != std::string::npos; pointer_end = uri.find("&", pointer_begin + 1))
	{
		int equel_pos = uri.find("=", pointer_begin);
		this->params[uri.substr(pointer_begin, equel_pos - pointer_begin)] = uri.substr(equel_pos + 1, pointer_end - equel_pos - 1);
		pointer_begin = pointer_end + 1;
	}
}


unsigned int Network::URI::getLength()
{
	return path.size();
}

std::unordered_map<std::string, std::string>& Network::URI::getParamsPtr()
{
	return params;
}


std::string Network::URI::toString(bool with_params)
{
	std::string result;
	for (unsigned int i = 0; i < path.size(); i++)
		result += "/" + path[i];

	if (with_params && (this->params.size() > 0))
	{
		result += "?";
		for (auto& [key, value] : this->params)
			result += key + "=" + value + "&";
		result.pop_back();
	}

	return result;
}


std::string& Network::URI::operator[](int index)
{
	return path[index];
}



Network::URL::URL()
{
	scheme = "http";
	host = "localhost";
	uri = URI();
}

Network::URL::URL(std::string scheme, std::string host)
{
	this->scheme = scheme;
	this->host = host;
	this->uri = URI();
}

Network::URL::URL(std::string scheme, std::string host, URI uri)
{
	this->scheme = scheme;
	this->host = host;
	this->uri = uri;
}

Network::URL::URL(std::string url)
{
	int pointer_begin = 0;
	int pointer_end = url.find("://");
	scheme = url.substr(0, pointer_end);
	pointer_begin = pointer_end + 3;

	pointer_end = url.find("/", pointer_begin + 1);
	if (pointer_end == std::string::npos)
	{
		url += "/";
		pointer_end = url.size() - 1;
	}
	host = url.substr(pointer_begin, pointer_end - pointer_begin);
	if (pointer_end == std::string::npos)
		return;

	pointer_begin = pointer_end;
	uri = URI(url.substr(pointer_begin, url.size() - pointer_begin));
}


void Network::URL::setScheme(std::string new_scheme)
{
	scheme = new_scheme;
}

void Network::URL::setHost(std::string new_host)
{
	host = new_host;
}

void Network::URL::setURI(URI new_uri)
{
	uri = new_uri;
}


std::string Network::URL::getScheme()
{
	return scheme;
}

std::string Network::URL::getHost()
{
	return host;
}

Network::URI Network::URL::getURI()
{
	return uri;
}


std::string Network::URL::toString()
{
	return scheme + "://" + host + uri.toString();
}
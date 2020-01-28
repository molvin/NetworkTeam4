#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Engine/Engine.h"
#include "Server.h"
#include "Client.h"
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#undef main

#define SERVER 0
#define IP "10.20.3.3"

//TODO: try all local ips for server, so we dont have to switch ips
//TODO: put connect on its own thread and keep trying until we connect

//TODO: build script for building server and client at the same time

Server* server = nullptr;
Client* client = nullptr;

//TODO
//Handle connect when server isn't up yet
//Handle disconnect on server
//Handle disconnect on client

std::string exec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);

	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}

	return result;
}

int main()
{	

	std::string Command = exec("arp -a");

	std::cout << Command << std::endl;

	std::size_t StartPos = Command.find("Type");
	//StartPos = Command.find("Type", StartPos + 1);5
	if (StartPos != std::string::npos)
		std::cout << StartPos << "\n\n\n";

	Command.erase(0, StartPos + 5);

	//std::list<int, std::string> Ip;
	std::vector<std::string> ips;
	std::string IpSave;
	int IpCount = 0;
	int IpPos = 1;
	bool stringEnd = false;

	// TODO: Save in a list
	while (!stringEnd)
	{
		for (int i = 2; i < 57; i++)
		{
			if (Command[i] != ' ')
			{
				IpSave.resize(IpPos, Command[i]);
				IpPos++;
			}
			else
			{
				i = 56;
				if (Command[i] == '\n')
				{
					IpCount++;
					IpPos = 1;

					std::size_t EndPos = Command.find("static");
					if (EndPos <= 57)
						stringEnd = true;

					//if (Command[i + 1] == '\0')

					break;
				}
			}
		}

		std::cout << IpSave << "\n";
		ips.push_back(IpSave);
		//IpSave = "";
		Command.erase(0, 57);
	}


	engineInit(SERVER);

	if (SERVER)
	{
		server = new Server();
		server->SocketClient.OnConnection = std::bind(&Server::OnConnect, server, std::placeholders::_1);

	}
	else
	{
		client = new Client();
		client->Join(IP, 50000);

	}


	while(true)
	{
		if (SERVER)
		{
			server->Update();
		}
		else
		{
			client->Update();
			if (!engIsOpen())
				break;
		}

	}
	
	if(SERVER)
	{
		server->SocketClient.Close();
	}
	else
	{
		client->SocketClient.Close();
	}

	return 0;
}

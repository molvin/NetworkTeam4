#include <cstdlib>
#include "Networking/NetworkClient.h"
#include"Networking/BinaryStream.hpp"
#include "Test/TestMessage.h"
#include "Test/OtherTestMessage.h"

int main()
{	
	TestMessage message0;
	message0.TestInt = 12;
	OtherTestMessage message1;
	message1.Value = 3.14f;

	TestMessage* reg = new TestMessage();
	OtherTestMessage* regother = new OtherTestMessage();

	
	NetworkClient client(50000);
	NetworkClient server(60000);

	server.RegisterMessage((Message*)reg, MessageType::Test);
	client.RegisterMessage((Message*)reg, MessageType::Test);
	server.RegisterMessage((Message*)regother, MessageType::OtherTest);
	client.RegisterMessage((Message*)regother, MessageType::OtherTest);

	while(true)
	{
		server.AddMessageToQueue((Message*)&message0, MessageType::Test);
		server.AddMessageToQueue((Message*)&message1, MessageType::OtherTest);
		server.SendData();
	}

	client.Close();
	server.Close();

	return 0;
}

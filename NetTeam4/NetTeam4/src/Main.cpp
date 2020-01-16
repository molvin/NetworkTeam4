#include <cstdlib>
#include "Networking/NetworkClient.h"
#include"Networking/BinaryStream.hpp"

int main()
{	
	BinaryStream stream;
	stream.Write<int>(6);
	stream.Write<bool>(true);

	printf("%d\n", stream.Cursor);

	system("PAUSE");


	NetworkClient client(5555);
	NetworkClient server(5555);
	while(true)
	{
		server.SendData(&stream.Buffer[0], stream.Size());
	}

	client.Close();
	server.Close();
	return 0;
}

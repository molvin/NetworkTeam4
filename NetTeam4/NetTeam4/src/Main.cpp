#include <cstdlib>
#include "Networking/NetworkClient.h"

int main()
{
	NetworkClient client(5555);
	NetworkClient server(5555);
	
	while(true)
	{
		
	}

	client.Close();
	server.Close();
	return 0;
}

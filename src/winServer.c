#include "winServer.h"
#include "strutils.h"

#include <stdio.h>
#include <stdlib.h>

SOCKET winServerInit (unsigned port)
{
	WSADATA wsa;
	SOCKET soc;
	struct sockaddr_in server;

	printf("Initializing Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2), &wsa)) 
	{
		printf("Failed. Error Code: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	printf("Creating socket...\n");
	if ((soc = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		printf("Failed. Error Code: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	// Setup server
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Binding socket
	printf("Binding socket...\n");
	if (bind(soc, (struct sockaddr*) &server, sizeof(server))) 
	{
		printf("Failed. Error Code: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	if (listen(soc, 0x10)) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	return soc;
}

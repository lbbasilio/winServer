#include "winServer.h"
#include "strutils.h"

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 0x0FFF

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

char* winServerReadFile (char* filename, int* fileSize)
{
	FILE* inputFile;
	char* fileContent;

	char filePath[1024];	
	enum {HTML, CSS, JS, IMG} fileType;

	// TODO: implement strFindLastOf
	// so it can find files with .'s
	// in the name
	
	int pos = strFindFirstOf (filename, ".", 0);
	if (pos == -1) fileType = HTML;
	else
	{
		char* format = 	strSubstr (filename, pos, 1024); 
		if (strcmp(format, ".js") == 0) fileType = JS;
		else if (strcmp(format, ".css") == 0) fileType = CSS;
		else if (strcmp(format, ".html") == 0) fileType = HTML;
		else fileType = IMG;
		free(format);
	}

	switch (fileType)
	{
		case HTML:	strcpy(filePath, "./html/"); break;
		case CSS:	strcpy(filePath, "./css/");  break;
		case JS:	strcpy(filePath, "./js/");	 break;
		case IMG:	strcpy(filePath, "./img/");	 break;
	}

	strCat(filePath, filename);
	inputFile = fopen(filePath, "rb");

	if (inputFile)
	{
		// Get file size
		fseek(inputFile, 0, SEEK_END);
		size_t size = ftell(inputFile);
		fileContent = malloc(size + 1);	
		*fileSize = size;

		// Read file
		rewind(inputFile); 
		fread((void*) fileContent, sizeof(char), size, inputFile);
		fclose(inputFile);

		// Add NUL termination
		fileContent[size] = '\0';

	}
	else
	{
		printf ("Unable to open file %s", filePath);
		fileContent = NULL;		
		*fileSize = 0;
	}

	return fileContent;
}

char* winServerRcv (SOCKET connection, int* status)
{
	char* rcvLine = malloc(BUFFER_SIZE * sizeof(char));
	memset(rcvLine, 0, BUFFER_SIZE);		

	int n = 0;
	while ((n = recv(connection, rcvLine, BUFFER_SIZE-1, 0)) > 0)
	{
		printf("%s\n", rcvLine);		
		if (rcvLine[n - 1] == '\n') break;
		memset(rcvLine, 0, BUFFER_SIZE);
	}

	if (n < 0)
	{
		printf("Error while reading message. CODE: %d", WSAGetLastError());
		*status = -1;
		free(rcvLine);
		return NULL;
	}

	*status = 0;
	return rcvLine;
}

char* winServerProcessRequest (char* rcvLine, int* rspSize)
{
	// TODO: http request processing
	
	
	// Read requested file
	int tokenCount;
	char** tokens = strSplit (rcvLine, " \t\r\n", &tokenCount);
	char file[255];
	if (tokens)
	{
		int pos = strFindFirstOf (tokens[1], ".", 0);
		if (pos == -1)
		{
			if (strcmp(tokens[1], "/") == 0) strcpy(file, "/index.html");
			else strcpy(file, tokens[1]);
		}
		else strcpy(file, tokens[1]);
	}

	// Read file and generate response
	int size;
	char* payload = winServerReadFile (file, &size);
	char* rspLine = NULL;
	*rspSize = 0;
	if (size)
	{
		rspLine = winServerCreateResponse(tokens, tokenCount, size);
		*rspSize = strlen(rspLine) + size + 1;
		memmove(rspLine + strlen(rspLine), payload, size + 1);	
	}

	// Cleanup
	int i;
	for (i = 0; i < tokenCount; ++i) free(tokens[i]);
	free(tokens);
	free(payload);

	return rspLine;
}

void winServerLoop (SOCKET soc)
{
	for (;;)
	{
		SOCKET connection = accept(soc, NULL, NULL);

		// Receive request
		int status;
		char* rcvLine = winServerRcv (connection, &status);
		if (status) continue;
		
		fprintf(stdin, "\n%s", rcvLine);
		// Process Request
		int rspSize;
		char* rspLine = winServerProcessRequest(rcvLine, &rspSize);
		if (!rspLine) continue;
		// TODO: add 404 page

		// Respond to request
		send(connection, rspLine, rspSize, 0);
		closesocket(connection);

		free(rcvLine);
		free(rspLine);
	}

	WSACleanup();
}

// TODO: change winServerReadFile to receive file type
// and determine fileType inside winServerCreateResponse
char* winServerCreateResponse(char** tokens, int tokenCount, int payloadSize)
{
	char headerType[255];
	enum {HTML, CSS, JS, IMG} fileType;

	tokenCount;
	// TODO: implement strFindLastOf
	// so it can find files with .'s
	// in the name
	
	int pos = strFindFirstOf (tokens[1], ".", 0);
	if (pos == -1) fileType = HTML;
	else
	{
		char* format = 	strSubstr (tokens[1], pos, 1024); 
		if (strcmp(format, ".js") == 0) fileType = JS;
		else if (strcmp(format, ".css") == 0) fileType = CSS;
		else if (strcmp(format, ".html") == 0) fileType = HTML;
		else fileType = IMG;
		free(format);
	}

	switch (fileType)
	{
		case HTML: strcpy(headerType, "text/html"); break;
		case CSS: strcpy(headerType, "text/css"); break;
		case JS: strcpy(headerType, "text/javascript; charset=UTF-8"); break;
		case IMG: strcpy(headerType, "image/x-icon"); break;
	}

	char header[1024];
	sprintf(header, "HTTP/1.1 200 OK\r\ncontent-encoding: identity\r\ncontent-length: %d\r\ncontent-type: %s\r\n\r\n", payloadSize, headerType);
	int fullSize = strlen(header) + payloadSize; 
	char* buffer = malloc(fullSize + 1); // NUL termination
	strcpy(buffer, header);
	
	return buffer;
}

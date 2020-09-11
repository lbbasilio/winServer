
#include <stdio.h>
#include <stdlib.h>

#ifdef __WIN32__
#include <winsock2.h>
#endif

#include "strutils.h"

#define PORT 2000
#define BUFFSIZE 4096

SOCKET initialize (int port) {

	WSADATA wsa;
	SOCKET soc;
	struct sockaddr_in server;

	printf("Initializing Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2), &wsa)) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}

	printf("Creating socket...\n");
	if ((soc = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}

	// Setup server
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Binding socket
	printf("Binding socket...\n");
	if (bind(soc, (struct sockaddr*) &server, sizeof(server))) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}

	if (listen(soc, 10)) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}

	return soc;
}

enum fileType {NONE, HTML, CSS, IMG, SCRIPT};
typedef struct {
	char* content;
	int size;
} fileContent;

fileContent readFile (char* filename, enum fileType type) {

	FILE* inputFile;
	char filepath[255];
	
	switch(type) {
		case HTML: strcpy(filepath, "./html"); break;
		case CSS: strcpy(filepath, "./css"); break;
		case IMG: strcpy(filepath, "./img"); break;
		case SCRIPT: strcpy(filepath, "./scripts"); break;
		default: break;
	}

	strcat(filepath, filename);
	inputFile = fopen(filepath, "rb");

	// Get number of characters
	fseek(inputFile, 0, SEEK_END);
	size_t size = ftell(inputFile);
	char* text = (char*) malloc(size + 1);
	
	// Read file
	rewind(inputFile);
	fread((void*) text, sizeof(char), size, inputFile);
	fclose(inputFile);

	// Add NUL termination
	text[size] = 0;

	// Set return value
	fileContent retVal;
	retVal.content = text;
	retVal.size = size; 
	
	return retVal;
}

int main () {


	SOCKET soc;
	soc = initialize(PORT);

	for (;;) {

		SOCKET connection;
		char rcvLine[BUFFSIZE];

		printf("Waiting for connection on port %d\n", PORT);
		fflush(stdout);
		connection = accept(soc, NULL, NULL);

		printf("Reading messages...\n");
		memset(rcvLine, 0, BUFFSIZE);
		int n = 0;
		while ((n = recv(connection, rcvLine, BUFFSIZE-1, 0)) > 0) {
			fprintf(stdout, "\n%s\n", rcvLine);
			if (rcvLine[n - 1] == '\n') break;
			memset(rcvLine, 0, BUFFSIZE);
		}

		if (n < 0) {
			printf("Failed. Error Code: %d", WSAGetLastError());
			exit(1);
		}


		strTokens lines = split(rcvLine, '\n');
		if (lines.strings == NULL) continue;

		strTokens tokens = split(lines.strings[0], ' ');

		fileContent htmlFile;

		char contType[255];
		int pos = findFirstOf (tokens.strings[1], '.', 0);
		if (pos == -1) {

			if (strcmp(tokens.strings[1], "/") == 0) htmlFile = readFile("/index.html", HTML);
			else {
				char aux[255];
				strcpy(aux, tokens.strings[1]);
				strcat(aux, ".html");
				htmlFile = readFile(aux, HTML);
			}

			strcpy(contType, "text/html");

		} else {

			char* format = substr(tokens.strings[1], pos, 0);
			if (strcmp(format, ".js") == 0) {
			
				htmlFile = readFile(tokens.strings[1], SCRIPT);
				strcpy(contType, "text/javascript; charset=UTF-8");

			} else if (strcmp(format, ".css") == 0) {
			
				htmlFile = readFile(tokens.strings[1], CSS);
				strcpy(contType, "text/css");		
			
			} else {

				htmlFile = readFile(tokens.strings[1], IMG);
				strcpy(contType, "image/x-icon");

			}
		}

		// Respond
		char header[255]; 
		sprintf(header, "HTTP/1.1 200 OK\r\ncontent-encoding: identity\r\ncontent-length: %d\r\ncontent-type: %s\r\n\r\n" , htmlFile.size, contType); 
		int stringSize = strlen(header) + htmlFile.size;
		char* buffer = malloc(stringSize + 1);

		strcpy(buffer, header);
		//strcat(buffer, htmlFile.content);
		memmove(buffer + strlen(header), htmlFile.content, htmlFile.size + 1);
		printf("\n%s\n", buffer);

		send(connection, buffer, stringSize, 0);
		closesocket(connection);

		// Free memory
		free(buffer);
		free(htmlFile.content);

		int k;
		for (k = 0; k < lines.num; ++k) free(lines.strings[k]);
		for (k = 0; k < tokens.num; ++k) free(tokens.strings[k]);
		free(lines.strings);
		free(tokens.strings);
	}

	WSACleanup();
	return 0;
}

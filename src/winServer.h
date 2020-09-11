#ifndef WINSERVER_H
#define WINSERVER_H

#include <winsock2.h>

SOCKET winServerInit (unsigned port);
char* winServerReadFile (char* filename, int* fileSize);
void winServerLoop (SOCKET soc);
char* winServerCreateResponse(char** tokens, int tokenCount, int payloadSize);

#endif

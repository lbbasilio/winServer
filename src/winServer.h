#ifndef WINSERVER_H
#define WINSERVER_H

#include <winsock2.h>

SOCKET winServerInit (unsigned port);
char* winServerRcv (SOCKET connection, int* status);
char* winServerProcessRequest (char* rcvLine, int* rspSize);
char* winServerReadFile (char* filename, int* fileSize);
char* winServerCreateResponse (char** tokens, int tokenCount, int payloadSize);

void winServerLoop (SOCKET soc);

#endif

#include "winServer.h"

int main()
{
	SOCKET soc = winServerInit (3000);
	winServerLoop (soc);
	return 0;
}

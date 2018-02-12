#include "tcp.h"

int main(void) {
	Tribal::SocketConfig sc;
	SOCKET sock = null;

	sc.RegisterOption(sock, IPPROTO_TCP, TCP_NODELAY, null, 0);
	return 0;
}
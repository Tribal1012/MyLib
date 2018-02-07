#pragma once
#include <WinSock2.h>
#include "type.h"
#include "define.h"

#define LOCALHOST "127.0.0.1"
#define ADDR_LEN  16

typedef puint8 IP_ADDR;
typedef uint16 PORT_ADDR;

namespace Tribal {
	static uint8 localhost[] = LOCALHOST;

	class TCPObject {
	private:
		uint8	    ip[16];
		PORT_ADDR   port;

	protected:
		SOCKET      sock;

	private:
		/*
		   Create a socket.
		   If socket() return error, this function will return FALSE.
		 */
		bool CreateSocket() {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			return (sock != -1) ? true : false;
		}

	protected:
		/*
		   This function used for store IP Address and Port Address in TCPObject.
		   But it's use isn't implemented.
		 */
		void SetAddress(IP_ADDR ip, PORT_ADDR port) {
			puint8 src = (ip != null) ? ip : localhost;		// source pointer
			puint8 dst = this->ip;							// destination pointer

			for (uint32 i = 0; i < ADDR_LEN; i++)
				*dst++ = *src++;

			this->port = port;
		}

	public:
		TCPObject() {
			puint8 ptr = this->ip;

			for (uint32 i = 0; i < ADDR_LEN; i++)
				*ptr++ = 0;

			this->port = 0;

			while (!CreateSocket());
		}

		virtual ~TCPObject() {}

		/* Get socket's value from TCPOBJECT */
		SOCKET GetSocket() const {
			return sock;
		}

		/* Send data with TCPOBJECT */
		uint32 SendTCPData(ccData data, uint32 len) const {
			return send(sock, data, len, 0);
		}

		/* Recieve data with TCPOBJECT */
		uint32 RecvTCPData(cData data, cuint32 max) {
			return recv(sock, data, max, 0);
		}

		/* Close socket for close the connection*/
		void CloseSocket() {
			closesocket(sock);
		}

		/* 
		   for setsockopt(Broadcast),
		   Broadcast(MSDN) : Configures a socket for sending broadcast data.
		 */
		TCPObject* SetBroadcast() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, null, 0);

			return this;
		}

#ifdef _DEBUG
		/* 
		   for setsockopt(Debug), This function will only execute on Debug mode.
		   Debug(MSDN) : Enables debug output. Microsoft providers currently do not output any debug information.
		 */
		TCPObject* SetDebug() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_DEBUG, null, 0);

			return this;
		}
#endif
		/*
		for setsockopt(Linger).
		LINGER(MSDN) : Lingers on close if unsent data is present.
		*/
		TCPObject* SetLinger() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_LINGER, null, 0);

			return this;
		}

		/* 
		   for setsockopt(Don't Linger).
		   DONTLINGER(MSDN) : Does not block close waiting for unsent data to be sent. 
		                      Setting this option is equivalent to setting SO_LINGER with l_onoff set to zero.
		 */
		TCPObject* SetDontLinger() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, null, 0);

			return this;
		}

		/*
		   for setsockopt(Don't Route).
		   DONTROUTE(MSDN) : Sets whether outgoing data should be sent on interface the socket is bound to and not a routed on some other interface. 
		                     This option is not supported on ATM sockets (results in an error).
		 */
		TCPObject* SetDontRoute() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_DONTROUTE, null, 0);

			return this;
		}

		/*
		   for setsockopt(OOB in Line).
		   OOBINLINE(MSDN) : Indicates that out-of-bound data should be returned in-line with regular data. 
		                     This option is only valid for connection-oriented protocols that support out-of-band data. 
							 For a discussion of this topic, see Protocol Independent Out-Of-band Data.
		 */
		TCPObject* SetOOBinLine() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_OOBINLINE, null, 0);

			return this;
		}

		/*
		   for setsockopt(KeepAlive).
		   KEEPALIVE(MSDN) : Enables sending keep-alive packets for a socket connection. 
		                     Not supported on ATM sockets (results in an error).
		 */
		TCPObject* SetKeepAlive() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, null, 0);

			return this;
		}

		/*
		   for setsockopt(RecvBuf).
		   RCVBUF(MSDN) : Specifies the total per-socket buffer space reserved for receives.
		 */
		TCPObject* SetRecvBuf() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, null, 0);

			return this;
		}

		/*
		   for setsockopt(SendBuf).
		   SNDBUF(MSDN) : Specifies the total per-socket buffer space reserved for sends.
		 */
		TCPObject* SetSendBuf() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, null, 0);

			return this;
		}

		/*
		   for setsockopt(ResueAddr).
		   REUSEADDR(MSDN) : Allows the socket to be bound to an address that is already in use.
		                     For more information, see bind. Not applicable on ATM sockets.
		 */
		TCPObject* SetReuseAddr() {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, null, 0);

			return this;
		}

		/*
		   Create TCPServer Object, And It is returned with TCPObject type. 
		 */
		static TCPObject* CreateTCPServer() {
			return new TCPServer();
		}

		/*
		Create TCPClient Object, And It is returned with TCPObject type.
		*/
		static TCPObject* CreateTCPClient() {
			return new TCPClient();
		}
	};

	class TCPServer : public TCPObject {
	private:
		SOCKET clnt_sock;

	public:
		TCPServer() :TCPObject() {}

		~TCPServer() {}

		/*
		   execute Socket API function(bind(), listen(), accept()).
		   This function require server's Port Address for bind.
		 */
		void Start(PORT_ADDR port) {
			sockaddr_in sa;
			sockaddr_in clnt_sa;
			uint32 clnt_sa_size = 0;
			uint32 result = 0;

			ZeroMemory(&sa, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			sa.sin_port = htons(port);
			
			result = bind(sock, (sockaddr*)&sa, sizeof(sockaddr_in));
			if (result == -1) {
				return;
			}

			SetAddress(null, port);

			result = listen(sock, CLIENTCONNECT);
			if (result == -1) {
				return;
			}

			ZeroMemory(&clnt_sa, sizeof(sockaddr_in));
			clnt_sock = accept(sock, (sockaddr*)&clnt_sa, (int*)&clnt_sa_size);
			if (result == -1) {
				return;
			}
		}
	};

	class TCPClient : public TCPObject {
	private:
	public:
		TCPClient() :TCPObject() {}

		~TCPClient() {}

		/*
		   execute Socket API function(connect()).
		   This function require target's IP Address and Port Address for connect.
		 */
		void Start(IP_ADDR ip, PORT_ADDR port) {
			sockaddr_in sa;
			uint32 result = 0;

			ZeroMemory(&sa, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_addr.S_un.S_addr = inet_addr((const char*)ip);
			sa.sin_port = htons(port);

			result = connect(sock, (sockaddr*)&sa, sizeof(sockaddr_in));
			if (result == -1) {
				return;
			}

			SetAddress(ip, port);
		}
	};
}
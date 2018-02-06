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
		bool CreateSocket() {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			return (sock != -1) ? true : false;
		}

	protected:
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

		SOCKET GetSocket() const {
			return sock;
		}

		uint32 SendTCPData(ccData data, uint32 len) const {
			send(sock, data, len, 0);
		}

		uint32 RecvTCPData(cData data, cuint32 max) {
			recv(sock, data, max, 0);
		}

		void CloseSocket() {
			closesocket(sock);
		}

		static TCPObject* CreateTCPServer() {
			return new TCPServer();
		}

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

		void Start(IP_ADDR ip, PORT_ADDR port) {
			sockaddr_in sa;
			sockaddr_in clnt_sa;
			uint32 clnt_sa_size = 0;
			uint32 result = 0;

			ZeroMemory(&sa, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_addr.S_un.S_addr = inet_addr((const char*)ip);
			sa.sin_port = htons(port);
			
			result = bind(sock, (sockaddr*)&sa, sizeof(sockaddr_in));
			if (result == -1) {
				return;
			}

			SetAddress(ip, port);

			result = listen(sock, 5);
			if (result == -1) {
				return;
			}

			ZeroMemory(&clnt_sa, sizeof(sockaddr_in));
			clnt_sock = accept(sock, (sockaddr*)&clnt_sa, (int*)&clnt_sa_size);
			if (result == -1) {
				return;
			}
		}

		void SetSocketOption(uint32 flag) {}
	};

	class TCPClient : public TCPObject {
	private:
	public:
		TCPClient() :TCPObject() {}

		~TCPClient() {}

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

		void SetSocketOption(uint32 flag) {}
	};
}
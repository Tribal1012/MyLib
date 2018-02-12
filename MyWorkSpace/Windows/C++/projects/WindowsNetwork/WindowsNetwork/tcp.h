#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "type.h"
#include "define.h"
#include "socket.h"

#define LOCALHOST "127.0.0.1"
#define ADDR_LEN  16

typedef puint8 IP_ADDR;
typedef uint16 PORT_ADDR;

namespace Tribal {
	static uint8 localhost[] = LOCALHOST;

	class TCPSocket :public Socket {
	protected:
		sockaddr_in m_sa;

	protected:
		/*
		Create a socket.
		If socket() return error, this function will return -1.
		*/
		SOCKET CreateSocket() override {
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			return m_sock;
		}
		
		void CloseSocket() override {
			closesocket(m_sock);
		}

		uint32 SendData(ccData data, cuint32 len) override {
			return send(m_sock, data, len, 0);
		}

		uint32 RecvData(cData data, cuint32 max) override {
			return recv(m_sock, data, max, 0);
		}

	public:
		TCPSocket():Socket() {
			ZeroMemory(&m_sa, sizeof(sockaddr_in));
		}

		virtual ~TCPSocket() {}

		// send override
		// recv override
	};

	class TCPServer :public TCPSocket {
	private:
		SOCKET m_clnt_sock;
		sockaddr_in m_clnt_sa;

	private:
		TCPServer() :TCPSocket() {
			ZeroMemory(&m_clnt_sa, sizeof(sockaddr_in));
		}

		/*
		    execute Socket API function(bind(), listen()).
		    This function require server's Port Address with own socket for bind.
		 */
		bool ListenServer(PORT_ADDR port) {
			uint32 result = 0;

			m_sa.sin_family = AF_INET;
			m_sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			m_sa.sin_port = htons(port);

			result = bind(m_sock, (sockaddr*)&m_sa, sizeof(sockaddr_in));
			if (result == -1) {
				return false;
			}

			result = listen(m_sock, CLIENTCONNECT);
			if (result == -1) {
				return false;
			}

			return true;
		}

	public:
		~TCPServer() {}

		/*
		    execute Socket API function(accept()).
			server is blocked for accept.
		 */
		void Operate() {
			uint32 clnt_sa_size = 0;

			m_clnt_sock = accept(m_sock, (sockaddr*)&m_clnt_sa, (int*)&clnt_sa_size);
			if (m_clnt_sock == -1) return;
		}

		/*
		    return TCPServer's object
		 */
		static Socket* CreateServer(PORT_ADDR port) {
			//TCPServer* pInstance = GetInstance<TCPServer>();
			//pInstance->ListenServer(port);
			TCPServer* pInstance;
			return pInstance;
		}
	};

	class TCPClient : public TCPSocket {
	private:
		TCPClient() :TCPSocket() {}

	public:
		~TCPClient() {}

		/*
		    execute Socket API function(connect()).
		    This function require target's IP Address and Port Address with own socket for connect.
		 */
		void remote(IP_ADDR ip, PORT_ADDR port) {
			uint32 result = 0;

			m_sa.sin_family = AF_INET;
#ifdef _WINSOCK_DEPRECATED_NO_WARNINGS
			m_sa.sin_addr.S_un.S_addr = inet_addr((const char*)ip);
#else
			InetPton(AF_INET, (PCWSTR)ip, &m_sa.sin_addr.S_un.S_addr);
#endif
			m_sa.sin_port = htons(port);

			result = connect(m_sock, (sockaddr*)&m_sa, sizeof(sockaddr_in));
			if (result == -1) return;
		}
	};
}
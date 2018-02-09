#pragma once
#include <WinSock2.h>
#include "type.h"
#include "define.h"
#include "socket.h"

#define LOCALHOST "127.0.0.1"
#define ADDR_LEN  16

typedef puint8 IP_ADDR;
typedef uint16 PORT_ADDR;

namespace Tribal {
	static uint8 localhost[] = LOCALHOST;

	class TCPObject {
	protected:
		uint8	         m_ip[16];
		PORT_ADDR        m_port;
		TCPSocketAction* m_action;
		SOCKET           m_sock;

	protected:
		/*
		   This function used for store IP Address and Port Address in TCPObject.
		   But it's use isn't implemented.
		 */
		void SetAddress(IP_ADDR ip, PORT_ADDR port) {
			puint8 src = (ip != null) ? ip : localhost;		// source pointer
			puint8 dst = this->m_ip;							// destination pointer

			for (uint32 i = 0; i < ADDR_LEN; i++)
				*dst++ = *src++;

			this->m_port = port;
		}

	public:
		TCPObject(IP_ADDR ip, PORT_ADDR port) {
			this->m_action = TCPSocketActionObject::GetInstance();
			this->m_sock = this->m_action->CreateSocket();

			SetAddress(ip, port);
		}

		virtual ~TCPObject() {
			CLEANUPOBJECT(m_action)
		}

		virtual bool execute() = 0;
	};

	class TCPServer : public TCPObject {
	private:
		SOCKET m_clnt_sock;

	public:
		TCPServer(IP_ADDR ip, PORT_ADDR port) :TCPObject(ip, port) {}
		~TCPServer() {}

		bool execute() override {
			m_clnt_sock = m_action->OperateServer(m_sock, m_port);
			if (m_clnt_sock == -1) return false;
		}
	};

	class TCPClient : public TCPObject {
	private:
	public:
		TCPClient(IP_ADDR ip, PORT_ADDR port) :TCPObject(ip, port) {}
		~TCPClient() {}

		bool execute() override {
			SOCKET tmp;

			tmp = m_action->OperateClient(m_sock, (char*)m_ip, m_port);
			if (tmp == -1) return false;
		}
	};
}
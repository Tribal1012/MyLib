#pragma once
#include <WinSock2.h>
#include <map>
#include "type.h"
#include "define.h"

namespace Tribal {
	/*
	   Create TCPSocketAction object for TCP Object.
	   This class only can call GetInstance().
	 */
	class TCPSocketActionObject:public Singleton<TCPSocketAction> {
	public:
		TCPSocketActionObject() :Singleton() {}
		~TCPSocketActionObject() {}
	};

	/*
	   Create SocketConfig object for TCPSocketAction object.
	   This class only can call GetInstance().
	 */
	class SocketConfigObject :public Singleton<SocketConfig> {
	public:
		SocketConfigObject() :Singleton() {}
		~SocketConfigObject() {}
	};

	/*
	   TCPSocketAction have command pattern.
	   because TCPSocketAction should handle the work of the server and the client separately. 
	 */
	class TCPSocketAction {
	private:
		SocketConfig* m_sockconfig;     // for handle setsockopt()

	private:
		void HandleActionError() {
			uint32 errorcode = GetLastError();
		}

	public:
		TCPSocketAction() {
			m_sockconfig = SocketConfigObject::GetInstance();
		}

		// It is not support multithread processes.
		~TCPSocketAction() {
			CLEANUPOBJECT(m_sockconfig)
		}

		/*
		Create a socket.
		If socket() return error, this function will return -1.
		*/
		SOCKET CreateSocket() {
			SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			return sock;
		}

		/* Close socket for close the connection*/
		void CloseSocket(SOCKET sock) {
			closesocket(sock);
		}

		/* Send data with TCPOBJECT */
		uint32 SendData(SOCKET sock, ccData data, uint32 len) const {
			return send(sock, data, len, 0);
		}

		/* Recieve data with TCPOBJECT */
		uint32 RecvData(SOCKET sock, cData data, cuint32 max) {
			return recv(sock, data, max, 0);
		}

		/*
		   execute Socket API function(bind(), listen(), accept()).
		   This function require server's Port Address with own socket for bind.
		 */
		SOCKET OperateServer(SOCKET sock, uint32 port) {
			sockaddr_in sa;
			sockaddr_in clnt_sa;
			uint32 clnt_sa_size = 0;
			uint32 result = 0;
			SOCKET clnt_sock;

			ZeroMemory(&sa, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			sa.sin_port = htons(port);

			result = bind(sock, (sockaddr*)&sa, sizeof(sockaddr_in));
			if (result == -1) {
				return;
			}

			result = listen(sock, CLIENTCONNECT);
			if (result == -1) {
				return;
			}

			ZeroMemory(&clnt_sa, sizeof(sockaddr_in));
			clnt_sock = accept(sock, (sockaddr*)&clnt_sa, (int*)&clnt_sa_size);

			return clnt_sock;
		}

		/*
		   execute Socket API function(connect()).
		   This function require target's IP Address and Port Address with own socket for connect.
		 */
		SOCKET OperateClient(SOCKET sock, char* ip, uint32 port) {
			sockaddr_in sa;
			uint32 result = 0;

			ZeroMemory(&sa, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_addr.S_un.S_addr = inet_addr((const char*)ip);
			sa.sin_port = htons(port);

			result = connect(sock, (sockaddr*)&sa, sizeof(sockaddr_in));

			return sock;
		}
	};

/*
   Socketoption macro for SocketConfig object.
   It mean level that setsockopt() function have.
   (https://msdn.microsoft.com/en-us/library/windows/desktop/ms740476(v=vs.85).aspx).
 */
#define Socket_Level 0x0                // SOL_SOCKET
#define IPProto      0x1                // IPPROTO_TCP
#define NSProto      0x2                // NSPROTO_IPX

 /*
 Socketoption macro for SocketConfig object.
 It mean option that setsockopt() function have.
 (https://msdn.microsoft.com/en-us/library/windows/desktop/ms740476(v=vs.85).aspx).
 */
#define Broadcast               0x0     // SO_BROADCAST
#define Conditional_Accept      0x1     // SO_CONDITIONAL_ACCEPT
#define Debug                   0x2     // SO_DEBUG
#define Linger                  0x3     // SO_LINGER
#define DontLinger              0x4     // SO_DONTLINGER
#define DontRoute               0x5     // SO_DONTROUTE
#define KeepAlive               0x6     // SO_KEEPALIVE
#define OOBinLine               0x7     // SO_OOBINLINE
#define RCVBUF                  0x8     // SO_RCVBUF
#define SNDBUF                  0x9     // SO_SNDBUF
#define ReuseAddr               0xA     // SO_REUSEADDR
#define ExclusiveAddrUse        0xB     // SO_EXCLUSIVEADDRUSE
#ifdef TIME0
	#define RCVTIME0                0xC     // SO_RCVTIME0
	#define SNDTIME0                0xD     // SO_SNDTIME0
#endif
#ifdef EXIST_ACCEPT_CONTEXT
	#define Update_Accept_Context   0xE     // SO_UPDATE_ACCEPT_CONTEXT
#endif
#define PVD_Config              0xF     // PVD_CONFIG

#define NoDelay                 0x10    // TCP_NODELAY

#ifdef EXIST_IPX
	#define PType                   0x20    // IPX_PTYPE
	#define FilterType              0x21    // IPX_FILTERTYPE
	#define StopFilterType          0x22    // IPX_STOPFILTERPTYPE
	#define DSType                  0x23    // IPX_DSTYPE
	#define Extended_Address        0x24    // IPX_EXTENDED_ADDRESS
	#define RecvHDR                 0x25    // IPX_RECVHDR
	#define RCVBroadcast            0x26    // IPX_RECEIVE_BROADCAST
	#define ImmeditatesPXACK        0x27    // IPX_IMMEDIATESPXACK
#endif

	/*
	   The SocketConfig class is for setsockopt() function.
	   It use only setsockopt using flag. 
	 */
#ifdef EXIST_IPX
	#define MAX_LEVEL_COUNT  3
#else
	#define MAX_LEVEL_COUNT  2
#endif

/*
#ifdef EXIST_IPX
	#ifdef TIME0
		#ifdef EXIST_ACCEPT_CONTEXT
			#define MAX_OPTION_COUNT 25
		#else
			#define MAX_OPTION_COUNT 24
		#endif
	#else
		#ifdef EXIST_ACCEPT_CONTEXT
			#define MAX_OPTION_COUNT 23
		#else
			#define MAX_OPTION_COUNT 22
		#endif
	#endif
#else
	#ifdef TIME0
		#ifdef EXIST_ACCEPT_CONTEXT
			#define MAX_OPTION_COUNT 18
		#else
			#define MAX_OPTION_COUNT 17
		#endif
	#else
		#ifdef EXIST_ACCEPT_CONTEXT
			#define MAX_OPTION_COUNT 16
		#else
			#define MAX_OPTION_COUNT 15
		#endif
	#endif
#endif
*/
/* for mOption array's offset */
#define MAX_OPTION_COUNT 25

	class SocketConfig {
	private:
		const int32 mLevel[MAX_LEVEL_COUNT] = { 
			SOL_SOCKET, IPPROTO_TCP
#ifdef EXIST_IPX
			, NSPROTO_IPX
#endif
		};
		const int32 mOption[MAX_OPTION_COUNT] = {
			SO_BROADCAST,       SO_CONDITIONAL_ACCEPT, SO_DEBUG,                 SO_LINGER,
			SO_DONTLINGER,      SO_DONTROUTE,          SO_KEEPALIVE,             SO_OOBINLINE,
			SO_RCVBUF,          SO_SNDBUF,             SO_REUSEADDR,             SO_EXCLUSIVEADDRUSE,
#ifdef TIME0
			SO_RCVTIME0,        SO_SNDTIME0,
#else
			0, 0,
#endif
#ifdef EXIST_ACCEPT_CONTEXT
			SO_UPDATE_ACCEPT_CONTEXT, 
#else
			0,
#endif
			PVD_CONFIG,	TCP_NODELAY
#ifdef EXIST_IPX
			, IPX_PTYPE,        IPX_FILTERTYPE,        IPX_STOPFILTERPTYPE,
			IPX_DSTYPE,         IPX_EXTENDED_ADDRESS,  IPX_RECVHDR,              IPX_RECEIVE_BROADCAST,
			IPX_IMMEDIATESPXACK
#endif
		};

	private:
		/*
		for setsockopt(Broadcast),
		Broadcast(MSDN) : Configures a socket for sending broadcast data.
		*/
		SocketConfig* SetBroadcast(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, null, 0);

			return this;
		}

#ifdef _DEBUG
		/*
		for setsockopt(Debug), This function will only execute on Debug mode.
		Debug(MSDN) : Enables debug output. Microsoft providers currently do not output any debug information.
		*/
		SocketConfig* SetDebug(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_DEBUG, null, 0);

			return this;
		}
#endif
		/*
		for setsockopt(Linger).
		LINGER(MSDN) : Lingers on close if unsent data is present.
		*/
		SocketConfig* SetLinger(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_LINGER, null, 0);

			return this;
		}

		/*
		for setsockopt(Don't Linger).
		DONTLINGER(MSDN) : Does not block close waiting for unsent data to be sent.
		                   Setting this option is equivalent to setting SO_LINGER with l_onoff set to zero.
		*/
		SocketConfig* SetDontLinger(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, null, 0);

			return this;
		}

		/*
		for setsockopt(Don't Route).
		DONTROUTE(MSDN) : Sets whether outgoing data should be sent on interface the socket is bound to and not a routed on some other interface.
		                  This option is not supported on ATM sockets (results in an error).
		*/
		SocketConfig* SetDontRoute(SOCKET sock) {
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
		SocketConfig* SetOOBinLine(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_OOBINLINE, null, 0);

			return this;
		}

		/*
		for setsockopt(KeepAlive).
		KEEPALIVE(MSDN) : Enables sending keep-alive packets for a socket connection.
		Not supported on ATM sockets (results in an error).
		*/
		SocketConfig* SetKeepAlive(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, null, 0);

			return this;
		}

		/*
		for setsockopt(RecvBuf).
		RCVBUF(MSDN) : Specifies the total per-socket buffer space reserved for receives.
		*/
		SocketConfig* SetRecvBuf(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, null, 0);

			return this;
		}

		/*
		for setsockopt(SendBuf).
		SNDBUF(MSDN) : Specifies the total per-socket buffer space reserved for sends.
		*/
		SocketConfig* SetSendBuf(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, null, 0);

			return this;
		}

		/*
		for setsockopt(ResueAddr).
		REUSEADDR(MSDN) : Allows the socket to be bound to an address that is already in use.
		                  For more information, see bind. Not applicable on ATM sockets.
		*/
		SocketConfig* SetReuseAddr(SOCKET sock) {
			uint32 result = 0;

			result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, null, 0);

			return this;
		}

	public:
		SocketConfig() {}
		~SocketConfig() {}

		SocketConfig* SetSocketOption(SOCKET sock, uint32 option, void* value, uint32 value_len) {
			uint32 level_offset = 0;
			uint32 result = 0;

			level_offset = option / 0x10;
			if (level_offset >= MAX_LEVEL_COUNT) {
				return this;
			}

			result = setsockopt(sock, mLevel[level_offset], mOption[option], (const char*)value, value_len);

			return this;
		}
	};
}
#pragma once
#include "tcp.h"

namespace Tribal {
	class NetworkFactory
	{
	public:
		virtual TCPObject *GetTCPObject() = 0;
		virtual ~NetworkFactory() = 0;
	};
}
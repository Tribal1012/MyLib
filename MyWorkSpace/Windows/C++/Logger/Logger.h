#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdio>

#include <Windows.h>

namespace GlobalLogger {
	class LogStream {
	private:
		char *longlong_to_string(__out char *buf, __in unsigned long long n, __in int len, __in DWORD flag);
		char *longlong_to_hexstring(__out char *buf, __in unsigned long long u, __in int len, __in DWORD flag);

	public:
		LogStream& format(__in const char* fmt, ...);
		DWORD SetDump(__in BYTE* pData, __in DWORD dwLen);
		VOID Reset();

		BYTE* GetStream();
		DWORD GetStreamLength();

	private:
		// std::vector<char> m_stream;
		std::string m_stream;
	};

#define LONGFLAG     0x00000001
#define LONGLONGFLAG 0x00000002
#define HALFFLAG     0x00000004
#define HALFHALFFLAG 0x00000008
#define SIZETFLAG    0x00000010
#define ALTFLAG      0x00000020
#define CAPSFLAG     0x00000040
#define SHOWSIGNFLAG 0x00000080
#define SIGNEDFLAG   0x00000100
#define LEFTFORMATFLAG 0x00000200
#define LEADZEROFLAG 0x00000400

	class Logger {
	private:
		static Logger* pInstance;

	private:
		static void destroy();

		/* Normal standard output mode */
		// Logger();
		/* Have file */
		Logger(__in BYTE* file);

	public:
		static Logger* GetGlobalLogger(__in_opt BYTE* file = NULL);
		~Logger();

		DWORD Open(__in std::string& filename);
		DWORD Write(LogStream* lStream);
		DWORD Write(LogStream& lStream);
		VOID Close();

		friend Logger& operator << (Logger& log, LogStream& lstream);
		friend Logger& operator << (Logger& log, LogStream* lstream);
		
		Logger& operator *(Logger& log);

	private:
		// std::ifstream m_in;
		std::ofstream *m_out;

		bool m_bIsOpen;
	};

	class LoggerUtil {
	public:
		static void hexdump(__in BYTE * p, __in DWORD len);
		static void fhexdump(__in std::ofstream& out, __in BYTE * p, __in DWORD len);
	};
}

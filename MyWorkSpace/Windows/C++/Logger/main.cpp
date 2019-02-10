#include <Windows.h>

#include "Logger.h"

#include <tchar.h>

using GlobalLogger::Logger;
using GlobalLogger::LogStream;

#define LOGPATH "C:\\Users\\Tribal\\Desktop\\log_test.txt"

/* for testing */
int 
_tmain(
	__in int argc, 
	__in _TCHAR *argv[]) {
	Logger* pLogger = Logger::GetGlobalLogger((BYTE*)LOGPATH);
	LogStream lStream = LogStream();

	BYTE buffer[0x100] = { 0, };

	lStream.format("[+] Result : 0x%x %s %u\n", 0x10, "Hello", 100);
	lStream.format("[+] Ok.... zzZ \n");
	lStream.format("[+] Test String : %ls\n", L"Testing Tribal");
	lStream += L"WString Input!!\n";
	*pLogger << lStream;
	lStream.Reset();

	lStream.SetDump(buffer, 0x100);
	*pLogger << lStream;
	lStream.Reset();

	pLogger->Close();

	return 0;
}

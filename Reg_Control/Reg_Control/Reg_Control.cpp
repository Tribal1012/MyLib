// Reg_Control.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Windows.h"
#include "Winreg.h"
#include "Winerror.h"

void reg_del(HKEY hkey, TCHAR* path) {
	DWORD result;
	TCHAR* s = NULL;

	result = RegDeleteKeyEx(hkey, path, KEY_WOW64_64KEY, 0);
	if (result) {
		printf("RegDeleteKeyEx Error!\n");
		printf("Error Code : %x\n");

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			0,
			result,
			0,
			(TCHAR*)&s,
			0,
			0);

		_tprintf(_T("Error : %s\n"), s);

		LocalFree(s);

		exit(-1);
	}
}

int main()
{
	HKEY hkey1 = NULL;
	DWORD result;
	TCHAR* s = 0;
	

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SYSTEM\\CurrentControlSet\\Services\\", 
				REG_OPTION_OPEN_LINK, 
				KEY_ALL_ACCESS,
				&hkey1);
	if (result) {
		printf("RegOpenKeyEx Error!\n");
		printf("Error Code : %x\n");

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			0,
			result,
			0,
			(TCHAR*)&s,
			0,
			0);

		_tprintf(_T("Error : %s\n"), s);

		LocalFree(s);

		return -1;
	}

	reg_del(hkey1, "PROCMON23\\Instances\\Process Monitor 23 Instance");
	reg_del(hkey1, "PROCMON23\\Instances");
	reg_del(hkey1, "PROCMON23");

	RegCloseKey(hkey1);

	SendMessage(HWND_BROADCAST,
		WM_SETTINGCHANGE,
		0,
		0);

    return 0;
}

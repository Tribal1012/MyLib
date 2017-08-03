// Service_Control.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Windows.h"

void error_log(const DWORD errcode) {
	switch (errcode) {
	case ERROR_ACCESS_DENIED:
		puts("ERROR_ACCESS_DENIED");
		break;
	case ERROR_INVALID_HANDLE:
		puts("ERROR_INVALID_HANDLE");
		break;
	case ERROR_INVALID_NAME:
		puts("ERROR_INVALID_NAME");
		break;
	case ERROR_SERVICE_DOES_NOT_EXIST:
		puts("ERROR_SERVICE_DOES_NOT_EXIST");
		break;
	default:
		puts("I don't know...");
		break;
	}
}

int main()
{
	SERVICE_STATUS srvstat;
	DWORD errcode;

	SC_HANDLE h_scm = OpenSCManager(NULL, 
									NULL,
									SC_MANAGER_ALL_ACCESS);
	if (h_scm == NULL) {
		puts("failed OpenSCManager\n");
		errcode = GetLastError();
		error_log(errcode);
		return -1;
	}

	SC_HANDLE h_sm = OpenService(h_scm, 
								"PROCMON23", 
								SERVICE_ALL_ACCESS);
	if (h_sm == NULL) {
		puts("PROCMON23 didn't is found...\n");
		errcode = GetLastError();
		error_log(errcode);
		return -1;
	}

	CloseServiceHandle(h_sm);
	CloseServiceHandle(h_scm);

    return 0;
}


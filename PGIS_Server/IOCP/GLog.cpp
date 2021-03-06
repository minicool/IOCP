#include "stdafx.h"

#define	SIZE_LOG_NAME	128
char szLogDir[SIZE_LOG_NAME + 1] = ".\\GLog.ini";
char szLogClass[SIZE_LOG_NAME + 1];
char szLogName[SIZE_LOG_NAME + 1];
DWORD dwIndex = 0;
BOOL bIsFirst = TRUE;
CRITICAL_SECTION GLogCS;

void GLog_Init(void)
{
	char szDir[512];

	GetModuleFileName(0, szDir, 513);
	int i;
	i = strlen(szDir) - 1;
	while(i > 0)
	{
		if(szDir[i] == '\\')
		{
			szDir[i] = 0;
			break;
		}
		i--;
	}
	sprintf(szLogDir, "%s\\GLog.ini", szDir);

	SYSTEMTIME SysTime;

	GetSystemTime(&SysTime);
	sprintf(szLogClass, "LogClass_%d_%d_%d", 
			SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	InitializeCriticalSection(&GLogCS);
}

void GLog_Write(char* lpszLog)
{
	if(bIsFirst)
	{
		bIsFirst = FALSE;
		GLog_Init();
	}
//	EnterCriticalSection(&GLogCS);
	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);
	sprintf(szLogName, "%d_%d_%d_%d_%d_%d_%d_%d", 
				SysTime.wYear, SysTime.wMonth, SysTime.wDay,
				SysTime.wHour + 8, SysTime. wMinute, SysTime.wSecond, SysTime.wMilliseconds,
				dwIndex);
	WritePrivateProfileString(szLogClass, szLogName, lpszLog, szLogDir);
	dwIndex++;
//	LeaveCriticalSection(&GLogCS);
}



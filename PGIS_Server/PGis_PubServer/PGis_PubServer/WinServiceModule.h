/*********************************************************************
* Copyright (c) 2009, 
* All rights reserved.
* 
* 文件名称：WinServiceModule.h
* 文件标识：创建windows服务，此类不是通用的类，只允许有一个实例
* 摘    要：
*
*   作者            日期            版本            描述
* 
*********************************************************************/
#ifndef __MY_WINSERVICEMODEULE_H_INCLUDED_
#define __MY_WINSERVICEMODEULE_H_INCLUDED_

class CWinServiceModule
{
public:
    CWinServiceModule();
    ~CWinServiceModule();

    BOOL StartService();
    BOOL ParseStandardArgs(LPSTR lpCmdLine);
    BOOL SetServiceName(const char * strServiceName);

public:
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
    BOOL Start();
    VOID terminate(DWORD error);

    BOOL SendStatusToSCM (DWORD dwCurrentState,
					  DWORD dwWin32ExitCode, 
					  DWORD dwServiceSpecificExitCode,
					  DWORD dwCheckPoint,
					  DWORD dwWaitHint);

    void LogEvent(LPCTSTR pFormat, ...);
    
    void DEBUG_TRACE(int level,const char *format , ...);

private:
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);

    // static data
    static CWinServiceModule* m_pThis;

protected:
    SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
    HANDLE m_hTerminateEvent;
    
    TCHAR m_szServiceName[256];
};

#endif
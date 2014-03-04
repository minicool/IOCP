#include "StdAfx.h"

#include <time.h>
#include "WinServiceModule.h"
#include "GPSProc.h"
#include "WriteLogThread.h"

extern CGPSProc gPGisPubNetProc; 
extern CWriteLogThread gWriteLogApp;

CWinServiceModule* CWinServiceModule::m_pThis = NULL;

CWinServiceModule::CWinServiceModule()
{
    //只允许有一个实例
    m_pThis = this;
    char szServiceName[] = "HiTom_GPSPUBLICNET";
    memset(m_szServiceName,0,256*sizeof(TCHAR));
    strncpy(m_szServiceName, szServiceName, sizeof(szServiceName)-1);
}

CWinServiceModule::~CWinServiceModule()
{

}

//static member function (callback)
VOID CWinServiceModule::Handler(DWORD controlCode) 
{
	DWORD currentState = 0;
	BOOL success;
    CWinServiceModule* pService = m_pThis;

	switch(controlCode)
	{
	// There is no START option because
	// ServiceMain gets called on a start
	// Stop the service
	case SERVICE_CONTROL_STOP:
		// Tell the SCM what's happening
		success = m_pThis->SendStatusToSCM(SERVICE_STOP_PENDING,NO_ERROR, 0, 1, 5000);
		// Set the event that is holding ServiceMain
		// so that ServiceMain can return
		SetEvent(m_pThis->m_hTerminateEvent);
		return;
	// Pause the service
	case SERVICE_CONTROL_PAUSE:
		//不处理
		break;
	// Resume from a pause
	case SERVICE_CONTROL_CONTINUE:
		//不处理
		break;
	// Update current status
	case SERVICE_CONTROL_INTERROGATE:
		// it will fall to bottom and send status
		break;
	// Do nothing in a shutdown. Could do cleanup
	// here but it must be very quick.
	case SERVICE_CONTROL_SHUTDOWN:
		return;	
	default:
		break;
	}
	//m_pThis->SendStatusToSCM(currentState, NO_ERROR, 0, 0, 0);
}

//static member function (callback)
void CWinServiceModule::ServiceMain(DWORD argc, LPTSTR *argv) 
{
	BOOL success;
    CWinServiceModule* pService = m_pThis;
	// immediately call Registration function
	pService->m_ServiceStatusHandle =
		RegisterServiceCtrlHandler(
		pService->m_szServiceName, (LPHANDLER_FUNCTION)Handler);

	if (!pService->m_ServiceStatusHandle) 
    {
        pService->terminate(GetLastError()); 
        return;
    }
	
	// Notify SCM of progress
	success = pService->SendStatusToSCM(SERVICE_START_PENDING,NO_ERROR, 0, 1, 5000);
	if (!success)
    {
        pService->terminate(GetLastError());
        return;
    }
	
	// create the termination event
	pService->m_hTerminateEvent = CreateEvent (0, TRUE, FALSE, 0);
	if (!pService->m_hTerminateEvent) 
    {
        pService->terminate(GetLastError()); 
        return;
    }
	
	// Notify SCM of progress
	success = pService->SendStatusToSCM(SERVICE_START_PENDING,NO_ERROR, 0, 2, 1000);
	if (!success) 
    {
        pService->terminate(GetLastError());
        return;
    }

	// Notify SCM of progress
	success = pService->SendStatusToSCM(SERVICE_START_PENDING,NO_ERROR, 0, 3, 5000);
	if (!success) 
    {
        pService->terminate(GetLastError()); 
        return;
    }
	
	//Start the service itself
    if(gPGisPubNetProc.StartPubNetSystem())
    {
        gWriteLogApp.WriteLog(FOR_ALARM, "%s [SYS] HiTown GPS公网接入服务程序启动成功!\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"));
    }
    else
    {
        gWriteLogApp.WriteLog(FOR_ALARM, "%s [SYS] HiTown GPS公网接入服务程序启动失败!\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"));

        pService->terminate(0);

        return ;
    }
    
	// The service is now running. 
	// Notify SCM of progress
	success = pService->SendStatusToSCM(SERVICE_RUNNING,NO_ERROR, 0, 0, 0);
	if (!success) 
    {
        pService->terminate(GetLastError()); 
        return;
    }
	
	// Wait for stop signal, and then terminate
	WaitForSingleObject (pService->m_hTerminateEvent, INFINITE);


    //退出的后续处理
    gPGisPubNetProc.StopPubNetSystem();

    pService->terminate(0);
    
     
}


//设置服务名
BOOL CWinServiceModule::SetServiceName(const char * strServiceName)
{
    if(strServiceName != NULL)
    {
        memset(m_szServiceName,0,256*sizeof(TCHAR));
        strcpy(m_szServiceName, strServiceName);
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CWinServiceModule::ParseStandardArgs(LPSTR lpCmdLine)
{
    if(lpCmdLine == NULL)
    {

        return FALSE;
    }

    if (stricmp(lpCmdLine, "/install") == 0)
	{
		if(Install())
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s Install公网接入服务成功 \n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        }
        else
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s Install公网接入服务失败，服务已安装.\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        }
        return TRUE;
	}
	else if (stricmp(lpCmdLine, "/uninstall") == 0)
	{
		Uninstall();

        return TRUE;
	}
    else if(stricmp(lpCmdLine,"/start") == 0)
    {

        Start();
        
        return TRUE;
    }


    return FALSE;
}

//记录服务事件
void CWinServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR  chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;
	
	hEventSource = RegisterEventSource(NULL, m_szServiceName);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}

BOOL CWinServiceModule::StartService()
{
    SERVICE_TABLE_ENTRY st[] = 
    {
        {m_szServiceName,ServiceMain},
        {NULL, NULL}
    };

    BOOL res = ::StartServiceCtrlDispatcher(st);
    if (!res)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s Register Service Main Function Error \n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        
        LogEvent(_T("Register Service Main Function Error!"));
    }
    else
    {
        //DEBUG_TRACE(1,"StartServiceCtrlDispatcher ok \n");
    }
    return res;

}

// This function consolidates the activities of 
// updating the service status with
// SetServiceStatus
BOOL CWinServiceModule::SendStatusToSCM (DWORD dwCurrentState,
					  DWORD dwWin32ExitCode, 
					  DWORD dwServiceSpecificExitCode,
					  DWORD dwCheckPoint,
					  DWORD dwWaitHint)
{
    //SERVICE_STOPPED
	BOOL success;
	SERVICE_STATUS serviceStatus;
	// Fill in all of the SERVICE_STATUS fields
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;
	// If in the process of doing something, then accept
	// no control events, else accept anything
	if (dwCurrentState == SERVICE_START_PENDING)
    {
		serviceStatus.dwControlsAccepted = 0;
    }
	else
    {
		serviceStatus.dwControlsAccepted = 
		SERVICE_ACCEPT_STOP |
		SERVICE_ACCEPT_PAUSE_CONTINUE |
		SERVICE_ACCEPT_SHUTDOWN;
    }
	
	// if a specific exit code is defined, set up
	// the win32 exit code properly
	if (dwServiceSpecificExitCode == 0)
		serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
	else
		serviceStatus.dwWin32ExitCode = 
		ERROR_SERVICE_SPECIFIC_ERROR;
	serviceStatus.dwServiceSpecificExitCode =
		dwServiceSpecificExitCode;
	
	serviceStatus.dwCheckPoint = dwCheckPoint;
	serviceStatus.dwWaitHint = dwWaitHint;
	
	// Pass the status record to the SCM
	success = SetServiceStatus (m_ServiceStatusHandle, 
		&serviceStatus);
	return success;
}

VOID CWinServiceModule::terminate(DWORD error)
{

    //退出服务
	// if terminateEvent has been created, close it.
	if (m_hTerminateEvent) 
    {
        CloseHandle(m_hTerminateEvent);
    }
	// Send a message to the scm to tell about stopage
	if (m_ServiceStatusHandle)
    {
        SendStatusToSCM(SERVICE_STOPPED, error,0, 0, 0);
    }
    
    //gWriteLogApp.WriteLog(FOR_ALARM, "%s 退出HZ_GPS公网接入服务程序!\n",
    //    CTime::GetCurrentTime().Format("%H:%M:%S"));
	
}

//判断服务是否已经被安装
BOOL CWinServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM != NULL)
    {
		//打开服务
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//安装服务
BOOL CWinServiceModule::Install()
{
    if (IsInstalled())
    {
        return TRUE;
    }

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[MAX_PATH] ={0};
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
    
    //DEBUG_TRACE(1,"szFilePath=%s \n",szFilePath);
	//创建服务
    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, m_szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);


    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), m_szServiceName, MB_OK);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    return TRUE;
}

//启动服务
BOOL CWinServiceModule::Start()
{
    BOOL bResult = FALSE;

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
		//打开服务
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_ALL_ACCESS);
        if (hService != NULL)
        {
            bResult = ::StartService(hService,NULL,NULL);
            if(!bResult)
            {
                LogEvent("StartService error no.: %i\n",GetLastError());
            }
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//移除服务
BOOL CWinServiceModule::Uninstall()
{
    if (!IsInstalled())
    {
        return TRUE;
    }

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), m_szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
    {
        return TRUE;
    }

    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}


void CWinServiceModule::DEBUG_TRACE(int level,const char *format , ...)
{
    // return;
    char m_strInfo[512]={0};
    if(format != NULL)
    {
        va_list args;
        va_start(args, format);
        vsprintf(m_strInfo,format,args);
        va_end(args);
    }
    
    TCHAR szFilePath[MAX_PATH] ={0};
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);

    (strrchr(szFilePath,'\\'))[1] = 0; 
    char *fn = "test.txt";
    strcat(szFilePath,fn);
    
    char m_logInfo[512]={0};
    time_t now;
    struct tm *tmnow;
    
    char strDay[24] = {0} ;
    time(&now);
    tmnow = localtime(&now);
    strftime(strDay, 24, "%Y-%m-%d %H:%M:%S ", tmnow);
    
    strcat(m_logInfo,strDay);
    strcat(m_logInfo,m_strInfo);

    FILE *fhandle;
    //::GetModuleFileName(NULL, FilePath, MAX_PATH);
    fhandle = fopen(szFilePath, "a+");
    if(fhandle == NULL)
    {
        return ;
    }
    
    if(fseek(fhandle, 0, SEEK_END) != 0)
    {
        fclose(fhandle);
        return ;
    }
    
    fwrite((LPCTSTR)m_logInfo, 1, strlen(m_logInfo), fhandle);
    fclose(fhandle);
 
    return;
}

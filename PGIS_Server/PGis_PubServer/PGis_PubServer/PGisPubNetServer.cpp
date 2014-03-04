// hytPubNetServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PGisPubNetServer.h"
#include <conio.h>
#include "rwini.h"
#include "WinServiceModule.h"
#include "GPSProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
int gRecvGpsCount = 0 ;
int gSendGpsCount = 0;
BOOL g_bQuit = FALSE;
CGPSProc gPGisPubNetProc;

using namespace std;

extern CWriteLogThread gWriteLogApp;

void PrintfHeadInfo()
{
printf( 
"====================Hitown PGIS公网接入服务程序==================\n\
   CMD 'C' Clean Screen!\n\
   CMD 'Ctrl+Q' when you want to exit!\n\
   CMD 'S' GPSData Statistical!\n\
   CMD 'Ctrl+S' Clean Statistic!\n\
   CMD 'T' Ter Info\n\
==================================================================\n");
}

void CheckKey( void *dummy )
{
    int ch;
    PrintfHeadInfo();
    do
    {
        ch = _getch();
        ch = toupper( ch );
        switch( ch )
        {
        case 'C':
            system("cls");
            PrintfHeadInfo();
            break;
        case 'S':
            //printf("收GPS数据总数:%d,向主控发送GPS数据:%d\n",gRecvGpsCount,gSendGpsCount);
            break;
        case 'S' - 64: 
            gRecvGpsCount = 0;
            printf("GPS数据统计清零\n");
            break;
		case 'T': 
			gPGisPubNetProc.m_BaseBusinessManage.ShowLoginTerNum();
			break;
        default:
            break;
        }
    }
    while( ch != ('Q'- 64) );
    
    g_bQuit = TRUE;
    _endthread();
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;
    
    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: change error code to suit your needs
        cerr << _T("Fatal Error: MFC initialization failed") << endl;
        nRetCode = 1;
    }
    else
    {
        // TODO: code your application's behavior here.
        CString strHello;
        strHello.LoadString(IDS_HELLO);
    }
#ifdef __SERVICE_MODE_
    
    CWinServiceModule MyWinService; 
    CString tempstr =_T("pGisPubNetCfg.ini"); 
    Crwini rwIniTemp;
    if(!rwIniTemp.CheckIniFile(tempstr))
    {
        TRACE("%s [SYS]系统启动失败_1,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        Sleep(100);
        return FALSE;
    }
    char m_strTempName[512] ={0};
    if(rwIniTemp.ReadString(_T("SystemSet"),_T("ServerName"),tempstr,m_strTempName) == 0) 
    {
        sprintf(m_strTempName,"PGISPubServer");
    }
    
    MyWinService.SetServiceName(m_strTempName);
    if(!MyWinService.ParseStandardArgs(argv[1]))
    {
        if(!MyWinService.IsInstalled())
        {
            //MyWinService.Install();
            return nRetCode;
        }
        MyWinService.StartService();
    }
#else
    
    _beginthread(CheckKey,0,NULL);
    Sleep(50);
    SetTimer(NULL,0,1000,0L);
    
    gPGisPubNetProc.StartPubNetSystem();
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) 
    { 
        switch(msg.message)
        {              
            //定时器处理
        case WM_TIMER:
            if(g_bQuit)
            {
                gPGisPubNetProc.StopPubNetSystem();
                Sleep(100);
                PostMessage(NULL,WM_QUIT,0,0);
            }
            break; 
        default:
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            break;
        }
    }
#endif
    return nRetCode;
    
}





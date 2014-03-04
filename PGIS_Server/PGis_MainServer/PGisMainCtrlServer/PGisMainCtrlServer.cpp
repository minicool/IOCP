// PGisMainCtrlServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PGisMainCtrlServer.h"
#include "rwini.h"
#include <conio.h>
#include "WinServiceModule.h"
#include "MemoryPool.h"
#include "Encrypt.h"

#include "HttpInterface/HttpInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//
#define DllExport   __declspec( dllexport )

#pragma comment(lib, "cpsx0.lib")

// 根据CPSX的l/m/s, 计算队: 起始个呼/个呼数/起始组呼/组呼数
extern "C" DllExport int CountCpsxSchemeArry( 
                                             WORD pCpsx[60][4], 
                                             UINT l, UINT m, UINT s); 


// 计算指定CPSX队中最大组呼号
extern "C" DllExport UINT GetCpsxMaxGroupIdent1(
                                                UINT iTeam, 
                                                UINT l, UINT m, UINT s);
// ----------------------------------------------------------
extern "C" DllExport int GetCpsxMaxGroupIdent(
                                              UINT *maxID, UINT iTeam, 
                                              UINT l, UINT m, UINT s);
//extern "C" DllExport UINT GetCpsxMaxGroupIdent(...);


// 计算指定CPSX队中最大个呼号
extern "C" DllExport UINT GetCpsxMaxIndvIdent(
                                              UINT iTeam, 
                                              UINT l, UINT m, UINT s);


// 计算内码 ID 对应的 队号/用户号 外码显示(如"20201")
extern "C" DllExport int GetCpsxFltIdText(char cIdent[], 
                                          WORD iIdent, 
                                          UINT l, UINT m, UINT s); 

// 根据外码显示字符串(如"20201"), 计算CPSX的EEPROM数据
extern "C" DllExport WORD GetCpsxIdValueOfText1( 
                                                char *strUserID, 
                                                UINT l, UINT m, UINT s); 
// -------------------------------------------------
extern "C" DllExport int GetCpsxIdValueOfText( 
                                              WORD *pUserID, 
                                              char *strUserID, 
                                              UINT l, UINT m, UINT s); 
///////////////////////////////////////////////////////////////////////////////////////////////
// The one and only application object
//CWinApp theApp;
#define WEB_CMDLIST_MAXCOUNT 256

BOOL g_bQuit = FALSE;

CWriteLogThread gWriteLogApp;
CPgisMainSer gPGisMainCtrlProc;

using namespace std;

::CCriticalSection g_csRecvClientList;
CList<RECV_CLIENTDATA, RECV_CLIENTDATA&> g_RecvClientList;

::CCriticalSection g_csRecvServerList;
CList<RECV_SERVERDATA, RECV_SERVERDATA&> g_RecvServerList;

UINT g_dwWorkThreadID = 0;

UINT g_dwWebThreadID = 0;
CMemoryPool gWebServiceList;

//记录当前连接的GIS客户端数量
int g_nRecvConntGisClientCount = 0;

///////////////////////////////////山海经纬/////////////////////////////////////////////////
//
// typedef enum enumTimerType
// {
// 	SH_TCP_CONN_FAIL,
// 	SH_TCP_CONN_SUCCESS,
// 	SH_TCP_WAIT_LOGIN_ACK,
// 	SH_TCP_LOGIN_SUCCESS,
// 	SH_TCP_WAIT_LOGIN_MSG, //tcp连接成功后等待收GPRS连接登录消息定时
// 	SH_WAIT_CONN_TICK,
// }SH_TCPCONN_TYPE;
// 
// typedef enum enumShanHaiConnState
// {
// 	SH_TYE_LOGIN,
// 	SH_TYPE_TICK,
// 	SH_TYPE_GPSDATA,
// 
// }SH_CONNECT_STATE;
// 
// #define MIN_SHANHAI_PACKAGE_LEN     2
// #define MAX_SHANHAI_PACKAGE_LEN     128
// 
// #define SH_MAX_GPS_MESSAGE_LEN 256

///////////////////////////////////////////////////////////////////////////////////////////////
//
void PrintfHeadInfo()
{
  printf( "=========================HITOWN PGIS主控程序=========================\n\
  CMD 'C' 清屏!\n\
  CMD 'L' 重新读取并且设置日志级别!\n\
  CMD 'S' 查看统计数据!\n\
  CMD 'Ctrl+S' 清除统计数据!\n\
  CMD 'D' 查看在线终端信息!\n\
  CMD 'Ctrl+D' 查看在线终端数量!\n\
  CMD 'G' 查看GIS客户端连接!\n\
  CMD 'Ctrl+G' 查看GIS客户端数量!\n\
  CMD 'Ctrl+Q' 退出系统!\n\
==================================================================\n");
}

int gWriteGpsConnt = 0;
int gRecvGpsCount = 0;
int g_nRecvLocationGPSCount = 0;
int g_nSendLocationGPSCount = 0;
int g_nPatrAlertCount = 0;//收巡防告警数据统计

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
        case 'L':
            gPGisMainCtrlProc.SetSysLogLevel();
			break;
		case 'S':
			printf("GP统计,收:%d条数据,写库成功:%d条数据,向地理位置发送:%d,接收位置:%d,巡防告警:%d \n",gRecvGpsCount,gWriteGpsConnt,g_nSendLocationGPSCount,g_nRecvLocationGPSCount,g_nPatrAlertCount);
			break;
		case 'S' - 64:
			gRecvGpsCount = 0;
			gWriteGpsConnt = 0;
			g_nRecvLocationGPSCount = 0;
            g_nSendLocationGPSCount = 0;
			g_nPatrAlertCount = 0;
			printf("清除统计数据成功\n");
			break;
		case 'D':
			gPGisMainCtrlProc.m_BusinessManage_ConToPub.ShowOnLineDeviceInfo();
			break;
		case 'D' - 64:
			gPGisMainCtrlProc.m_BusinessManage_ConToPub.ShowOnLineDeviceNum();
			break;
		case 'G':
			gPGisMainCtrlProc.m_BusinessManage_GISTerminal.ShowLoginGisClientInfo();
			break;
		case 'G' -64:
			gPGisMainCtrlProc.m_BusinessManage_GISTerminal.ShowLoginGisNum();
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

	CString tempstr =_T("pGisMainCtrlCfg.ini");

    Crwini rwIniTemp;
    if(!rwIniTemp.CheckIniFile(tempstr))
    {
		//MessageBox(NULL, _T("系统启动失败_1,找不到配置文件"), "提示", MB_OK);
        //gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败_1,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        //Sleep(100);
		return FALSE;
    }

	char m_strTempName[512] ={0};
    if(rwIniTemp.ReadString(_T("SystemSet"),_T("ServerName"),tempstr,m_strTempName) == 0) 
    {
        sprintf(m_strTempName,"HT_GisMainCtrlServer");
        
    }
    MyWinService.SetServiceName(m_strTempName);

    if(!MyWinService.ParseStandardArgs(argv[1]))
    {
		//如果服务没有安装 则安装之
		if(!MyWinService.IsInstalled())
		{
			MyWinService.Install();

			return nRetCode;
		}

        MyWinService.StartService();
    }
#else
	//WideCharToMultiByte();
	//MultiByteToWideChar()
    _beginthread(CheckKey,0,NULL);
    Sleep(50);
    SetTimer(NULL,0,1000,0L);
    
    gPGisMainCtrlProc.Start();
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) 
    { 
        switch(msg.message)
        {              
            //定时器处理
        case WM_TIMER:
            if(g_bQuit)
            {
                gPGisMainCtrlProc.Stop();
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

///////////////////////////////////////////////////////////////////////////////////////////////
//检测IP地址格式是否正确
BOOL CheckIPAddressFormat(const char *strIPAddress)
{
    int IP[4];
    int i = 0;
    for(i = 0 ; i < 4; i++)
    {
        IP[i] = -1;
    }

    int iStrLen;
    iStrLen = strlen(strIPAddress);

    if(iStrLen > 16 || strIPAddress == NULL)
    {
        return FALSE;
    }

    _stscanf(strIPAddress, _T("%d.%d.%d.%d"),IP,IP+1,IP+2,IP+3); //sscanf
    for(i = 0 ; i < 4; i++)
    {
        if(IP[i] == -1 || IP[i] > 255)
        {
            return FALSE;
        }
    }
    //TCHAR szIpBuf[32] = {0};
    CString szIpBuf;
    szIpBuf.Format(_T("%d.%d.%d.%d"),IP[0],IP[1],IP[2],IP[3]); //sprintf

    int ilen = szIpBuf.GetLength();
    if(ilen != iStrLen)
    {
        return FALSE;
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
CPgisMainSer::CPgisMainSer()
{
    m_bSysStartFlag = FALSE;
    m_nMaxGisConnCount = -1;

    g_strConfigFile = _T("pGisMainCtrlCfg.ini");
}

CPgisMainSer::~CPgisMainSer()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////
//转换成内码
BOOL CPgisMainSer::GetCpsxIdValue(const char *strValue)
{
    if(strlen(strValue) != 8)
    {
        return FALSE;
    }

	char tempBuf[16] = {0};
    
    int nHeadValue;
    
    sscanf(strValue,"%3d%s",&nHeadValue,tempBuf);

    nHeadValue = nHeadValue - 200;
    if(nHeadValue < 0 || nHeadValue > 127 )
    {
        return FALSE;
    }
    
    WORD wId = 0; 
    int nTemp = GetCpsxIdValueOfText(&wId, tempBuf, m_nL, m_nM, m_nS); 

    if(!nTemp)
    {
        return FALSE;
    }

    return TRUE;
}

//加密验证
BOOL CPgisMainSer::SysEncryptCheck()
{
    Crwini rwIniTemp;

    CString tempstr =_T("pGisMainCtrlCfg.ini");

    BOOL bRes = TRUE;
    CString strTempEncrypt1,strTempEncrypt2;
    if(rwIniTemp.ReadString(_T("SystemSet"),_T("EncryptText"),tempstr,strTempEncrypt1.GetBuffer(256)) == 0) 
    {
        bRes = FALSE ;
    }
    else
    {
        strTempEncrypt1.ReleaseBuffer();
    }

    CEncrypt tempEncryptClass;

    bRes = tempEncryptClass.CompareEncryptText(strTempEncrypt1,m_nMaxGisConnCount);
    if(!bRes)
    {
        CString strPswdTxt;
        strPswdTxt = tempEncryptClass.OutputPWSDText();

        TCHAR szFilePath[MAX_PATH] ={0};
        ::GetModuleFileName(NULL, szFilePath, MAX_PATH);

        (strrchr(szFilePath,'\\'))[1] = 0; 
        char *fn = "HYTPswd.txt";
        strcat(szFilePath,fn);

        char m_pswdInfo[512]={0};

        strcat(m_pswdInfo,strPswdTxt);

        FILE *fhandle;
        fhandle = fopen(szFilePath, "w+");
        if(fhandle == NULL)
        {
            return bRes;
        }

        if(fseek(fhandle, 0, SEEK_END) != 0)
        {
            fclose(fhandle);
            return bRes;
        }

        fwrite((LPCTSTR)m_pswdInfo, 1, strlen(m_pswdInfo), fhandle);
        fclose(fhandle);

    }
    return bRes;
}

//转换成宽字符
BOOL CPgisMainSer::GetUnCodeTextInfo(CString strPatroAlertMessage ,char *pPatroAlertInfo)
{	
    unsigned char TempAlertInfo[MAX_LEN_TEXT_MSG] ={0};
    int size;

    int nTempLen = strPatroAlertMessage.GetLength();
    size=::MultiByteToWideChar(CP_ACP,0,strPatroAlertMessage,nTempLen,NULL,0);
    if(size==0)
    {
        return FALSE;
    }
    wchar_t *widebuff=new wchar_t[size];
    ::MultiByteToWideChar(CP_ACP,0,strPatroAlertMessage,nTempLen,widebuff,size);

    int ucodeLen = sizeof(wchar_t)*size;
    memcpy(TempAlertInfo,widebuff,ucodeLen);

    CString strTemp;
    CString strAlertInf = _T("");
    int i;
    for(i = 0 ; i < ucodeLen; i++)
    {
        strTemp.Format("%.2X",TempAlertInfo[i]);
        strAlertInf = strAlertInf + strTemp;
    }

    nTempLen = strAlertInf.GetLength();
    if(nTempLen >= MAX_LEN_TEXT_MSG)
    {
        nTempLen = MAX_LEN_TEXT_MSG - 2;
    }

    memcpy(pPatroAlertInfo,(char *)(LPCTSTR)strAlertInf,nTempLen);

    if(widebuff)
    {
        delete []widebuff;
    }
    return TRUE;

}

//版本信息
void CPgisMainSer::InitMyVersion()
{
    char constVersion[128] = {0};

    _snprintf(constVersion, 128-1, "Hitom接入GPS主控软件 Version:%s, Build Time:%s %s", 
        "1.2.0.6", __DATE__, __TIME__);

    gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] %s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),constVersion);
}

//设置日志水平
void CPgisMainSer::SetSysLogLevel()
{
    int nTempLogLevel;
    nTempLogLevel = rwIniTemp.GetInt(_T("SystemSet"),_T("LogLevel"),g_strConfigFile);
    if(m_nLogLevel != nTempLogLevel)
    {
        gWriteLogApp.SetLogLevel(nTempLogLevel);

        m_nLogLevel = nTempLogLevel;
    }
}

//读取系统配置数据
BOOL CPgisMainSer::LoadSystemConfig()
{
    if(!rwIniTemp.CheckIniFile(g_strConfigFile))
    {
        gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 系统启动失败,找不到配置文件\n"),CTime::GetCurrentTime().Format("%H:%M:%S"));
        return FALSE;
    }

    //日志级别
    m_nLogLevel = rwIniTemp.GetInt(_T("SystemSet"),_T("LogLevel"),g_strConfigFile);

	//ShanHai连接数量
	m_nShanhaiCount = rwIniTemp.GetInt(_T("SystemSet"),_T("ShanhaiCount"),g_strConfigFile);

	//读取内外码转换参数
    m_nL = rwIniTemp.GetIntX(_T("SetCpsx"),_T("L"),g_strConfigFile);
	if(m_nL < 0)
	{
		m_nL = 10;
	}
    m_nM = rwIniTemp.GetIntX(_T("SetCpsx"),_T("M"),g_strConfigFile);
	if(m_nM < 0)
	{
		m_nM = 10;
	}
    m_nS = rwIniTemp.GetIntX(_T("SetCpsx"),_T("S"),g_strConfigFile);
	if(m_nS < 0)
	{
		m_nS = 10;
	}

	CString strAlertInfo;
	if(rwIniTemp.ReadString(_T("SystemSet"),_T("PatroAlertTextInfo"),g_strConfigFile,strAlertInfo.GetBuffer(256)) == 0) 
    {
        strAlertInfo = _T("您已经超出了巡防区域");
    }
	else
	{
		strAlertInfo.ReleaseBuffer();
	}
	memset(m_PatroAlertTextInfo,0,MAX_LEN_TEXT_MSG);
	GetUnCodeTextInfo(strAlertInfo,m_PatroAlertTextInfo);

    return TRUE;
}

//系统启动
BOOL CPgisMainSer::Start()
{
    if(m_bSysStartFlag)
    {
        return FALSE;
    }
    //版本信息
    InitMyVersion();

    //Load配置文件
    if(!LoadSystemConfig())
    {
        return FALSE;
    }

#ifdef __SYS_ENCRYPT_CHECK_
    if(!SysEncryptCheck())
    {
        gWriteLogApp.WriteLog( FOR_ALARM ,_T("%s [SYS] 系统启动失败,非法的加密串!\n"),
            CTime::GetCurrentTime().Format("%H:%M:%S") );

        //MessageBox(NULL,"主控服务启动失败,非法的加密串!","错误",MB_ICONERROR);
        Sleep(10);
        return FALSE;
    }
    else
    {
        gWriteLogApp.WriteLog( FOR_ALARM ,_T("%s [SYS] 加密验证成功!\n"),
            CTime::GetCurrentTime().Format("%H:%M:%S") );
    }
#endif

    //设置日志级别
    gWriteLogApp.SetLogLevel(m_nLogLevel);

    gWriteGpsConnt = 0;
    gRecvGpsCount = 0;

    g_nRecvLocationGPSCount = 0;
    g_nSendLocationGPSCount = 0;

    g_nPatrAlertCount = 0;

	if (!m_DBManage_WebServer.StartThread())
	{
		return FALSE;
	}

	if (!m_iocpsocket.Start())
	{
		return FALSE;
	}

	if (m_BusinessManage_GISTerminal.Start())
	{
		g_dwGISTerminalContext = m_BusinessManage_GISTerminal.GetContext();
	}

	if (m_BusinessManage_ConToPub.Start())
	{
		g_dwConToPubContext = m_BusinessManage_ConToPub.GetContext();
	}

	if (m_BusinessManage_ConnectToLocationServer.Start())
	{
		g_dwLocationServerContext = m_BusinessManage_ConnectToLocationServer.GetContext();
	}	

	if (m_BusinessManage_ConnectToGPSCheckServer.Start())
	{
		g_dwGPSCheckServerContext = m_BusinessManage_ConnectToGPSCheckServer.GetContext();
	}

	for (int nCount = 0;nCount < m_nShanhaiCount;++nCount)
	{
		CBusinessManage_ConnectToShanhaiServer* m_PBusinessManage_ConnectToShanhaiServer = new CBusinessManage_ConnectToShanhaiServer;
		DWORD g_dwShanhaiServerContext;
		if (m_PBusinessManage_ConnectToShanhaiServer->Start(nCount))
		{
			g_dwShanhaiServerContext = m_PBusinessManage_ConnectToShanhaiServer->GetContext();
			m_BusinessManage_ConnectToShanhaiServer_vec.push_back(m_PBusinessManage_ConnectToShanhaiServer);
		}
	}

    m_bSysStartFlag = TRUE;

    return TRUE;
}

//系统停止
void CPgisMainSer::Stop()
{
    if(m_bSysStartFlag)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 退出GIS主控,共收GPS数据:%d条,写库ok:%d条,向地理位置发送:%d,收位置:%d,巡防告警:%d\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),
            gRecvGpsCount,
            gWriteGpsConnt,
            g_nSendLocationGPSCount,
            g_nRecvLocationGPSCount,
            g_nPatrAlertCount);

		m_iocpsocket.Stop();
		m_DBManage_WebServer.StopThread();
		
        m_bSysStartFlag = FALSE;	
    }
}

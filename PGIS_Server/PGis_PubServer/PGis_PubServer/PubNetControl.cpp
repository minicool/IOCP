//
#include "StdAfx.h"
#include "PubNetControl.h"
#include "rwini.h"
#include "TcpInterface.h"
///////////////////////////////////////////////////////////////////////////////////////////
CWriteLogThread gWriteLogApp;

extern int gRecvGpsCount; //接收GPS数据统计
extern int gSendGpsCount; //向主控发送GPS数据统计

int g_nSendTxtMsgInterval = 2; //发送文本小小时间间隔，初始化时从配置文件中读取

UINT g_dwDataAnalyseThreadID = 0;
//临界区
CCriticalSection g_csPubNetRecvList;
CList<RECV_MESSAGE, RECV_MESSAGE&> g_PubNetRecvList;

extern const char *g_pcFtpDownFileName; //需要从FTP下载下来文件

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
//公网上行协议描述定义
struct PublicUpProtocalInfo
{
    int code;
    char *description;
};

static PublicUpProtocalInfo UpProtocalDescriptions[] =
{
    {GPS_TINMEROUT_REPORT,"定时上传GPS信息"}, //0x00
    {GPS_MONITOR_RESPONSE,"即时跟踪GPS信息"},
    {GPS_BOUNDCALL_RESPONSE,"区域点名GPS信息"}, //0x02
    {SEND_MESSAGE_ANSWER,"文字信息回复信息"}, //0x03
    {0x04,NULL},{0x05,NULL},{0x06,NULL},{0x07,NULL},
    {0x08,NULL},{0x09,NULL},{0x0a,NULL},{0x0b,NULL},
    {0x0c,NULL},{0x0d,NULL},{0x0e,NULL},{0x0f,NULL},
    
    {TERMINAL_LOGIN_REPORT,"GPRS连接登录协议"}, //0x10
    {TERMINAL_OPEN_REPORT,"开机信息协议"},
    {GPS_TERMINAL_OPEN_REPORT,"开机GPS信息协议"},
    {TCPCONNECT_TICK_REPORT,"握手信息信息协议"},
    {PRIVATENET_LINEON_REPORT,"专网上线协议"},
    {PRIVATENET_LINEOFF_REPORT,"专网下线协议"},
    {QUERY_MY_POSITION,"上传请求查询我的位置信息协议"},
    {QUERY_DESTINATION_POSITION,"上传请求查询目的地经纬度协议"}, //0x17
    {CTL_TERMINAL_MOUNTGUARD_RESPONSE,"终端上班下班"}, //0x18
    
    {GPS_HISTORY_REPORT,"历史GPS数据"},
    {0x1A,NULL},{0x1B,NULL},
    {0x1C,NULL},{0x1D,NULL},{0x1E,NULL},{0x1F,NULL},
    
    {CTL_PRIVATENET_LINE_RESPONSE,"应答控制专网上线和专网下线命令"}, //0x20
    {CTL_TERMINAL_WAKEUP_RESPONSE,"应答控制终端集群遥毙和摇醒"}, //0x21
    
    {0x22,NULL},{0x23,NULL},
    {0x24,NULL},{0x25,NULL},{0x26,NULL},{0x27,NULL},
    
    {0x28,NULL},{0x29,NULL},{0x2A,NULL},{0x2B,NULL},
    {0x2C,NULL},{0x2D,NULL},{0x2E,NULL},{0x2F,NULL},
    
    {GPS_ALERTING_REPORT,"报警协议"}, //0x30
    {0x31,NULL},{0x32,NULL},{0x33,NULL},
    {0x34,NULL},{0x35,NULL},{0x36,NULL},{0x37,NULL},
    {0x38,NULL},{0x39,NULL},{0x3A,NULL},{0x3B,NULL},
    {0x3C,NULL},{0x3D,NULL},{0x3E,NULL},{0x3F,NULL},
    
    {0x40,NULL},{0x41,NULL},{0x42,NULL},{0x43,NULL},
    {0x44,NULL},{0x45,NULL},{0x46,NULL},{0x47,NULL},
    {0x48,NULL},{0x49,NULL},{0x4A,NULL},{0x4B,NULL},
    {0x4C,NULL},{0x4D,NULL},{0x4E,NULL},{0x4F,NULL},
    
    {0x50,NULL},{0x51,NULL},{0x52,NULL},{0x53,NULL},
    {0x54,NULL},{0x55,NULL},{0x56,NULL},{0x57,NULL},
    {0x58,NULL},{0x59,NULL},{0x5A,NULL},{0x5B,NULL},
    {0x5C,NULL},{0x5D,NULL},{0x5E,NULL},{0x5F,NULL},
    
    {SET_COMMUNICATION_MODEL_RESPONSE,"应答设置通讯方式参数"}, //0x60
    {SET_GPRS_PARAMETER_RESPONSE,"应答设置GPRS参数"},
    {SET_GPS_PARAMETER_RESPONSE,"应答设置GSM参数"},
    {SET_TERMINAL_NUM_RESPONSE,"应答设置终端专网号码"},
    {SET_TEMP_NUM_RESPONSE,"应答设置动态临时编组号码"},
    {SET_ALERT_NUM_RESPONSE,"应答设置专网倒放告警号码"},
    {SET_SAFETYCHECK_NUM_RESPONSE,"应答设置专网安检号码"},
    {SET_GPSCENTER_NUM_RESPONSE,"应答设置专网GPS中心号码"},
    {SET_BACK_MODEL_RESPONSE,"应答设置专网背景模式"}, //0x68
    {0x69,NULL},{0x6A,NULL},{0x6B,NULL},
    {0x6C,NULL},{0x6D,NULL},{0x6E,NULL},{0x6F,NULL},
    
    {0x70,NULL},{0x71,NULL},{0x72,NULL},{0x73,NULL},
    {0x74,NULL},{0x75,NULL},{0x76,NULL},{0x77,NULL},
    {0x78,NULL},{0x79,NULL},{0x7A,NULL},{0x7B,NULL},
    {0x7C,NULL},{0x7D,NULL},{0x7E,NULL},{0x7F,NULL},
    
    {QUERY_COMMUNICATION_MODEL_RESPONSE,"应答读取通讯方式参数"}, //0x80
    {QUERY_GPS_PARAMETER_RESPONSE,"应答读取GPRS参数"},
    {QUERY_GSM_PARAMETER_RESPONSE,"应答读取GSM参数"},
    {QUERY_TERMINAL_NUM_RESPONSE,"应答读取终端专网号码"},
    {QUERY_TEMP_NUM_RESPONSE,"应答读取动态临时编组号码"},
    {QUERY_ALERT_NUM_RESPONSE,"应答读取专网倒放告警号码"},
    {QUERY_SAFETYCHECK_NUM_RESPONSE,"应答读取专网安检号码"},
    {QUERY_GPSCENTER_NUM_RESPONSE,"应答读取专网GPS中心号码"},
    {QUERY_BACK_MODEL_RESPONSE,"应答读取专网背景扫描模式"},
    {QUERY_MAGNETIC_BOUND_RESPONSE,"应答读取专网场强参数"},
    {QUERY_TERMINAL_TYPE_RESPONSE,"应答读取终端机型参数"},
    {QUERY_WAKEUP_RESPONSE,"应答读取终端遥毙摇醒状态"},
    {QUERY_GPS_SEND_TINMEROUT,"应答读取终端固定上传GPS信息时间间隔"}
    
};
//
///////////////////////////////////////////////////////////////////////////////////////////////
//
//检测IP地址格式是否正确
BOOL CPubNetControl::CheckIPAddressFormat(const char *strIPAddress)
{
    int IP[4];
    int i = 0;
    for(i = 0 ; i < 4; i++)
    {
        IP[i] = -1;
    }
    sscanf(strIPAddress, "%d.%d.%d.%d",IP,IP+1,IP+2,IP+3);
    for(i = 0 ; i < 4; i++)
    {
        if(IP[i] == -1 || IP[i] > 255)
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

//取得校验值
int CPubNetControl::GetChksum(const char *strMsg)
{
    int nChk ;
    int nLen = strlen(strMsg);
    nChk = 0;
    for(int i = 0; i < nLen; i++ )
    {
        nChk = nChk ^ strMsg[i];
    }
    
    return nChk;
}

//
//转换成内码
BOOL CPubNetControl::GetCpsxIdValue(const char *strValue, char *strOut)
{
    if(strlen(strValue) != 8)
    {
        return FALSE;
    }
    
    char tempBuf[16];
    
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
    sprintf(strOut,"%.2X%.4X",nHeadValue,wId);
    
    return TRUE;
}

CString CPubNetControl::GetCpsxFltId(const char *strValue)
{
    CString strRetrun = _T(" ");
    
    char tempBuf[16];
    if(strlen(strValue) != 6)
    {
        return strRetrun;
    }
    
    int nHeadValue;
    int wId = 0;
    sscanf(strValue,"%2X%x",&nHeadValue,&wId);
    if(nHeadValue < 0 || nHeadValue > 127 )
    {
        return strRetrun;
    }
    nHeadValue = nHeadValue + 200;
    
    memset(tempBuf,0,16);
    int nTemp = GetCpsxFltIdText(tempBuf, wId, m_nL, m_nM, m_nS);
    if(!nTemp)
    {
        return strRetrun;
    }
    
    wId = atoi(tempBuf); 
    strRetrun.Format("%.3d%.5d",nHeadValue,wId);
    
    return strRetrun;
}

BOOL CPubNetControl::GetCpsxFltId(const char *strValue, char *strOut)
{
    char tempBuf[16];
    if(strlen(strValue) != 6)
    {
        return FALSE;
    }
    int nHeadValue;
    int wId = 0;
    sscanf(strValue,"%2X%x",&nHeadValue,&wId);
    if(nHeadValue < 0 || nHeadValue > 127 )
    {
        return FALSE;
    }
    nHeadValue = nHeadValue + 200;
    
    memset(tempBuf,0,16);
    int nTemp = GetCpsxFltIdText(tempBuf, wId, m_nL, m_nM, m_nS);
    if(!nTemp)
    {
        return FALSE;
    }
    
    wId = atoi(tempBuf); 
    sprintf(strOut,"%.3d%.5d",nHeadValue,wId);
    return TRUE;
    
}

///////////////////////////////////////////////////////////////////////////////////////////
//
CPubNetControl::CPubNetControl()
{
    WSADATA wsaData;
    WORD     wVersion;
    wVersion = MAKEWORD(2,2);
    
    //申请Socket资源
    int nRes  = WSAStartup(wVersion,&wsaData);//版本信息
    if(nRes != 0)
    {
        WSACleanup();
        TRACE("TcpServer no Sock dll to use!!\n");	
    }
    
    if((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))//查找WINSOCK的DLL
    {
        WSACleanup();
        TRACE("TcpServer no right Sock dll!!\n");
    }
    
    m_bThreadStart = FALSE;
    
    m_dwDataAnalyseThreadID = 0;
    m_hDataAnalyseThread = NULL;
    m_hDataAnalyseThreadEndEvt = NULL;
    
    m_dwDataDispatchThreadID = 0;
    m_hDataDispatchThread = NULL;
    m_hDataDispatchThreadEndEvt = NULL;
    
    int i;
    for(i = 0; i < MAX_PUB_PROTOCAL_DOWN; i++)
    {
        m_nAckCount[i] = 0;
    }
    
    memset(m_LocalIpAddress,0,MAX_IPADDRESS_LEN);
    
    m_SockPubNetServer.InitRoutineServer(MAX_PUBLICNET_CLIENT_CONNECT);
    
    
    m_bStartFlag = FALSE;
    
    
    InitFieldStatisticTable();
}

CPubNetControl::~CPubNetControl()
{
    m_SockPubNetServer.ReleaseRoutineServer();
    WSACleanup();
}

///////////////////////////////////////////////////////////////////////////////////////////
//版本信息
void CPubNetControl::InitMyVersion()
{
    char constVersion[128] = {0};
    
    _snprintf(constVersion, 128-1, "hyt公网GPS接入系统 Version:%s, Build Time:%s %s", 
        "1.2.0.6", __DATE__, __TIME__);
    
    gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),constVersion);
}

//初始化协议字段个数,用来判断协议格式是否正确
void CPubNetControl::InitFieldStatisticTable()
{
    for(int i = 0; i < MAX_PROTOCOL_UP; i++)
    {
        m_FieldStatisticTB[i] = 0xFF;
    }

    m_FieldStatisticTB[TERMINAL_LOGIN_REPORT] = 8;  //GPRS连接登-10
    m_FieldStatisticTB[TERMINAL_OPEN_REPORT] = 9;   //开机信息协议-11
    m_FieldStatisticTB[GPS_TERMINAL_OPEN_REPORT] = 15; //开机GPS信息协议-12
    m_FieldStatisticTB[TCPCONNECT_TICK_REPORT] = 6;  //握手信息协议-13

    m_FieldStatisticTB[PRIVATENET_LINEON_REPORT] = 16;  //专网上线协议-14
    m_FieldStatisticTB[PRIVATENET_LINEOFF_REPORT] = 16; //专网下线协议-15

    m_FieldStatisticTB[GPS_TINMEROUT_REPORT] = 0xFF;//14; //固定时间间隔主动上传GPS信息-00  //kevin 2009-09-11

    m_FieldStatisticTB[GPS_MONITOR_RESPONSE] = 14; //应答即时跟踪GPS协议-01
    m_FieldStatisticTB[GPS_BOUNDCALL_RESPONSE] = 14;  //应答区域点名GPS协议-02
    m_FieldStatisticTB[SEND_MESSAGE_ANSWER] = 0xFF; //

    m_FieldStatisticTB[QUERY_MY_POSITION] = 15;            //上传请求查询我的位置信息协议-16
    m_FieldStatisticTB[QUERY_DESTINATION_POSITION] = 5;      //上传请求查询目的地经纬度协议-17
    m_FieldStatisticTB[CTL_TERMINAL_MOUNTGUARD_RESPONSE] = 16;//应答控制终端上班下班-18
    
    m_FieldStatisticTB[CTL_PRIVATENET_LINE_RESPONSE] = 7; //应答控制专网上线和专网下线命令-20
    m_FieldStatisticTB[CTL_TERMINAL_WAKEUP_RESPONSE] = 7; //应答控制终端集群遥毙和摇醒-21
    m_FieldStatisticTB[CTL_TERMINAL_MOUNTGUARD_RESPONSE] = 16; //终端在上班、下班-22

    m_FieldStatisticTB[GPS_ALERTING_REPORT] = 15; //报警协议-30

    m_FieldStatisticTB[SET_COMMUNICATION_MODEL_RESPONSE] = 6;//应答设置通讯方式参数-60
    m_FieldStatisticTB[SET_GPRS_PARAMETER_RESPONSE] = 6;//应答设置GPRS参数-61
    m_FieldStatisticTB[SET_GPS_PARAMETER_RESPONSE] = 6; //应答设置GSM参数-62

    m_FieldStatisticTB[SET_TERMINAL_NUM_RESPONSE] = 6; //应答设置终端专网号码-63
    m_FieldStatisticTB[SET_TEMP_NUM_RESPONSE] = 7; //6 //应答设置动态临时编组号码-64 //kevin 2009-09-11
    m_FieldStatisticTB[SET_ALERT_NUM_RESPONSE] = 6;//应答设置专网倒放告警号码-65
    m_FieldStatisticTB[SET_SAFETYCHECK_NUM_RESPONSE] = 6; //应答设置专网安检号码-66
    m_FieldStatisticTB[SET_GPSCENTER_NUM_RESPONSE] = 6;//应答设置专网GPS中心号码-67
    m_FieldStatisticTB[SET_BACK_MODEL_RESPONSE] = 6	; //应答设置专网背景模式-68

    m_FieldStatisticTB[QUERY_COMMUNICATION_MODEL_RESPONSE] = 7;//应答读取通讯方式参数-80
    m_FieldStatisticTB[QUERY_GPS_PARAMETER_RESPONSE] = 11; //应答读取GPRS参数-81
    m_FieldStatisticTB[QUERY_GSM_PARAMETER_RESPONSE] = 8; //应答读取GSM参数-82

    m_FieldStatisticTB[QUERY_TERMINAL_NUM_RESPONSE] = 9; //应答读取终端专网号码-83
    m_FieldStatisticTB[QUERY_TEMP_NUM_RESPONSE] = 9; //应答读取动态临时编组号码-84
    m_FieldStatisticTB[QUERY_SAFETYCHECK_NUM_RESPONSE] = 9; //应答读取专网安检号码-86 

    m_FieldStatisticTB[QUERY_ALERT_NUM_RESPONSE] = 9;//应答读取专网倒放告警号码-85
    m_FieldStatisticTB[QUERY_GPSCENTER_NUM_RESPONSE] = 9; //应答读取专网GPS中心号码-87
    m_FieldStatisticTB[QUERY_BACK_MODEL_RESPONSE] = 9 ; //应答读取专网背景扫描模式-88

    m_FieldStatisticTB[QUERY_MAGNETIC_BOUND_RESPONSE] = 7; //应答读取专网场强参数-89
    m_FieldStatisticTB[QUERY_TERMINAL_TYPE_RESPONSE] = 10; //应答读取终端机型参数-8A
    m_FieldStatisticTB[QUERY_GPS_SEND_TINMEROUT] = 7;
    m_FieldStatisticTB[QUERY_WAKEUP_RESPONSE] = 7; //应答读取终端遥毙摇醒状态-8B

}

//读取系统配置数据
BOOL CPubNetControl::LoadSystemConfig()
{
    Crwini rwIniTemp;

    CString tempstr =_T("hytPubNetCfg.ini");

    if(!rwIniTemp.CheckIniFile(tempstr))
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));

        return FALSE;
    }

    //默认设置GPS主动上传时间间隔
    m_nInitFixGpsDt = 60;
    m_nInitFixGpsDt = rwIniTemp.GetIntValue(_T("SystemSet"),_T("DefaultFixGpsDt"),tempstr);
    if(m_nInitFixGpsDt <= 0)
    {
        m_nInitFixGpsDt = 60;
    }

    //GSP次数
    m_nMonitorGPSCount = rwIniTemp.GetIntValue(_T("SystemSet"),_T("MonitorGPSCount"),tempstr);
    if(m_nMonitorGPSCount <= 0)
    {
        m_nMonitorGPSCount = 0;
    }

    //当GPRS连接登录后GPS开启模式
    m_nOpenGpsModel = rwIniTemp.GetIntValue(_T("SystemSet"),_T("OpenGpsModel"),tempstr); 
    if(m_nOpenGpsModel == -1)
    {
        m_nOpenGpsModel = 2;
    }

    m_nPubNetServerPort = rwIniTemp.GetInt(_T("TCPSet"),_T("PubNetServerPort"),tempstr);
    if(m_nPubNetServerPort == 0)
    {
        m_nPubNetServerPort = 5002;
    }

    if(rwIniTemp.ReadString(_T("TCPSet"),_T("PubNetServerIP"),tempstr,m_LocalIpAddress) == 0) 
    {
        //return FALSE;
    }
    
    m_nLoginTimeOut = rwIniTemp.GetInt(_T("SystemSet"),_T("LoginTimeOut"),tempstr);
    m_nConnTickTimeOut = rwIniTemp.GetInt(_T("SystemSet"),_T("ConnTickTimeOut"),tempstr);

    m_bDispatchZeroGPS = FALSE;
    if(rwIniTemp.GetInt(_T("SystemSet"),_T("DispatchZeroGPS"),tempstr) > 0)
    {
        m_bDispatchZeroGPS = TRUE;
    }
    
    //读取内外码转换参数
    m_nL = rwIniTemp.GetInt(_T("SetCpsx"),_T("L"),tempstr);
    m_nM = rwIniTemp.GetInt(_T("SetCpsx"),_T("M"),tempstr);
    m_nS = rwIniTemp.GetInt(_T("SetCpsx"),_T("S"),tempstr);

    m_nSendTxtMsgWaitAckTimeOut = rwIniTemp.GetInt(_T("SystemSet"),_T("SendTxtMsgWaitAckTimeOut"),tempstr);

    gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]读配置文件成功 \n",CTime::GetCurrentTime().Format("%H:%M:%S"));

    //发生文字消息时间间隔
    g_nSendTxtMsgInterval = rwIniTemp.GetInt(_T("SystemSet"),_T("SendTxtMsgInterval"),tempstr);
    if(g_nSendTxtMsgInterval <= 0 )
    {
        g_nSendTxtMsgInterval = 1;
    }
    
    //ftpDownLoad目录
    char tempPath[MAX_PATH] ={0};
    if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPDownLoadPatch"),tempstr,tempPath) == 0) 
    {
        //没有指定，则从FTP下载下来的文件保存在当前目录的FTPConvey目录里
        sprintf(m_strFTPDownLoadFileName,"%sFTPConvey\\%s",rwIniTemp.GetCurrPath(),g_pcFtpDownFileName);
    }
    else
    {
        if(strlen(tempPath) == 0)
        {
            sprintf(m_strFTPDownLoadFileName,"%sFTPConvey\\%s",rwIniTemp.GetCurrPath(),g_pcFtpDownFileName);
        }
        else
        {
            sprintf(m_strFTPDownLoadFileName,"%s\\%s",tempPath,g_pcFtpDownFileName);
        }
    }

    //读系统参数
    return TRUE;

}

BOOL CPubNetControl::StartThread()
{
    if(m_bThreadStart)
    {
        TRACE(_T("System thread has run.\n"));
        return TRUE;
    }
    
    //系统工作线程
    if(m_hDataAnalyseThreadEndEvt == 0)
    {
        m_hDataAnalyseThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);
    }
    
    m_hDataAnalyseThread = (HANDLE)_beginthreadex( NULL,0,&DataAnalyseThreadProc,this, 0,&m_dwDataAnalyseThreadID );
    if(!m_hDataAnalyseThread)
    {
        TRACE(_T("Strat System thread fail.\n"));
        return FALSE;
    }
    
    if( m_hDataDispatchThreadEndEvt == 0 )
    {
        m_hDataDispatchThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);
    }
    
    m_hDataDispatchThread = (HANDLE)_beginthreadex( NULL,0,&DataDispatchThreadProc,this, 0,&m_dwDataDispatchThreadID);
    if(!m_hDataDispatchThread)
    {
        return FALSE;
    }
    
    g_dwDataAnalyseThreadID = m_dwDataAnalyseThreadID;

    m_bThreadStart = TRUE;
    
    return TRUE;
}

void CPubNetControl::StopThread()
{
    if( m_bThreadStart )
    {
        m_bThreadStart = FALSE;
        //终止系统工作线程
        PostThreadMessage(m_dwDataAnalyseThreadID, WM_THREAD_STOP, 0L, 0L);
        ::WaitForSingleObject(m_hDataAnalyseThreadEndEvt, INFINITE);
        m_dwDataAnalyseThreadID = 0;
        
        g_dwDataAnalyseThreadID = 0;
        
        CloseHandle(m_hDataAnalyseThreadEndEvt);
        m_hDataAnalyseThreadEndEvt = NULL;
        
        //终止线程
        PostThreadMessage(m_dwDataDispatchThreadID, WM_THREAD_STOP, 0L, 0L);
        ::WaitForSingleObject(m_hDataDispatchThreadEndEvt, INFINITE);
        m_dwDataDispatchThreadID = 0;
        
        CloseHandle(m_hDataDispatchThreadEndEvt);
        m_hDataDispatchThreadEndEvt = NULL;
        
        m_hDataAnalyseThread = NULL;
    }
}

BOOL CPubNetControl::StartPublicTcpServer()
{
    int nRes;
    if(CheckIPAddressFormat(m_LocalIpAddress))
    {
        nRes = m_SockPubNetServer.Start(m_nPubNetServerPort,m_LocalIpAddress,TcpServerPublicCallBack,(DWORD)this);
    }
    else
    {
        nRes = m_SockPubNetServer.Start(m_nPubNetServerPort,NULL, TcpServerPublicCallBack,(DWORD)this);
    }
    
    if(nRes != TCPSERVER_STARTSUCCESS)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]启动监听公网终端Socket连接失败,res:%d \n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),nRes);
        
        return FALSE;
    }
    
    gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]启动监听公网终端Socket连接成功,端口:%d \n",
        CTime::GetCurrentTime().Format("%H:%M:%S"),m_nPubNetServerPort);
    
    return TRUE;
}

//停止TCP Server
void CPubNetControl::StopPublicTcpServer()
{
    for(int i = 0; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        if(m_tbPubTerminalInfo[i].GetConnectState() != CONNECT_NULL )
        {
            m_tbPubTerminalInfo[i].SetConnectState(CONNECT_NULL);
            
            m_SockPubNetServer.CloseClient(i);
        }
    }
    
    m_SockPubNetServer.Close();
}

//设置日志
void CPubNetControl::SetLog()
{
    Crwini rwIniTemp;
    
    CString tempstr =_T("hytPubNetCfg.ini");
    
    if(rwIniTemp.CheckIniFile(tempstr))
    {
        //日志级别
        int nTempV;
        nTempV = rwIniTemp.GetInt(_T("SystemSet"),_T("LogLevel"),tempstr);
        gWriteLogApp.SetLogLevel(nTempV);
        
        nTempV = rwIniTemp.GetInt(_T("SystemSet"),_T("MonitorGPSCount"),tempstr);
        gWriteLogApp.SetLogFileCleanupDate(nTempV);
    }
}

//系统启动
BOOL CPubNetControl::StartHYTPubNetSystem()
{
    if(m_bStartFlag)
    {
        return TRUE;
    }

    InitMyVersion();
    //读取配置文件
    if(!LoadSystemConfig())
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]读取配置文件失败\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"));
        
        return FALSE;
    }
    
    SetLog();
    
    
    //启动线程
    if(!StartThread())
    {
        return FALSE;
    }
    
    //启动侦听公网终端的GPRS连接
    if(!StartPublicTcpServer())
    {
        StopThread();
        
        return FALSE;
    }
    
    //启动FTP处理模块
    if(!m_myFtpConveyFile.StartFTPConvey(m_dwDataAnalyseThreadID))
    {
         StopThread();
        return FALSE;
    }
    
    m_bStartFlag = TRUE;
   
    return TRUE;
}

//系统停止
BOOL CPubNetControl::StopHYTPubNetSystem()
{
    if(m_bStartFlag)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]公网接入服务器程序退出运行,共接收GPS数据:%d(条),向主控发送GPS数据:%d\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),gRecvGpsCount,gSendGpsCount);
        
        StopPublicTcpServer();
        
        StopThread();

        m_myFtpConveyFile.StopFTPConvey();
        
        m_bStartFlag = FALSE;
    }

    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//TCP客户端:公网数据接收
DWORD WINAPI CPubNetControl::TcpServerPublicCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance)
{
    char *ptr = NULL;
    int nSpaceLen = 0;
    char *pCurPoint = NULL;
    int nPackageLen = 0;
    
    CPubNetControl *pHytGpsPublicServer = (CPubNetControl *)dwInstance;
    TcpServerClient *pServerClient = (TcpServerClient *)dwParam;
    
    static char recvPublicData[MAX_RECVICE_BUFFER] = {0};
    static int nCountRecvLen = 0;
    
    static BOOL bListFull =  FALSE;
    switch(uMsg)
    {
    case TCPSERVER_EVENT_CLIENTJOIN:
        pHytGpsPublicServer->TcpClientConnected(SOCKET_SERVER_PUBLICNET_ID,
            pServerClient->nID,
            pServerClient->szIP,
            pServerClient->nPort);
        
        break;
    case TCPSERVER_EVENT_CLIENTEXIT:
        pHytGpsPublicServer->TcpClientDisconnect(SOCKET_SERVER_PUBLICNET_ID,
            pServerClient->nID,
            pServerClient->szIP,
            pServerClient->nPort);
        
        break;
    case TCPSERVER_EVENT_RECVDATA: 
        if(pServerClient != NULL && pHytGpsPublicServer != NULL)
        {
            pHytGpsPublicServer->PublicNetDataAnalyse(pServerClient->nID,
                pServerClient->pszRecvData,
                pServerClient->nRecvDataLen);
        }
        break;
    default:
        break;
    }
    
    return 0;
}

//TCP Server接受连接
void CPubNetControl::TcpClientConnected(int typeID,int nClientID,const char *strIP,int nPort)
{
    switch(typeID)
    {
    case SOCKET_SERVER_PUBLICNET_ID:
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]公网终端接入成功,IP地址:%s,端口:%d\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),
            nClientID,
            strIP,
            nPort);
        
        //设置状态为等待登陆
        m_tbPubTerminalInfo[nClientID].SetConnectState(CONNECT_WAIT_GPRSLOGIN,m_nLoginTimeOut);
        
        break;
    default:
        break;
    }
}

//TCP Client连接断开
void CPubNetControl::TcpClientDisconnect(int typeID,int nClientID,const char *strIP,int nPort,int nExitCode)
{
    switch(typeID)
    {
    case SOCKET_SERVER_PUBLICNET_ID:
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]公网终端TCP连接断开,IP地址:%s,端口:%d\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),
            nClientID,
            strIP,nPort);
        
        if(m_tbPubTerminalInfo[nClientID].GetConnectState() == CONNECT_GPRSLOGIN_OK)
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]终端GPRS连接登录取消,DeviceCode:%s\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"),
                nClientID,
                m_tbPubTerminalInfo[nClientID].GetDeviceNum());

            //写文件，公网终端下线
            char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
            sprintf(tempBuf,"%s,%d,0",m_tbPubTerminalInfo[nClientID].GetDeviceNum(),m_tbPubTerminalInfo[nClientID].GetDeviceKind());
            ReportDataToGisMain(A_TYPE_TERMINAL_LINE,tempBuf,strlen(tempBuf));
        }
        
        m_tbPubTerminalInfo[nClientID].SetConnectState(CONNECT_NULL);
        
        break;
    default:
        break;
    }
}

void CPubNetControl::PublicNetDataAnalyse(int nClientID,const char *recvData, int nRecvLen)
{
    if(nClientID < MAX_PUBLICNET_CLIENT_CONNECT)
    {
        m_tbPubTerminalInfo[nClientID].DataParseProcess(nClientID,recvData,nRecvLen);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//数据分析线程
UINT WINAPI CPubNetControl::DataAnalyseThreadProc(void *lpParameter)
{
    CPubNetControl *pHytPublicSys = NULL;
    pHytPublicSys = (CPubNetControl *)lpParameter;
    
    TRACE(_T("Into System thread%X\n"), pHytPublicSys->m_hDataAnalyseThread);
    ResetEvent(pHytPublicSys->m_hDataAnalyseThreadEndEvt);
    int timerid = SetTimer(NULL,0xf0, 1000, 0L); //产生1s的基准定时
    MSG msg;
    BOOL bcontinue = TRUE;
    while(bcontinue)
    {
        if(GetMessage(&msg,NULL,0,0) )
        {
            switch(msg.message)
            {
            case WM_THREAD_STOP:
                bcontinue = FALSE;
                break;
            case WM_TIMER:
                pHytPublicSys->OnCheckTerminalTickOverTimer();
                break;
            case WM_RECVICE_PUBGPS_MESSAGE:
                pHytPublicSys->OnAnalysePublicServerRecvData();
                break;
#ifndef WANGZH	//2013-04-19
            case WM_FTP_DOWNLOAD_OVER_MESSAGE: //下载文件完成通知
                pHytPublicSys->OnReadMainFtpConveyFile();
                break;
#endif
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }
    
    if(timerid != 0)
    {
        KillTimer(NULL, timerid);
    }
    
    SetEvent(pHytPublicSys->m_hDataAnalyseThreadEndEvt);
    TRACE(_T("Quit System thread\n"));
    _endthreadex(0);
    
    return 0;
}

//收终端登录消息定时器,每隔1秒检测一次
void CPubNetControl::OnCheckTerminalTickOverTimer()
{
    for(int i = 0 ; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        int nRes = m_tbPubTerminalInfo[i].CheckTimerOut();
        switch(nRes)
        {
        case 1:
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]等待收终端登录消息超时\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"),i);
            
            m_SockPubNetServer.CloseClient(i); //关闭终端的TCP连接
            break;
        case 2:
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]等待终端握手消息超时,DeviceCode:%s\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"),i,m_tbPubTerminalInfo[i].GetDeviceNum());
            
            m_SockPubNetServer.CloseClient(i); //关闭终端的TCP连接
            break;
        default:
            break;
            
        }
    }
}

//分析接收公网接入的数据包
BOOL CPubNetControl::OnAnalysePublicServerRecvData()
{
    g_csPubNetRecvList.Lock();
    if(g_PubNetRecvList.IsEmpty())
    {
        g_csPubNetRecvList.Unlock();
        return FALSE;
    }
    
    RECV_MESSAGE RecvPubMsg;
    RecvPubMsg = g_PubNetRecvList.GetHead();
    g_PubNetRecvList.RemoveHead();
    g_csPubNetRecvList.Unlock();

    char *pData[MAX_PUBLICNET_DATE_TYPE];
    char tempBuf[MAX_PUBLIC_PACKAGE_LEN] = {0};
    char dispatchBuf[MAX_PUBLIC_PACKAGE_LEN] = {0};

    //获取当前时间
    CString strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
    int i;
    for(i = 0; i < MAX_PUBLICNET_DATE_TYPE; i++)
    {
        pData[i] = NULL;
    }

    memcpy(tempBuf,RecvPubMsg.MessagePacket,RecvPubMsg.length);
    char *pBuf = tempBuf;
    i= 0 ;
    int nCount = 0;
    while((pData[i]=strtok(pBuf,",")) != NULL)
    {
        i++;
        nCount++;
        pBuf = NULL; 
    }

    int nCMD = 0;
    sscanf(pData[2], "%2x",&nCMD);

    CString strLogTrace;
    if( nCMD < MAX_PROTOCOL_UP )
    {
        if(m_FieldStatisticTB[nCMD] != nCount && m_FieldStatisticTB[nCMD] != 0xFF)
        {
            strLogTrace.Format("%s [ID:%d]错误的数据,%s\n",
                strCurrTime,
                RecvPubMsg.nClientID,
                RecvPubMsg.MessagePacket);

            gWriteLogApp.WriteLog(FOR_ALARM,strLogTrace);
            
            return FALSE;
        }
    }
    
    if(UpProtocalDescriptions[nCMD].description != NULL)
    {
        if( UpProtocalDescriptions[nCMD].code == nCMD )
        {
            strLogTrace.Format("%s [ID:%d]<上行>%s:%s\n",strCurrTime,
                RecvPubMsg.nClientID,
                UpProtocalDescriptions[nCMD].description,
                RecvPubMsg.MessagePacket);
			
			gWriteLogApp.WriteLog(FOR_TRACE,strLogTrace);
        }
        else
        {
            strLogTrace.Format("%s [ID:%d]<上行>接收数据:%s\n",strCurrTime,
                RecvPubMsg.nClientID,
                RecvPubMsg.MessagePacket);

            gWriteLogApp.WriteLog(FOR_TRACE,strLogTrace);
        }
    }

    switch(nCMD)
    {
    /*********************************上行应用协议******************************************/
    case TERMINAL_LOGIN_REPORT: //GPRS连接登
    {
        //打包成注册消息发给GIS主控程序,将开机信息记录对应的队列
        int nRes = TerminalLoginProccess( RecvPubMsg.nClientID,
            pData[1], //SIM卡号
            pData[3], //集群终端ID
            pData[4]  //本机型号
            );

        if(nRes > 0)
        {
            //向终端回登录确认
            DispatchAckToTerminal(RecvPubMsg.nClientID,ACK_TERMINAL_LOGIN,pData[5]);
            DispatchSetFixGpsDtCmdToTerminal(RecvPubMsg.nClientID);
            
            if(nRes == 1) //
            {
                //向GIS主控发送上线信息
                char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
                //写文件，报告终端上线
                sprintf(tempSendBuf,"%s,%d,1",pData[1],m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetDeviceKind());
                ReportDataToGisMain(A_TYPE_TERMINAL_LINE,tempSendBuf,strlen(tempSendBuf));
            }
        }
        break;
    }
	case TCPCONNECT_TICK_REPORT:  //握手信息协议-13
    {
		//中心应答握手
        DispatchAckToTerminal(RecvPubMsg.nClientID,ACK_TCPCONNECT_TICK,pData[3]);
        //重新启动心跳检查定时
        if(m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetConnectState() == CONNECT_GPRSLOGIN_OK)
        {
            m_tbPubTerminalInfo[RecvPubMsg.nClientID].SetConnectState(CONNECT_GPRSLOGIN_OK,m_nConnTickTimeOut);
        }
		break;
    }
    case PRIVATENET_LINEON_REPORT:  //专网上线协议-14
    case PRIVATENET_LINEOFF_REPORT: //专网下线协议-15
    {
        int nAckType;
        GPS_DATA_TYPE nGpsType;
        if(nCMD == PRIVATENET_LINEON_REPORT)
        {
            nGpsType = GPS_DATA_PRIVATENET_LINEON;
            nAckType = ACK_PRIVATENET_LINEON;
        }
        else
        {
            nGpsType = GPS_DATA_PRIVATENET_LINEOFF;
            nAckType = ACK_PRIVATENET_LINEOFF;
        }

        //给终端应答 pData[13] 为ACK字段
        DispatchAckToTerminal(RecvPubMsg.nClientID,nAckType,pData[13]);

        //专网上下线GPS数据
//         PublicGpsDataProcess(nGpsType,RecvPubMsg.nClientID,
// 			    pData[1], //SIM卡号
// 			    pData[3], //时间
// 			    pData[4], //N/S 纬度标志
// 			    pData[5], //纬度
// 			    pData[6], //E/W 经度标志
// 			    pData[7], //经度
// 			    pData[8], //定位标志 是否有效
// 			    pData[9],  //速度
//              pData[10],  //方向
//              pData[11],  //日期
//              pData[14]  //状态
// 			);
        
        break;
    }
    case GPS_TINMEROUT_REPORT:  //固定时间间隔主动上传GPS信息-00
    { 
        char strTempState[9] = {0};
        if(nCount == 14) //新协议
        {
            if(m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetConnectState() != CONNECT_GPRSLOGIN_OK)
            {
                gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误,终端未登记,DeviceCode:%s\n",strCurrTime,RecvPubMsg.nClientID,pData[1]);
                return FALSE;
		    } 
        }
        else if(nCount == 13)
        {
            if(m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetConnectState() != CONNECT_GPRSLOGIN_OK)
            {
                if(TerminalLoginProccess( RecvPubMsg.nClientID,
                    pData[1], //SIM卡号
                    pData[3], //集群终端ID
                    pData[4],  //本机型号
                    TRUE //是老协议
                    ) == 1)
                {
                    //
                    //写文件，终端上线
                    //向GIS主控发送上线信息
                    char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
                    sprintf(tempSendBuf,"%s,%d,1",pData[1],m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetDeviceKind());
                    ReportDataToGisMain(A_TYPE_TERMINAL_LINE,tempSendBuf,strlen(tempSendBuf));
                }
            }
        }
        else
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误的定时上传GPS消息,字段个数有误\n",strCurrTime,RecvPubMsg.nClientID);
            return FALSE;
        }
        
        memcpy(strTempState,pData[12],8);
		if(!PublicGpsDataProcess(GPS_DATA_TINMEROUT,RecvPubMsg.nClientID,
			    pData[1], //SIM卡号
			    pData[3], //时间
			    pData[4], //N/S 纬度标志
			    pData[5], //纬度
			    pData[6], //E/W 经度标志
			    pData[7], //经度
			    pData[8], //定位标志 是否有效
			    pData[9],  //速度
                pData[10],  //方向
                pData[11],  //日期
                strTempState,  //状态
                RecvPubMsg.MessagePacket
			))
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(定时上传GPS消息),字段格式错\n",strCurrTime,RecvPubMsg.nClientID);
			
            return FALSE;
        }
		break;
    }
    case GPS_HISTORY_REPORT: //历史GPS数据
        //if(!PubNetHistoryGpsDataProcess(pData[1], pData[3],pData[4]))
        //{
        //    gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(历史GPS消息),字段格式错\n",strCurrTime,RecvPubMsg.nClientID);
        //    return FALSE;
        //}
        break;
	case GPS_MONITOR_RESPONSE:  //即时跟踪GPS协议-01
    {
        if(m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetConnectState() != CONNECT_GPRSLOGIN_OK)
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误,终端未登记,DeviceCode:%s\n",strCurrTime,RecvPubMsg.nClientID,pData[1]);
            return FALSE;
		}
        if(!PublicGpsDataProcess(GPS_DATA_MONITOR,RecvPubMsg.nClientID,
                pData[1], //SIM卡号
			    pData[3], //时间
			    pData[4], //N/S 纬度标志
			    pData[5], //纬度
			    pData[6], //E/W 经度标志
			    pData[7], //经度
			    pData[8], //定位标志 是否有效
			    pData[9],  //速度
                pData[10],  //方向
                pData[11],  //日期
                pData[12],  //状态
                RecvPubMsg.MessagePacket
            ) )
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(即时跟踪GPS信息),字段格式错\n",strCurrTime,RecvPubMsg.nClientID);
            return FALSE;
        }
        break;
    }
    case GPS_BOUNDCALL_RESPONSE:  //应答区域点名GPS协议-02
    {
        if(m_tbPubTerminalInfo[RecvPubMsg.nClientID].GetConnectState() != CONNECT_GPRSLOGIN_OK)
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(区域点名GPS信息),终端未登记,DeviceCode:%s\n",strCurrTime,RecvPubMsg.nClientID,pData[1]);
            return FALSE;
		}
        break;
    }
    case SEND_MESSAGE_ANSWER:
    {
        int nMsgAck = 0;
        sscanf(pData[3], "%4x",&nMsgAck);
        SendMessageAnswerOkProcess(RecvPubMsg.nClientID,pData[1],nMsgAck);
        break;
    }
    case QUERY_MY_POSITION://上传请求查询“我的位置”信息协议-16
    {
        //写文件,请求我的位置
        break;
    }
    case QUERY_DESTINATION_POSITION: //上传请求查询目的地经纬度协议-17
    {
        //写文件,上传请求查询目的地经纬度协议
        break;
    }
    case CTL_TERMINAL_MOUNTGUARD_RESPONSE:  //终端上班下班-18
    {
        int nTmpGuardState = atoi(pData[3]);
        
        //应答上班下班
        DispatchAckToTerminal(RecvPubMsg.nClientID,ACK_TERMINAL_MOUNTGUARD,pData[13]);
                
        GPS_DATA_TYPE nGpsType;
        if(nTmpGuardState == 1)
        {
            nGpsType = GPS_DATA_MY_ONDUTY;
        }
        else
        {
            nGpsType = GPS_DATA_MY_OFFDUTY;
        }

        //上班下班GPS数据
//         PublicGpsDataProcess(nGpsType,RecvPubMsg.nClientID,
// 			    pData[1], //SIM卡号
// 			    pData[3], //时间
// 			    pData[4], //N/S 纬度标志
// 			    pData[5], //纬度
// 			    pData[6], //E/W 经度标志
// 			    pData[7], //经度
// 			    pData[8], //定位标志 是否有效
// 			    pData[9],  //速度
//                 pData[10],  //方向
//                 pData[11],  //日期
//                 pData[14]  //状态
// 			);
        break;
    }
    /*****************************上行应答控制命令协议**************************************/

    /***********************************上行报警协议****************************************/

    /*********************************上行应答设置参数协议**********************************/

    /*********************************上行应答读取参数协议**********************************/

    /***************************************************************************************/
    default:
        //gWriteLogApp.WriteLog(FOR_ALARM,"%s 不能识别的数据\n",strCurrTime);
        strLogTrace.Format("%s [ID:%d]不能识别的数据,%s\n",
                strCurrTime,
                RecvPubMsg.nClientID,
                RecvPubMsg.MessagePacket);

        gWriteLogApp.WriteLog(FOR_ALARM,strLogTrace);

        break;
    }

    return TRUE;
}

void CPubNetControl::DispatchAckToTerminal(int nClientID,int AckType,const char *AckValue)
{
    int nAckValue = 0;
    sscanf(AckValue,"%X",&nAckValue);
    
    PostThreadMessage(m_dwDataDispatchThreadID,
        WM_SEND_ACK_TERMINAL_MESSAGE,
        MAKELPARAM((WORD)nClientID, (WORD)AckType),
        (WORD)nAckValue);
}

//设置设置定时上传时间
void CPubNetControl::DispatchSetFixGpsDtCmdToTerminal(int nClientID)
{
    if(m_nOpenGpsModel == OPEN_GPS_GPRSLOGIN)
    {
        PostThreadMessage(m_dwDataDispatchThreadID,
            WM_SET_TERMINAL_GPSINTERVAL_MESSAGE,
            (WORD)nClientID,
            (WORD)m_nInitFixGpsDt);
    }
    else if(m_nOpenGpsModel == OPEN_GRPS_SOON )
    {
        PostThreadMessage(m_dwDataDispatchThreadID,
            WM_SET_TERMINAL_GPSMONITOR_MESSAGE,
            MAKELPARAM((WORD)nClientID, (WORD)m_nInitFixGpsDt),
            (WORD)m_nMonitorGPSCount);
    }
}

//GPRS连接登录消息处理，在系统中登记终端信息（记录对应的队列）
int CPubNetControl::TerminalLoginProccess( int nClientID,
                                     const char *strDeviceCode, //DeviceCode
                                     const char * strTrunkNum, //集群ID
                                     const char *strModel, //本机型号
                                     BOOL isOldProtocol)
{  
    int nRes = 0;
    //判断连接是否以及被登记
    if(m_tbPubTerminalInfo[nClientID].GetConnectState() == CONNECT_GPRSLOGIN_OK)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]终端GPRS连接已经登记,DeviceCode:%s \n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),
            nClientID,
            m_tbPubTerminalInfo[nClientID].GetDeviceNum());
        
        return nRes;
    }
    
    nRes = 1;
    //查询该号码是否已经登录
    for(int i = 0; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        
        if(m_tbPubTerminalInfo[i].GetConnectState() == CONNECT_GPRSLOGIN_OK)
        {
            if( (strcmp(m_tbPubTerminalInfo[i].GetDeviceNum(),strDeviceCode) == 0) &&
                nClientID != i)
            {
                
                gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]有相同DeviceCode的终端登录，强制断开之前的连接,DeviceCode:%s\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),i,strDeviceCode);
                
                m_tbPubTerminalInfo[i].SetConnectState(CONNECT_NULL);

                //关闭TCP连接
                m_SockPubNetServer.CloseClient(i);
                
                
                nRes = 2;
                break;
            }
        }
    }
    
    //终端型号
    int nTempKind ;
    if( strcmp(strModel,"TC890GM") == 0 )
    {
        nTempKind = KIND_TC890GM;
    }
    else if( strcmp(strModel,"TM800") == 0 )
    {
        nTempKind = KIND_TM800;
    }
    else
    {
        nTempKind = KIND_UNKNOWN;
    }
    
    m_tbPubTerminalInfo[nClientID].SetDeviceNum(strDeviceCode);
    m_tbPubTerminalInfo[nClientID].SetDeviceKind(nTempKind);
    
    m_tbPubTerminalInfo[nClientID].SetConnectState(CONNECT_GPRSLOGIN_OK,m_nConnTickTimeOut);
    
    m_tbPubTerminalInfo[nClientID].m_bOldProtocol = isOldProtocol;
    
    return nRes;
}

//收到发送文字信息成功应答处理过程 add by kevin 2009-12-23
void CPubNetControl::SendMessageAnswerOkProcess(int nPubDeviceID,const char *pcDeviceNum,int nAckValue)
{
    if(!m_tbPubTerminalInfo[nPubDeviceID].SendMessageAnswerOkCheck(nAckValue))
    {
        return;
    }

    CString strTemp;
    
    int nSendMsgID = m_tbPubTerminalInfo[nPubDeviceID].GetSendMsgID();
    if(nSendMsgID <= 0)
    {
        return;
    }
    
    strTemp.Format("%d",nSendMsgID);
    
    char TempMsgPacket[MAX_GPS_MESSAGE_LEN] = {0};

    int nTempLen;
    sprintf(TempMsgPacket,"%s,03,%d", //03表示下发警情文字信息
        pcDeviceNum,
        nSendMsgID);
    nTempLen = strlen(TempMsgPacket);
    //写文件，向主控报告发送文字消息应答,MsgID
    ReportDataToGisMain(A_TYPE_APPCMD_TERMINAL,TempMsgPacket,nTempLen);

    //change by kevin 2010-5-26
    m_tbPubTerminalInfo[nPubDeviceID].StopSendMessageWaitAnswer();
}

//处理GPS数据
BOOL CPubNetControl::PublicGpsDataProcess(GPS_DATA_TYPE gpsDataType,
                          int nClientID,
                          const char *SIMNum, //SIM卡号
                          const char * strTimer, //时间
                          const char *nsFlag,   //N/S
                          const char *latitude, //纬度
                          const char *ewFlag,  //E/W
                          const char *longitude, //经度
                          const char *avFlag,  //是否有效
                          const char *speed,   //速度
                          const char *Direction,   //方向
                          const char *Date,   //日期;
                          const char *state,
                          const char *strAllData
    )
{
    //数据检测
    if(strlen(strTimer) != 6)
    {
        return FALSE;
    }
    
    if(strlen(avFlag) != 1 || strlen(nsFlag) != 1 || strlen(ewFlag) != 1)
    {
        return FALSE;
    }   
    if(latitude[4] != '.' || strlen(latitude) != 9)
    {
        return FALSE;
    }
    if(longitude[5] != '.' || strlen(longitude) != 10)
    {
        return FALSE;
    }
    
    if(!m_bDispatchZeroGPS)
    {
        float nlatitude = atol(latitude);
        if(nlatitude < 1 )
        {
            return TRUE;
        }
    }

    char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
    sprintf(tempBuf,"%s,%.2X,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",SIMNum,gpsDataType,strTimer,
        nsFlag,latitude,ewFlag,longitude,
        avFlag,speed,Direction,Date,
        state);
    
    gRecvGpsCount++;
    
    ReportDataToGisMain(A_TYPE_TERMINAL_GPS,tempBuf,strlen(tempBuf));

    return TRUE;
}

//将数据写文件,然后上传到FTP
BOOL CPubNetControl::ReportDataToGisMain(int nType,const char *DataBuf,int nBufLen)
{
    if(DataBuf == NULL)
    {
        return FALSE;
    }
    char TempMsgPacket[MAX_GPS_MESSAGE_LEN] = {0};
    BOOL bRes = TRUE;
    int nTempLen;
    switch(nType)
    {
    case A_TYPE_TERMINAL_GPS: //终端GPS数据
        nTempLen = nBufLen + 6;
        sprintf(TempMsgPacket,"%.4X%.2X%s\n",nTempLen,A_TYPE_TERMINAL_GPS,DataBuf);
        break;
    case A_TYPE_TERMINAL_LINE: //终端上下线
        nTempLen = nBufLen + 6;
        sprintf(TempMsgPacket,"%.4X%.2X%s\n",nTempLen,A_TYPE_TERMINAL_LINE,DataBuf);
        break;
    case A_TYPE_APPCMD_TERMINAL:
        nTempLen = nBufLen + 6;
        sprintf(TempMsgPacket,"%.4X%.2X%s\n",nTempLen,A_TYPE_APPCMD_TERMINAL,DataBuf);
        break;
    default:
        bRes = FALSE;
        break;
    }
    
    if(bRes)
    {
        //写文件
		m_myFtpConveyFile.WriteData(0,TempMsgPacket);
    }
    return bRes;
}

//读从FTP下载下来的文件，与主控的交互信息 DataAnalyseThreadProc
BOOL CPubNetControl::OnReadMainFtpConveyFile()
{
    CFileFind IsValidFile;
    if(!IsValidFile.FindFile(m_strFTPDownLoadFileName)) 
    {
        //文件不存在
        return FALSE;
    }
    
    FILE *fptr;
    if((fptr = fopen(m_strFTPDownLoadFileName, "rb")) == NULL)
    {    
        //打开文件失败
        return FALSE;
    }
    
    unsigned char rdDataBuf[MAX_GPS_MESSAGE_LEN*2] = {0};
    int nReadLen = 0;
    BOOL bLoop = TRUE;
    int nSeekCount = 0;
    while(bLoop)
    {
        if(fseek(fptr,nSeekCount, SEEK_SET) == 0)
        {
            nReadLen = fread(rdDataBuf,1,MAX_GPS_MESSAGE_LEN, fptr);
            //对读取的数据进行分析
            if(nReadLen > 0)
            {
                int nCurrPoint = 0;
                char strGetNumber[MAX_GPS_MESSAGE_LEN] = {0}; 
                int nNumLen;
                CString strAddNum;
                
                for(int i = 0; i < nReadLen; i++)
                {
                    if(rdDataBuf[i] == '\r' || rdDataBuf[i] == '\n') 
                    {
                        memset(strGetNumber,0,32);
                        nNumLen = i - nCurrPoint;
                        memcpy(strGetNumber,rdDataBuf+nCurrPoint,nNumLen);
                        nCurrPoint = i+1;
                        strAddNum.Format(_T("%s"),strGetNumber);
                        if(strAddNum.GetLength() > 4)
                        {
                            FtpMainDataAnalyse(strGetNumber);         
                        }
                    }
                }
                nSeekCount = nSeekCount + nCurrPoint;
                if(nSeekCount == 0) //读了MAX_GPS_MESSAGE_LEN没有找到一行结束
                {
                    nSeekCount = MAX_GPS_MESSAGE_LEN;
                }
            }
            
            if( nReadLen < MAX_GPS_MESSAGE_LEN)
            {
                //文件读完退出
                fclose(fptr);  
                fptr = NULL;
                bLoop = FALSE;
            }
        }
    }
    
    if(fptr)
    {
        fclose(fptr); 
    }
    
	TRY
	{
		//删除文件
		CFile::Remove(m_strFTPDownLoadFileName);
	}
	CATCH( CFileException, e )
	{
		TRACE("删除文件失败");
	}
	END_CATCH

    
    return TRUE;
}

typedef enum enumGisMainAppCmd
{
    aAPP_CMD_SetFixGpsDt = 0, //设置固定上传GPS信息时间间隔
    aAPP_CMD_GpsTrace = 1, //即时跟踪命令
    aAPP_CMD_AreaGpsCall = 2,//区域点名
    aAPP_CMD_SendMessage = 3,
    app_MAX = 4,
}GISMAIN_APP_CMD;

static int g_AppCmdTB[app_MAX] = {SET_GPS_TINMEROUT,SET_GPS_MONITOR,SET_GPS_BOUNDCALL,APP_SEND_MESSAGE};

//分析读文件读到的数据包
BOOL CPubNetControl::FtpMainDataAnalyse(const char *pstrMainDataPacket)
{
    if(pstrMainDataPacket == NULL)
    {
        return FALSE;
    }

    char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
    char *pData[20];
    int i;
    for(i = 0; i < 20; i++)
    {
        pData[i] = NULL;
    }

    int nDataLen = strlen(pstrMainDataPacket);
    memcpy(tempBuf,pstrMainDataPacket,nDataLen);
    char *pBuf = tempBuf;
    i = 0;
    int nCount = 0;
    while((pData[i]=strtok(pBuf,",")) != NULL)
    {
        i++;
        nCount++;
        pBuf = NULL; 
    }
    if(nCount < 1)
    {
        return FALSE;
    }
    
    int nCMD = 0;
    int nPackageLen;
    if(strlen(pData[0]) < 6)
    {
        return FALSE;
    }
    sscanf(pData[0], "%4x%2x",&nPackageLen,&nCMD);

    if(nPackageLen != nDataLen)
    {
        //协议格式错误
        return FALSE;
    }
    int nMoveLen = 6;
    int nTempValue;
    switch(nCMD)
    {
    case A_TYPE_LOGIN: //主控启动后会写入这个命令
        MainStartSuccessResponse();
        break;
    case A_TYPE_APPCMD_TERMINAL: //下发应用命令给终端,固定上传,即时跟踪,文字信息
    {
        int nClientID = GetClientID(pData[0]+6); 
        nTempValue = atoi(pData[1]);
        int nMoveLen = strlen(pData[0]) + strlen(pData[1]) + 2;
        if(nClientID >= 0 && nTempValue < app_MAX)
        {
            SendDataToPubNetTerminal(nClientID,
                g_AppCmdTB[nTempValue],
                pstrMainDataPacket+nMoveLen, //为截去了终端号码、和信息类型的数据
                nDataLen - nMoveLen);
        }
        break;
    }
    default:
        break;
    }

    return TRUE;

}

//读文件读到主控启动成功消息处理,将所有的在线终端写文件
void CPubNetControl::MainStartSuccessResponse()
{
    int i = 0; 
    char TempBuf[MAX_GPS_MESSAGE_LEN] = {0};
    for(i = 0; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        if(m_tbPubTerminalInfo[i].GetConnectState() == CONNECT_GPRSLOGIN_OK)
        {
            sprintf(TempBuf,"%s,%d,1",m_tbPubTerminalInfo[i].GetDeviceNum(),m_tbPubTerminalInfo[i].GetDeviceKind());
            
            ReportDataToGisMain(A_TYPE_TERMINAL_LINE,TempBuf,strlen(TempBuf));
            
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//通过SIM卡号查询连接ID
int CPubNetControl::GetClientID(const char *strDeviceCode)
{
    if(strDeviceCode == NULL)
    {
        return -1;
    }
    
    if(strlen(strDeviceCode) == 0)
    {
        return -1;
    }
    
    //查询该号码是否已经登录
    for(int i = 0; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        if( m_tbPubTerminalInfo[i].GetConnectState() == CONNECT_GPRSLOGIN_OK &&
            (strncmp(m_tbPubTerminalInfo[i].GetDeviceNum(),strDeviceCode,strlen(strDeviceCode)) == 0) )
        {
            return i;
        }
    }
    
    return -1;
}


//发送数据给公网终端的公共函数
BOOL CPubNetControl::SendDataToPubNetTerminal(int nClientID,int nType,const char *DataBuf,int nBufLen)
{
    char tempStr[MAX_GPS_MESSAGE_LEN] = {0};
    
    CString StrTrace;
    CString strCurrTime;

    CString strSIMNum = _T(" ");
    BOOL bErrorFlag = FALSE; 
    int nTempValue;
    if(nClientID >= 0 && nClientID < MAX_PUBLICNET_CLIENT_CONNECT)
    {
        
        strSIMNum.Format(_T("%s"),m_tbPubTerminalInfo[nClientID].GetDeviceNum());
    }
    else
    {
        return FALSE;
    }
    
    strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
    if(m_tbPubTerminalInfo[nClientID].m_bOldProtocol)
    {
        gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>终端为老的协议版本,不支持下行数据,DeviceCode:%s\n",
            strCurrTime,
            nClientID,strSIMNum);

        return FALSE;
    }

    int nChksum;
    char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
    switch(nType)
    {
    /*************************中心下发应用命令信息******************************/    
    case SET_GPS_TINMEROUT://设置固定上传GPS时间间隔命令-00
    {
        int nTimeFix = atoi(DataBuf);
        
        StrTrace = _T("设置固定上传GPS时间间隔");
        sprintf(tempStr,"HYT,%.2X,%.4d,",nType,nTimeFix);
        
        break;
    }  
    case SET_GPS_MONITOR://中心下发即时跟踪指令-01
    {
        int nCount = 0;
        int nTimeSlot = 0;
        sscanf(DataBuf,"%d,%d",&nTimeSlot,&nCount);
        
        sprintf(tempStr,"HYT,%.2X,%.4d,%.5d,",nType,nTimeSlot,nCount);
        StrTrace = _T("即时跟踪命令");

        break;
    }  
    case SET_GPS_BOUNDCALL: //区域点名信息-02
    {
        StrTrace = _T("设置区域点名");
        sprintf(tempStr,"HYT,%.2X,%s,",nType,DataBuf);

        break;
    }
    case APP_SEND_MESSAGE: //下发文字信
    {
        StrTrace = _T("下发文字信");
        char tmpMsg[MAX_GPS_MESSAGE_LEN] = {0};
        memcpy(tmpMsg,DataBuf,nBufLen);
        
        strtok(tmpMsg,",");
        char *ptmpMsg = tmpMsg;
        char *pStrMsgID = strtok(NULL,",");
        if(ptmpMsg == NULL || pStrMsgID == NULL)
        {
            return FALSE;
        }
        int nMsgID = atoi(pStrMsgID);

        if(g_nSendTxtMsgInterval > 0 && nMsgID > 0)
        {
            //文字消息延时发送
            m_tbPubTerminalInfo[nClientID].AddMsgTxtToBuffer(nMsgID,ptmpMsg);

            return TRUE;
        }
        else
        {
            //文字消息立即发送
            _snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",nType,ptmpMsg,m_nAckCount[nType]);
            if(nMsgID > 0)
            {
                //启动重发处理
                m_tbPubTerminalInfo[nClientID].StartSendMessageWaitAnswer( m_nAckCount[nType],
                    nMsgID,
                    ptmpMsg,
                    m_nSendTxtMsgWaitAckTimeOut);
            }
        }
        break;
    }
    /*************************中心下发控制命令信息******************************/
    case CTL_PRIVATENET_LINE://控制专网上线和专网下线命令-20
    {
        nTempValue = atoi(DataBuf);
        if(nTempValue > 0)
        {
            nTempValue = 1;
        }
        
        StrTrace = _T("控制专网上线和专网下线");
        sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,m_nAckCount[nType]);

        break;
    }
    case CTL_TERMINAL_WAKEUP://控制终端集群遥毙和摇醒-21
    {
        nTempValue = atoi(DataBuf);
        if(nTempValue > 0)
        {
            nTempValue = 1;
        }
        StrTrace = _T("控制终端集群遥毙和摇醒");
        sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,m_nAckCount[nType]);
        break;
    }
    /*************************中心下发应答报警信息******************************/
    case ACK_GPS_ALERTING://应答报警及解除报警-30
    {
        StrTrace = _T("应答报警及解除报警");
        nTempValue = atoi(DataBuf);
        sprintf(tempStr,"HYT,%.2X,%.2X,%.4X,",nType,nTempValue,m_nAckCount[nType]);
        break;
    }
    /*************************中心下发应答报警信息******************************/
    case CFG_COMMUNICATION_MODEL_CMD://设置通讯方式参数-60
    {
        nTempValue = atoi(DataBuf);
        if(nTempValue > 0)
        {
            nTempValue = 1;
        }
        StrTrace = _T("设置通讯方式参数");
        sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,m_nAckCount[nType]);
        break;
    }
    /*************************中心下发应答应用信息******************************/
    

    /*************************中心下发设置命令**********************************/
    case CFG_GPRS_PARAMETER_CMD://设置GPRS参数-61
    {
        StrTrace = _T("设置GPRS参数");
        sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }
    case CFG_GSM_PARAMETER_CMD://设置GSM参数-62
    {
        StrTrace = _T("设置GSM参数");
        sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }
    case CFG_TERMINAL_NUM_CMD://设置终端专网号码-63
    {    
        StrTrace = _T("设置终端专网号码");
        sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]);
        break;
    }
    case CFG_TEMPGROUP_NUM_CMD://设置动态临时编组号码-64
    {
        char tempCps[16] = {0};
        char tempCrateFlag[2] = {0};
        tempCrateFlag[0] = DataBuf[0];
        int nCreatFlag = atoi(tempCrateFlag);
        if( !GetCpsxIdValue(DataBuf+2,tempCps))
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s <下行>错误的号码格式,设置动态临时编组号码,DeviceCode:%s,cmdData:%s\n",strCurrTime,strSIMNum,DataBuf);
            return FALSE;
        }
        StrTrace = _T("设置动态临时编组号码");
        sprintf(tempStr,"HYT,%.2X,%d,%s,%.4X,",nType,nCreatFlag,tempCps,m_nAckCount[nType]); 
        break;
    }
    case CFG_ALERT_NUM_CMD://设置专网倒放告警号码-65
    {
        StrTrace = _T("设置专网倒放告警号码");
        sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }
    case CFG_SAFETYCHECK_NUM_CMD://设置专网安检号码-66
    {
        StrTrace = _T("设置专网安检号码");
        sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }
    case CFG_GPSCENTER_NUM_CMD://设置专网GPS中心号码-67
    {
        StrTrace = _T("设置专网GPS中心号码");
        sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }
    case CFG_BACK_MODEL_CMD://设置专网背景模式-68
    {
        StrTrace = _T("设置专网背景模式");
        sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,m_nAckCount[nType]); 
        break;
    }

    /************************中心下发读取参数命令********************************/
    case QUERY_COMMUNICATION_MODEL_CMD://读取通讯方式参数-80
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取通讯方式参数");
        break;
    }
    case QUERY_GPS_PARAMETER_CMD://读取GPRS参数-81
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取GPRS参数");
        break;
    }
    case QUERY_GSM_PARAMETER_CMD://读取GSM参数-82
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取GSM参数");
        break;
    }
    case QUERY_TERMINAL_NUM_CMD://读取终端专网号码-83
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取终端专网号码");
        break;
    }
    case QUERY_TEMPGROUP_NUM_CMD://读取动态临时编组号码-84
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取动态临时编组号码");
        break;
    }
    case QUERY_ALERT_NUM_CMD://读取专网倒放告警号码-85
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取专网倒放告警号码");
        break;
    }
    case QUERY_SAFETYCHECK_NUM_CMD://读取专网安检号码-86
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取终端安全检测号码");
        break;
    } 
    case QUERY_GPSCENTER_NUM_CMD://读取专网GPS中心号码-87
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取专网GPS中心号码");
        break;
    }
    case QUERY_BACK_MODEL_CMD://读取专网背景扫描模式-88
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取专网背景扫描模式");
        break;
    }
    case QUERY_MAGNETIC_BOUND_CMD://读取专网场强参数-89
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取专网场强参数");
        break;
    }
    case QUERY_TERMINAL_TYPE_CMD://读取终端机型参数-8A
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取终端机型参数");
        break;
    }
    case QUERY_WAKEUP_CMD ://读取终端遥毙摇醒状态-8B
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取终端遥毙摇醒状态");
        break;
    }
    case QUERY_GPSSEND_INTERVAL_CMD: //读取GPS固定上传时间间隔参数
    {
        sprintf(tempStr,"HYT,%.2X,%.4X,",nType,m_nAckCount[nType]);
        StrTrace = _T("读取GPS固定上传时间间隔参数");
        break;
    }
    default:
        return FALSE;
    }

    nChksum = GetChksum(tempStr);
    sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);

    m_nAckCount[nType]++;
    if(m_nAckCount[nType] >= 0xFFFF)
    {
        m_nAckCount[nType] = 0;
    }

    if(nType != SET_GPS_BOUNDCALL )
    {
        //发送数据
        if( m_SockPubNetServer.Send(nClientID,
            MessagePacket,
            strlen(MessagePacket)) == TCPSERVER_ERROR_SEND)
        {            
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>发送数据给终端失败,关闭TCP连接,DeviceCode:%s\n",
                strCurrTime,
                nClientID,
                strSIMNum);

            m_SockPubNetServer.CloseClient(nClientID);

            return FALSE;
        }

        gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>%s,DeviceCode:%s,数据:%s\n",
            strCurrTime,
            nClientID,
            StrTrace,strSIMNum,MessagePacket);
    }
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
UINT WINAPI CPubNetControl::DataDispatchThreadProc(void *lpParameter)
{
    CPubNetControl *pHytPublicSys = NULL;
    pHytPublicSys = (CPubNetControl *)lpParameter;
    
    //TRACE(_T("Into System thread%X\n"), pHytPublicSys->m_hSysThread);
    ResetEvent(pHytPublicSys->m_hDataDispatchThreadEndEvt);
    int timerid = SetTimer( NULL,NULL, 1000, 0L ); //产生1s的基准定时
    MSG msg;
    BOOL bcontinue = TRUE;
    while(bcontinue)
    {
        if( GetMessage(&msg,NULL,0,0) )
        {
            switch(msg.message)
            {
            case WM_THREAD_STOP:
                bcontinue = FALSE;
                break;
            case WM_TIMER:
                pHytPublicSys->OnCheckSendMessageWaitAnswerTimerOut();
                pHytPublicSys->OnCheckGetSendMsgTxtTimerOut();
                break;
            case WM_SET_TERMINAL_GPSINTERVAL_MESSAGE:
                pHytPublicSys->OnSetTerminalGpsIntervalMessage(msg.wParam,msg.lParam);
                break;
            case WM_SET_TERMINAL_GPSMONITOR_MESSAGE:
                pHytPublicSys->OnSetTerminalGpsMonitorMessage(msg.wParam,msg.lParam);
                break;
            case WM_SEND_ACK_TERMINAL_MESSAGE:
                pHytPublicSys->OnAnswerAckToTerminal(msg.wParam,msg.lParam);
                break;
            case WM_DISPATCH_DATA_MESSAGE:
                pHytPublicSys->OnDispatchDataMessage();
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }
    
    if(timerid != 0)
    {
        KillTimer(NULL, timerid);
    }
    
    SetEvent(pHytPublicSys->m_hDataDispatchThreadEndEvt);
    TRACE(_T("Quit Read DataBase thread\n"));
    _endthreadex(0);
    
    return 0;
}

//设置固定上传时间间隔
void CPubNetControl::OnSetTerminalGpsIntervalMessage(WPARAM wParam, LPARAM lparam)
{
    char tempStr[8] ={0};
    sprintf(tempStr,"%d",lparam);
    SendDataToPubNetTerminal((int)wParam,SET_GPS_TINMEROUT ,tempStr,strlen(tempStr));
}

//设置及时跟踪
void CPubNetControl::OnSetTerminalGpsMonitorMessage(WPARAM wParam, LPARAM lparam)
{
    char tempStr[64] ={0};
    int nClientID = LOWORD(wParam);  
    int nTimeSlot = HIWORD(wParam);
    int nSendCount = (int)lparam;
    
    sprintf(tempStr,"%d,%d",nTimeSlot,nSendCount);

    SendDataToPubNetTerminal(nClientID,SET_GPS_MONITOR ,tempStr,strlen(tempStr));
    
}

//将ACK数据发给终端
BOOL CPubNetControl::OnAnswerAckToTerminal(WPARAM wParam, LPARAM lparam)
{
    int nClientID = LOWORD(wParam);  
    int AckType = HIWORD(wParam);
    
    int nAckValue = (int)lparam;
    
    char tempStr[128] = {0};
    char sendBuf[256] = {0};
    CString StrTrace;
    switch(AckType)
    {
    case ACK_TERMINAL_LOGIN: //应答终端GPRS登录信-10
        sprintf(tempStr,"HYT,10,%.4X,",nAckValue);
        StrTrace = _T("应答终端GPRS登录");
        break;
    case ACK_TERMINAL_OPEN: //应答终端开机信息-11
        sprintf(tempStr,"HYT,11,%.4X,",nAckValue);
        StrTrace = _T("应答终端开机");
        break;
    case ACK_GPS_TERMINAL_OPEN: //应答终端开机GPS信息-12
        sprintf(tempStr,"HYT,12,%.4X,",nAckValue);
        StrTrace = _T("应答终端开机GPS");
        break;
    case ACK_TCPCONNECT_TICK: //应答握手
        sprintf(tempStr,"HYT,13,%.4X,",nAckValue);
        StrTrace = _T("应答握手");
        break;
    case ACK_PRIVATENET_LINEON: //向终端应答专网络上线确认
        sprintf(tempStr,"HYT,14,%.4X,",nAckValue);
        StrTrace = _T("应答专网上线确认");
        break;
    case ACK_PRIVATENET_LINEOFF: //向终端应答专网络下线确认
        sprintf(tempStr,"HYT,15,%.4X,",nAckValue);
        StrTrace = _T("应答专网络下线确认");
        break;
    case ACK_GPS_ALERTING: //向终端应答报警确认
        sprintf(tempStr,"HYT,30,00,%.4X,",nAckValue);
        StrTrace = _T("应答报警确认");
        break;
    case ACK_TERMINAL_MOUNTGUARD: //应答上班下班
        sprintf(tempStr,"HYT,18,00,%.4X,",nAckValue);
        StrTrace = _T("应答上班下班");
        break;
    default:
        return FALSE;
        break;
    }
    
    CString strCurrTime;
    strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
    //取效验和
    int nChksum = GetChksum(tempStr);
    sprintf(sendBuf,"*%s$%X#",tempStr,nChksum);
    
    //发送数据
    if( m_SockPubNetServer.Send(nClientID,
        sendBuf,
        strlen(sendBuf)) == TCPSERVER_ERROR_SEND )
    {
        
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>发送数据给终端失败,关闭TCP连接,DeviceCode:%s\n",
            strCurrTime,
            nClientID,
            m_tbPubTerminalInfo[nClientID].GetDeviceNum());
        
        m_SockPubNetServer.CloseClient(nClientID);
        
        return FALSE;
    }
    
    //if(AckType != ACK_TCPCONNECT_TICK)
    {
        gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>%s,DeviceCode:%s,data:%s\n",
            strCurrTime,
            nClientID,StrTrace,
            m_tbPubTerminalInfo[nClientID].GetDeviceNum(),
            sendBuf);
    }
    
    return TRUE;
    
}

//
BOOL CPubNetControl::OnDispatchDataMessage()
{
    m_csDispatchList.Lock();
    if(m_DispatchList.IsEmpty())
    {
        m_csDispatchList.Unlock();
        return FALSE;
    }

    DISPATCH_MESSAGE DispatchMsg;

    DispatchMsg = m_DispatchList.GetHead();
    m_DispatchList.RemoveHead();
    m_csDispatchList.Unlock();
    
    char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
    
    if(DispatchMsg.aPPID == 1) //发给GIS主控
    {
        return FALSE;
    }
   
    //发给终端
    memcpy(tempBuf,DispatchMsg.MessagePacket,MAX_GPS_MESSAGE_LEN);
    char *pBuf = tempBuf;
    int i= 0 ;
    char *pData[16];
    for(i = 0; i < 16; i++)
    {
        pData[i] = NULL;
    }
    
    int nCount = 0;
    i = 0;
    while((pData[i]=strtok(pBuf,",")) != NULL)
    {
        i++;
        nCount++;
        if(nCount >= 16)
        {
            return FALSE;
        }
        pBuf = NULL; 
    }

    if(nCount < 2)
    {
        return FALSE;
    }

    /*//i = 0;
    while((pData[i]=strtok(pBuf,",")) != NULL)
    {
        //i++;
        //if(i == 2)
        //{
        //    break;
        //}
        pBuf = NULL; 
    }*/

    int nClientID = GetClientID(pData[0]); 
    if(nClientID == -1)
    {
        CString strCurrTime;
        strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
        gWriteLogApp.WriteLog(FOR_ALARM,"%s <下行>终端还没有在系统登记,号码:%s:协议类型:%d,数据:%s\n",strCurrTime,
            pData[0],DispatchMsg.TypeID,DispatchMsg.MessagePacket);

        return FALSE; 
    }
    int nID = 0;
    sscanf(pData[1], "%2x",&nID);
    int nProtocalType = -1;
    //int nProtocalType = GetPubDownProtocalType(DispatchMsg.TypeID,nID);
    if(nProtocalType == -1)
    {
        return FALSE;
    }

    int nMoveLen = 0;
    nMoveLen = strlen(pData[0]) + strlen(pData[1]) + 1; //去掉终端号码,消息类型
    if(nMoveLen >= DispatchMsg.length)
    {
        //下发控制命令协议、下发应答和解除终端报警协议、下发读取参数给终端协议
        //查询类
        if(nProtocalType == CTL_TERMINAL_WAKEUP || nProtocalType == CTL_PRIVATENET_LINE)
        {
            char TempStr[4] ={0};
            nID = nID %2;  
            sprintf(tempBuf,"%d",nID);

            tempBuf[1] = NULL; //字符串结尾
            SendDataToPubNetTerminal(nClientID,
                nProtocalType,
                tempBuf,1);
        }
        else
        {
            SendDataToPubNetTerminal(nClientID,nProtocalType,NULL,0);
        }
    }
    else
    {
        //下发应用命令给终端协议、下发设置参数给终端协议
        nMoveLen = nMoveLen + 1;
        memcpy(tempBuf,DispatchMsg.MessagePacket+nMoveLen,DispatchMsg.length - nMoveLen );
        tempBuf[DispatchMsg.length - nMoveLen] = NULL; //字符串结尾

        SendDataToPubNetTerminal(nClientID,
            nProtocalType,
            tempBuf, //为截去了终端号码、和信息类型的数据
            DispatchMsg.length - nMoveLen);
    }

    return TRUE;
}

//================================================================
//检测是否有需要重发的文字消息
void CPubNetControl::OnCheckSendMessageWaitAnswerTimerOut()
{
    int i = 0;
    if(m_nSendTxtMsgWaitAckTimeOut <= 0)
    {
        return ;
    }
    
    for(i = 0; i< MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        int nRes = m_tbPubTerminalInfo[i].CheckResendMsgInfoTimerOut();
        if(nRes == 1)
        {
            ResendMessgToPubDevice(i,
                m_tbPubTerminalInfo[i].GetMessageInfo(),
                m_tbPubTerminalInfo[i].GetMsgAckValue());
        }
        else if( nRes == 2)
        {
            m_tbPubTerminalInfo[i].StopSendMessageWaitAnswer();
            
            gWriteLogApp.WriteLog(FOR_ALARM,"%s <GIS>向终端发送文字信息三次均未收到发送文字信息回复,%s\n",
                CTime::GetCurrentTime().Format("%H:%M:%S"),m_tbPubTerminalInfo[i].GetDeviceNum());
            
        }
    }
}

//重发文字消息
BOOL CPubNetControl::ResendMessgToPubDevice(int nPubDeviceID,const char *pcSendMsgInfo,int nAckVaule)
{
    char tempStr[MAX_GPS_MESSAGE_LEN] = {0};
    char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
    if(pcSendMsgInfo == NULL)
    {
        return FALSE;
    }
    
    if(strlen(pcSendMsgInfo) == 0)
    {
        return FALSE;
    }
    
    _snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",APP_SEND_MESSAGE,pcSendMsgInfo,nAckVaule);
    
    int nChksum = GetChksum(tempStr);
    sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);
    
    //发送数据
    if( m_SockPubNetServer.Send(nPubDeviceID,
        MessagePacket,
        strlen(MessagePacket)) == TCPSERVER_ERROR_SEND)
    {            
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>重发文字信息失败,关闭TCP连接,DeviceCode:%s\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),
            nPubDeviceID,
            m_tbPubTerminalInfo[nPubDeviceID].GetDeviceNum());
        
        m_SockPubNetServer.CloseClient(nPubDeviceID);
        
        return FALSE;
    }
    
    gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>重发文字信息,DeviceCode:%s,数据:%s\n",
        CTime::GetCurrentTime().Format("%H:%M:%S"),
        nPubDeviceID,
        m_tbPubTerminalInfo[nPubDeviceID].GetDeviceNum(),
        MessagePacket);
    
    return TRUE;
    
}

//
void CPubNetControl::OnCheckGetSendMsgTxtTimerOut()
{
    int i = 0;
    char csSendMsgTxt[MAX_GPS_MESSAGE_LEN] ={0};
    int nMsgID = 0;
    int nAckValue = m_nAckCount[APP_SEND_MESSAGE];
    
    char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
    char tempStr[MAX_GPS_MESSAGE_LEN] = {0};
    
    for(i = 0 ; i < MAX_PUBLICNET_CLIENT_CONNECT; i++)
    {
        if(m_tbPubTerminalInfo[i].GetMsgTxtTimerCheck(csSendMsgTxt,nMsgID))
        {
            _snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",APP_SEND_MESSAGE,csSendMsgTxt,nAckValue);
            
            int nChksum = GetChksum(tempStr);
            sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);
            
            //发送数据
            if( m_SockPubNetServer.Send(i,
                MessagePacket,
                strlen(MessagePacket)) == TCPSERVER_ERROR_SEND)
            {
                gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>发送文字信息失败,关闭TCP连接,DeviceCode:%s,数据:%s\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),
                    i,
                    m_tbPubTerminalInfo[i].GetDeviceNum(),
                    MessagePacket);
                
                m_SockPubNetServer.CloseClient(i);
            }
            else
            {
                gWriteLogApp.WriteLog(FOR_TRACE,"%s [ID:%d]<下行>发送文字信息,DeviceCode:%s,数据:%s\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),
                    i,
                    m_tbPubTerminalInfo[i].GetDeviceNum(),
                    MessagePacket);
                
                //启动重发定时
                m_tbPubTerminalInfo[i].StartSendMessageWaitAnswer( nAckValue,
                    nMsgID,
                    csSendMsgTxt,
                    m_nSendTxtMsgWaitAckTimeOut);
            }
            
            memset(MessagePacket,0,MAX_GPS_MESSAGE_LEN);
            
            m_nAckCount[APP_SEND_MESSAGE]++;
            if(m_nAckCount[APP_SEND_MESSAGE] >= 0xFFFF)
            {
                m_nAckCount[APP_SEND_MESSAGE] = 0;
            }
            
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//FTP上传下载



//
#include "StdAfx.h"
#include "GPSProc.h"
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
BOOL CGPSProc::CheckIPAddressFormat(const char *strIPAddress)
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
int CGPSProc::GetChksum(const char *strMsg)
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
BOOL CGPSProc::GetCpsxIdValue(const char *strValue, char *strOut)
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

CString CGPSProc::GetCpsxFltId(const char *strValue)
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

BOOL CGPSProc::GetCpsxFltId(const char *strValue, char *strOut)
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
CGPSProc::CGPSProc()
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
    
    int i;
    for(i = 0; i < MAX_PUB_PROTOCAL_DOWN; i++)
    {
        m_nAckCount[i] = 0;
    }
    
    memset(m_LocalIpAddress,0,MAX_IPADDRESS_LEN);
    
//    m_MppOLD.InitRoutineServer(MAX_PUBLICNET_CLIENT_CONNECT);
    
    
    m_bStartFlag = FALSE;
	m_IsIOCPActive = FALSE;
    
    
    InitFieldStatisticTable();

    SetLog();
}

CGPSProc::~CGPSProc()
{
//  m_MppOLD.ReleaseRoutineServer();
    WSACleanup();
    gWriteLogApp.StopLog();
}

///////////////////////////////////////////////////////////////////////////////////////////
//版本信息
void CGPSProc::InitMyVersion()
{
    char constVersion[128] = {0};
    
    _snprintf(constVersion, 128-1, "Hitom公网GPS接入系统 Version:%s", 
        "1.2.0.6", __DATE__, __TIME__);
    
    gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),constVersion);

	_snprintf(constVersion, 128-1, "Build Time:%s %s", __DATE__, __TIME__);

    gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),constVersion);
}

//初始化协议字段个数,用来判断协议格式是否正确
void CGPSProc::InitFieldStatisticTable()
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
BOOL CGPSProc::LoadSystemConfig()
{
    Crwini rwIniTemp;

    CString tempstr =_T("pGisPubNetCfg.ini");

    if(!rwIniTemp.CheckIniFile(tempstr))
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        
        return FALSE;
    }

    //当GPRS连接登录后GPS开启模式
    m_nOpenGpsModel = rwIniTemp.GetInt(_T("SystemSet"),_T("OpenGpsModel"),tempstr); 
    if(m_nOpenGpsModel > 0 )
    {
        //默认设置GPS主动上传时间间隔
        m_nInitFixGpsDt = rwIniTemp.GetInt(_T("SystemSet"),_T("DefaultFixGpsDt"),tempstr);
        if(m_nInitFixGpsDt <= 0)
        {
            m_nOpenGpsModel = 0;
        }  
        else
        {
            if(m_nInitFixGpsDt >= 60)
            {
                m_nOpenGpsModel = 1;
            }
            else
            {
                if(m_nInitFixGpsDt < 5) //不能低于5s
                {
                    m_nInitFixGpsDt = 5;
                }
                m_nOpenGpsModel = 2;
            }
        }
        
        //GSP次数
        m_nMonitorGPSCount = rwIniTemp.GetInt(_T("SystemSet"),_T("MonitorGPSCount"),tempstr);
        if(m_nMonitorGPSCount <= 0)
        {
            m_nMonitorGPSCount = 0;
        }
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

    //消息重发超时
    m_nReSendWaitAckTimeOut = rwIniTemp.GetInt(_T("SystemSet"),_T("ReSendWaitAckTimeOut"),tempstr);

    //发生文字消息时间间隔
    g_nSendTxtMsgInterval = rwIniTemp.GetInt(_T("SystemSet"),_T("SendTxtMsgInterval"),tempstr);
    if(g_nSendTxtMsgInterval <= 0 )
    {
        g_nSendTxtMsgInterval = 1;
    }

    return TRUE;

}

BOOL CGPSProc::StartThread()
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
    
    g_dwDataAnalyseThreadID = m_dwDataAnalyseThreadID;

    m_bThreadStart = TRUE;
    
    return TRUE;
}

void CGPSProc::StopThread()
{
    if( m_bThreadStart )
    {
        m_bThreadStart = FALSE;
        //终止系统工作线程
        PostThreadMessage(m_dwDataAnalyseThreadID, WM_THREAD_STOP, 0L, 0L);
        ::WaitForSingleObject(m_hDataAnalyseThreadEndEvt, INFINITE);
        m_dwDataAnalyseThreadID = 0;
        
        CloseHandle(m_hDataAnalyseThreadEndEvt);
        m_hDataAnalyseThreadEndEvt = NULL;
        
        m_hDataAnalyseThread = NULL;
    }
}

//设置日志
void CGPSProc::SetLog()
{
    Crwini rwIniTemp;
    
    CString tempstr =_T("pGisPubNetCfg.ini");
    
    if(rwIniTemp.CheckIniFile(tempstr))
    {
        //日志级别
        int nTempLevel;
        int nTempV;
        nTempLevel = rwIniTemp.GetInt(_T("SystemSet"),_T("LogLevel"),tempstr);
        
        nTempV = rwIniTemp.GetInt(_T("SystemSet"),_T("LogFileCleanupDate"),tempstr);

        gWriteLogApp.StartLog("GpsPublicNetLog",nTempLevel,nTempV);
    }
}

BOOL CGPSProc::LoadIOCPSystemConfig()
{
	Crwini rwIniTemp;

	CString tempstr =_T("pGisPubNetCfg.ini");

	if(!rwIniTemp.CheckIniFile(tempstr))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}

	if(0 == (m_dwMaxConnect = rwIniTemp.GetInt(_T("NetSet"),_T("MaxConnect"),tempstr)))
	{
		m_dwMaxConnect = 20000;
	}
	if(0 == (m_dwAcceptTime = rwIniTemp.GetInt(_T("NetSet"),_T("AcceptTime"),tempstr)))
	{
		m_dwAcceptTime = 10;
	}
	if(0 == (m_dwIdleOverTime = rwIniTemp.GetInt(_T("NetSet"),_T("IdleOverTime"),tempstr)))
	{
		m_dwIdleOverTime = 100;
	}
	if(0 == (m_dwHeartBeatTime = rwIniTemp.GetInt(_T("NetSet"),_T("HeartBeatTime"),tempstr)))
	{
		m_dwHeartBeatTime = 30;
	}
	if(0 == (m_dwTimeAutoConnect = rwIniTemp.GetInt(_T("NetSet"),_T("TimeAutoConnect"),tempstr)))
	{
		m_dwTimeAutoConnect = 5;
	}
	if(0 == (m_dwIODataCount = rwIniTemp.GetInt(_T("NetSet"),_T("IODataCount"),tempstr)))
	{
		m_dwIODataCount = 10;
	}
	if(0 == (m_dwIODataBuffer = rwIniTemp.GetInt(_T("NetSet"),_T("IODataBuffer"),tempstr)))
	{
		m_dwIODataBuffer = 1024;
	}
	if(0 == (m_nSocketBuffer = rwIniTemp.GetInt(_T("NetSet"),_T("SocketBuffer"),tempstr)))
	{
		m_nSocketBuffer = 8096;
	}	


	if(rwIniTemp.ReadString(_T("OldTer"),_T("LocalIP"),tempstr,m_szOldLocalIP) == 0) 
	{
		strncpy(m_szOldLocalIP,"",sizeof(m_szOldLocalIP));
	}
	if(0 == (m_nOldLocalPort = rwIniTemp.GetInt(_T("OldTer"),_T("LocalPort"),tempstr)))
	{
		m_nOldLocalPort = 9001;
	}
	if(rwIniTemp.ReadString(_T("OldTer"),_T("StartInfo"),tempstr,m_szOldStartInfo) == 0) 
	{
		strncpy(m_szOldStartInfo,_T("服务启动成功!"),sizeof(m_szOldStartInfo));
	}
	if(rwIniTemp.ReadString(_T("OldTer"),_T("ErrorInfo"),tempstr,m_szOldErrorInfo) == 0) 
	{
		strncpy(m_szOldErrorInfo,_T("服务启动失败!"),sizeof(m_szOldErrorInfo));
	}
	
	if(rwIniTemp.ReadString(_T("NewTer"),_T("LocalIP"),tempstr,m_szNewLocalIP) == 0) 
	{
		strncpy(m_szNewLocalIP,"",sizeof(m_szOldLocalIP));
	}
	if(0 == (m_nNewLocalPort = rwIniTemp.GetInt(_T("NewTer"),_T("LocalPort"),tempstr)))
	{
		m_nNewLocalPort = 9002;
	}
	if(rwIniTemp.ReadString(_T("NewTer"),_T("StartInfo"),tempstr,m_szNewStartInfo) == 0) 
	{
		strncpy(m_szNewStartInfo,_T("服务启动成功!"),sizeof(m_szNewStartInfo));
	}
	if(rwIniTemp.ReadString(_T("NewTer"),_T("ErrorInfo"),tempstr,m_szNewErrorInfo) == 0) 
	{
		strncpy(m_szNewErrorInfo,_T("服务启动失败!"),sizeof(m_szNewErrorInfo));
	}

	if(rwIniTemp.ReadString(_T("PubToCon"),_T("LocalIP"),tempstr,m_szPubToConLocalIP) == 0) 
	{
		strncpy(m_szPubToConLocalIP,"",sizeof(m_szPubToConLocalIP));
	}
	if(rwIniTemp.ReadString(_T("PubToCon"),_T("RemoteIP"),tempstr,m_szPubToConRemoteIP) == 0) 
	{
		strncpy(m_szPubToConRemoteIP,"",sizeof(m_szPubToConRemoteIP));
	}
	if(0 == (m_nPubToConPort = rwIniTemp.GetInt(_T("PubToCon"),_T("RemotePort"),tempstr)))
	{
		m_nPubToConPort = 9003;
	}
	if(rwIniTemp.ReadString(_T("PubToCon"),_T("StartInfo"),tempstr,m_szPubToConStartInfo) == 0) 
	{
		strncpy(m_szPubToConStartInfo,_T("服务启动成功!"),sizeof(m_szPubToConStartInfo));
	}
	if(rwIniTemp.ReadString(_T("PubToCon"),_T("ErrorInfo"),tempstr,m_szPubToConErrorInfo) == 0) 
	{
		strncpy(m_szPubToConErrorInfo,_T("服务启动失败!"),sizeof(m_szPubToConErrorInfo));	
	}
	if(0 == (m_nPubToConLocalPort = rwIniTemp.GetInt(_T("PubToCon"),_T("LocalPort"),tempstr)))
	{
		m_nPubToConLocalPort = 9004;
	}
	if(0 == (m_Udp = rwIniTemp.GetInt(_T("PubToCon"),_T("UDP"),tempstr)))
	{
		m_Udp = 0;
	}
	return TRUE;
}

//系统启动
BOOL CGPSProc::StartPubNetSystem()
{
    if(m_bStartFlag)
    {
        return TRUE;
    }

    InitMyVersion();
    //读取配置文件
    if(!LoadSystemConfig())
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]读取配置文件失败\n",CTime::GetCurrentTime().Format("%H:%M:%S"));      
        return FALSE;
    }

	if(!LoadIOCPSystemConfig())
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]读取配置文件失败\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}

	if(!StartThread())
	{
		return FALSE;
	}

	if (m_iocpsocket.OnStartIOCPSocket(m_dwMaxConnect,m_dwAcceptTime,m_dwIdleOverTime,m_dwHeartBeatTime,m_dwTimeAutoConnect,m_dwIODataBuffer,m_nSocketBuffer))
	{
		m_IsIOCPActive = TRUE;
	}
	
	if (m_IsIOCPActive && m_iocpsocket.GTcpSvr_OpenListen(m_dwBaseBusinessManage,m_szNewLocalIP,m_nNewLocalPort,0,(DWORD)&m_BaseBusinessManage,
		&CBusinessManage_NewTerminal::OnConnectSvr,
		&CBusinessManage_NewTerminal::OnReceiveSvr,
		&CBusinessManage_NewTerminal::OnSendedSvr,
		&CBusinessManage_NewTerminal::OnDisconnectSvr,
		&CBusinessManage_NewTerminal::OnSendErrorSvr,
		&CBusinessManage_NewTerminal::OnConnectionOverflow,
		&CBusinessManage_NewTerminal::OnIdleOvertime,
		&CBusinessManage_NewTerminal::OnUnknownError,
		&CBusinessManage_NewTerminal::OnCreateClientSvr,
		&CBusinessManage_NewTerminal::OnDestroyClientSvr))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szOldLocalIP,m_nOldLocalPort,m_szOldStartInfo);
// 		CString strInfo;
// 		strInfo.Format("%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szOldLocalIP,m_nOldLocalPort,m_szOldStartInfo);
// 		LOG(INFO)<<strInfo;
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szOldLocalIP,m_nOldLocalPort,m_szOldErrorInfo);   
	}


	if (m_IsIOCPActive && m_iocpsocket.GTcpSvr_OpenListen(m_dwOldTerminal_BusinessManage,m_szOldLocalIP,m_nOldLocalPort,0,(DWORD)&m_OldTerminal_BusinessManage,
		&CBusinessManage_OldTerminal::OnConnectSvr,
		&CBusinessManage_OldTerminal::OnReceiveSvr,
		&CBusinessManage_OldTerminal::OnSendedSvr,
		&CBusinessManage_OldTerminal::OnDisconnectSvr,
		&CBusinessManage_OldTerminal::OnSendErrorSvr,
		&CBusinessManage_OldTerminal::OnConnectionOverflow,
		&CBusinessManage_OldTerminal::OnIdleOvertime,
		&CBusinessManage_OldTerminal::OnUnknownError,
		&CBusinessManage_OldTerminal::OnCreateClientSvr,
		&CBusinessManage_OldTerminal::OnDestroyClientSvr))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szNewLocalIP,m_nNewLocalPort,m_szNewStartInfo);  
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szNewLocalIP,m_nNewLocalPort,m_szNewErrorInfo);  
	}
	
	if (m_Udp)
	{
		if (!m_MppPubToCon.Start(g_dwDataAnalyseThreadID,m_Udp,m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,m_nPubToConLocalPort))
		{
			m_MppPubToCon.Close();
			StopThread();
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] Model:%d RemoteIP:%s RemotePort:%d LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				m_Udp,m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,m_nPubToConLocalPort,m_szPubToConErrorInfo);  
			return FALSE;
		}
		else
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] Model:%d  RemoteIP:%s RemotePort:%d LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				m_Udp,m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,m_nPubToConLocalPort,m_szPubToConStartInfo); 
		}
	}
	else
	{
		if (m_IsIOCPActive && m_iocpsocket.GTcpSvr_Connect(m_dwPubToCon,m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,0,(DWORD)&m_PubToCon_BusinessManage,
			&CPubToCon_BusinessManage::OnConnectClt,
			&CPubToCon_BusinessManage::OnReceiveClt,
			&CPubToCon_BusinessManage::OnSendedClt,
			&CPubToCon_BusinessManage::OnSendErrorClt,
			&CPubToCon_BusinessManage::OnConnectError,
			&CPubToCon_BusinessManage::OnDisconnectClt,
			&CPubToCon_BusinessManage::OnHeartbeat,
			&CPubToCon_BusinessManage::OnCreateClientClt,
			&CPubToCon_BusinessManage::OnDestroyClientClt))
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,m_szPubToConStartInfo);  
		}
		else
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				m_szPubToConRemoteIP,m_nPubToConPort,m_szPubToConLocalIP,m_szPubToConErrorInfo);  
		}
	}

    m_bStartFlag = TRUE;
   
    return TRUE;
}

//系统停止
BOOL CGPSProc::StopPubNetSystem()
{
    if(m_bStartFlag)
    {
        gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS]公网接入服务器程序退出运行,共接收GPS数据:%d(条),向主控发送GPS数据:%d\n",
            CTime::GetCurrentTime().Format("%H:%M:%S"),gRecvGpsCount,gSendGpsCount);

		m_MppPubToCon.Close();
        StopThread();
   
        m_bStartFlag = FALSE;
    }

    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//数据分析线程
UINT WINAPI CGPSProc::DataAnalyseThreadProc(void *lpParameter)
{
    CGPSProc *pHytPublicSys = NULL;
    pHytPublicSys = (CGPSProc *)lpParameter;
    
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
                pHytPublicSys->m_MppPubToCon.OnReadMainFtpConveyFile();
				pHytPublicSys->m_OldTerminal_BusinessManage.OnCheck();
                 break;
			case WM_RECVICE_MESSAGE:
				pHytPublicSys->ProessAnalyseData((RECV_CLIENTDATA*)msg.wParam);
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
    
    SetEvent(pHytPublicSys->m_hDataAnalyseThreadEndEvt);
    TRACE(_T("Quit System thread\n"));
    _endthreadex(0);
    
    return 0;
}

void CGPSProc::ProessAnalyseData(RECV_CLIENTDATA *data)
{
	TRACE("[DATA_RECV] CMD:%d LEN:%d Message:%s SocketID:%d \n",data->nCmdType,data->length,data->MessagePacket,data->nSockID);
	switch(data->nAppID)
	{
	case SOCKET_PUBNETCLIENT_TYPEID:
		m_PubToCon_BusinessManage.ProessAnalyseData(m_dwPubToCon,data->MessagePacket,data->length,data->nCmdType);
		delete data;
		data = NULL;
		break;
	default:
		break;
	}
}
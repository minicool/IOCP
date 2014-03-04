
#if !defined(AFX_PGISMAINCTRLSERVER_H__D916395A_33B5_43C6_BCE5_4F6EE60E23D7__INCLUDED_)
#define AFX_PGISMAINCTRLSERVER_H__D916395A_33B5_43C6_BCE5_4F6EE60E23D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "GIOCPsocket.h"
#include "BusManage_ConnectToLocationServer.h"
#include "BusManage_ConnectToShanhaiServer.h"
#include "BusManage_ConnectToGPSCheckServer.h"
#include "BusManage_ConToPub.h"
#include "BusManage_GISTerminal.h"
#include "DBManage_WebServer.h"
#include "SubscriptionManager.h"

/////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

#pragma pack(pop)

//GPS数据类型
typedef enum enumGpsDataType
{
	GPS_DATA_PRIVATENET_LINEOFF,//00表示专网下线GPS
	GPS_DATA_PRIVATENET_LINEON,//01表示专网上线GPS
	GPS_DATA_TINMEROUT,//02表示固定上传GPS
	GPS_DATA_MONITOR,//03表示即时跟踪GPS
	GPS_DATA_BOUNDCALL,//04表示区域点名GPS
	GPS_DATA_MY_POSITION,//05表示请求"我的位置"的GPS
	GPS_DATA_MY_OFFDUTY,//06表示下班GPS
	GPS_DATA_MY_ONDUTY,//07表示上班GPS
	GPS_DATA_HISTORY,
}GPS_DATA_TYPE;

/////////////////////////////////////////////////////////////////////////////
//
#define MAX_LEN_TEXT_MSG 200

class CPgisMainSer
{
public:

    CPgisMainSer();
    ~CPgisMainSer();
    
    BOOL Start();
    void Stop();

	void InitMyVersion();
	BOOL LoadSystemConfig();
	void SetSysLogLevel();
public:
	BOOL GetUnCodeTextInfo(CString strPatroAlertMessage,char *pPatroAlertInfo);
	BOOL GetCpsxIdValue(const char *strValue);
	BOOL SysEncryptCheck();

public:
	char m_PatroAlertTextInfo[MAX_LEN_TEXT_MSG];

public:
	Crwini rwIniTemp;

private:
	int m_nL ;
	int m_nM ;
	int m_nS ;

	int m_nMaxGisConnCount;

public:
		CString g_strConfigFile;

		GIOCPsocket m_iocpsocket;

		DWORD m_dwMaxConnect;
		DWORD m_dwAcceptTime;
		DWORD m_dwIdleOverTime;
		DWORD m_dwHeartBeatTime;
		DWORD m_dwTimeAutoConnect;
		DWORD m_dwIODataCount;

		int	m_nSocketBuffer;
		char m_szStartInfo[512];
		char m_szErrorInfo[512];
		char m_szLoadErrorInfo[512];

		CBusinessManage_ConnectToLocationServer m_BusinessManage_ConnectToLocationServer;
		DWORD g_dwLocationServerContext;

		CBusinessManage_ConnectToGPSCheckServer m_BusinessManage_ConnectToGPSCheckServer;
		DWORD g_dwGPSCheckServerContext;

// 		CBusinessManage_ConnectToShanhaiServer m_BusinessManage_ConnectToShanhaiServer;
// 		DWORD g_dwShanhaiServerContext;

		std::vector<CBusinessManage_ConnectToShanhaiServer*> m_BusinessManage_ConnectToShanhaiServer_vec;

		CBusinessManage_ConToPub m_BusinessManage_ConToPub;
		DWORD g_dwConToPubContext;

		CBusinessManage_GISTerminal m_BusinessManage_GISTerminal;
		DWORD g_dwGISTerminalContext;

		CDBManage_WebServer	m_DBManage_WebServer;

		BOOL m_bSysStartFlag;
		int m_nLogLevel;
		int m_nShanhaiCount;

		SubscriptionManager m_SubscriptionManager;
};

#endif // !defined(AFX_PGISMAINCTRLSERVER_H__D916395A_33B5_43C6_BCE5_4F6EE60E23D7__INCLUDED_)

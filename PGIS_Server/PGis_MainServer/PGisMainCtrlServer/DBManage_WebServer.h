#pragma once
#include "../../IOCP/GThread.h"
#include "../../IOCP/GSocket.h"
#include "MemoryPool.h"

using namespace LocationData;

class CDBManage_WebServer
{
public:
	CDBManage_WebServer(void);
public:
	~CDBManage_WebServer(void);

	BOOL StartThread();
	void StopThread();

	BOOL StartWebService();
	void FreeWebService();
	BOOL LoadConfig();

	BOOL OnWebServiceCmd();

public:
	BOOL WebService_QueryDB_PubTerminalInfo(CString TerminalNum,int &nUserType,CString &strUserCode,CString &strDeptNum);
	BOOL WebService_UpdataDB_PubTerminalInfo_DeviceOnLineStatus(CString strDeviceCode,CString strState);
	BOOL WebService_WriteDB_AlarmMessage(const int nMsgID,const char * strDeviceCode);
	BOOL WebService_UpDataDB_Location(int nIndex,const char *strDevice,const char *strLocation);

	BOOL WebService_WriteDB_GpsData(const DWORD dwClientContext,const char *pRecvData,int nLen);
	BOOL WebService_WriteDB_GpsData(CString strDeviceID,const int nDeviceType,const char *pGpsData,int nLen);

	BOOL WebServer_LoadDB_PatrolDeviceInfo(BOOL isUpdateOnLineDeviceList);

	void ReportWebServiceDeviceOnLine(const char *strDrivceID);
	void ReportWebSendMessage(int MsgID,const char * strDeviceCode);
	void ReportWebServiceTerminalLine(const char *strDrivceID,int nState);
	void ReportWebServer_UpDataLocation(int nQueryIndex,char *strDeviceCode ,char *strLocation);
	void ReportWebServiceRecvGps(const DWORD dwClientContext,const char *pRecvData,int nLen);

	static UINT WINAPI WebServiceThreadProc(void *lpParameter);
	void StartWaitResult();
private:
	CString m_strWebServiceURL;
	BOOL	m_bWebServiceConn; //WEB ServiceÁ¬½Ó×´Ì¬
	BOOL	m_bWebLoadPatrokOk;

	GTHREAD	GTcpSvrServiceThreadData;

	BOOL	m_bThreadStart;
	UINT	g_dwWebThreadID;
	HANDLE	m_hWaitResultEvt;
	HANDLE	m_hWebThreadEndEvt;
	HANDLE	m_hWebThread;
	UINT	m_dwWebThreadID;

	CMemoryPool gWebServiceList;

	CLocationData *m_pWebService;

public:
	BOOL	m_bWebConnOK;
};

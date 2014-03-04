#pragma once

#include "../../IOCP/HashmapManager.h"
#include "../../IOCP/ListManager.h"
#include <string>

class CBusinessManage_ConnectToGPSCheckServer
{
public:
	CBusinessManage_ConnectToGPSCheckServer(void);
public:
	~CBusinessManage_ConnectToGPSCheckServer(void);

public:
	BOOL	Start();
	DWORD	GetContext();

private:
	BOOL	LoadConfig();

private:
	static void OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned);
	static void OnDisconnectClt(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnReceiveClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnReceiveErrorClt(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnHeartbeat(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnConnectError(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnCreateClientClt(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnDestroyClientClt(const DWORD dwClientContext, const DWORD dwInstaned);

private:
	BOOL	SendData(const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, const int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);

	void	SendPatroAlertToPubServer(char *pszDeviceCode,const char* pszPatrAlert);

	BOOL	EventHeartBeat(const DWORD dwClientContext);
	BOOL	EventLogin(const DWORD dwClientContext);

	BOOL	ReportWebServer_LoadPatrolDeviceData();

public:
	BOOL	SendUpGPSDataToGPSCheckServer(const char *szDeviceCode,const char *szLongitude,const char *szLatitude);
	BOOL	SendSnycDataToGPSCheckServer(const char *SnycData,const int nDataLen);

private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

public:
	typedef CListManager<CString> ListDataPatrolDev;
	ListDataPatrolDev m_PatrolDevListData;

	BOOL	FindDevicePatrol(const char *szDeviceNum);

private:
	DWORD m_dwGPSCheckServerContext;

	BOOL m_IsSuceeseStart;
	BOOL m_IsConnLocationServe;
	char m_szLocalIP[32];
	int m_nConRemotePort;
	char m_szConRemoteIP[32];
	char m_szStartInfo[512];
	char m_szErrorInfo[512];

public:
	BOOL m_bConnGpsCheckServer;
};

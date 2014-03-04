#pragma once

#include "../../IOCP/HashmapManager.h"
#include "../../IOCP/ListManager.h"

class CBusinessManage_ConnectToLocationServer
{
public:
	CBusinessManage_ConnectToLocationServer(void);
public:
	~CBusinessManage_ConnectToLocationServer(void);

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
	BOOL	GSendData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen,const int nCmdType);
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, const int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);

private:
	BOOL	EventHeartBeat(const DWORD dwClientContext);
	BOOL	EventLogin(const DWORD dwClientContext);

public:
	BOOL	SendUpDataLocation(const DWORD dwClientContext,const DWORD nQIndex,const char *pszDeviceCode,const char *pszLocation);
	BOOL	QueryLocationProcess(const DWORD dwClientContext_Search,const char *RecvData,int nLen);
	BOOL	QueryLocationProcess(const DWORD dwClientContext_Search,const char *szDeviceCode,const char *szLongitude,const char *szLatitude);

private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

private:
	typedef CListManager<DWORD>  ListDataReturindex;
	ListDataReturindex m_returnIndexDataList;

private:
	DWORD m_dwLocationServerContext;

	BOOL m_IsSuceeseStart;
	BOOL m_IsConnLocationServe;
	char m_szLocalIP[32];
	int m_nConRemotePort;
	char m_szConRemoteIP[32];
	char m_szStartInfo[512];
	char m_szErrorInfo[512];

public:
	BOOL m_bConnLocationServer;
};

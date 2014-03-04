#pragma once

#include "../../IOCP/HashmapManager.h"

class CBusinessManage_ConnectToShanhaiServer
{
public:
	CBusinessManage_ConnectToShanhaiServer(void);
public:
	~CBusinessManage_ConnectToShanhaiServer(void);

public:
	BOOL	Start(int nCount);
	DWORD	GetContext();

private:
	BOOL	LoadConfig(int nCount);

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
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, unsigned short us_CmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
//	BOOL	SendDataToShanHai(const DWORD dwClientContext,int nType,const char *DataBuf,const DWORD dwBufLen);	//OLD

private:
	BOOL	EventHeartBeat(const DWORD dwClientContext);
	BOOL	EventLogin(const DWORD dwClientContext);

public:
	BOOL	SendGPSData(const char *DataBuf,const DWORD dwBufLen);

private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

private:
	DWORD m_dwShanhaiServerContext;

	BOOL m_IsSuceeseStart;
	BOOL m_IsConnLocationServe;
	char m_szLocalIP[32];
	int m_nConRemotePort;
	char m_szConRemoteIP[32];
	char m_szStartInfo[512];
	char m_szErrorInfo[512];

	char m_strSHLoginID[10];
	char m_strSHLoginPSWD[50];

	int	m_nIndexServer;
public:
	BOOL m_bConnShanhaiServer;
	char m_szSubscriptionID[256];
	char m_szSubscription[1024];
};

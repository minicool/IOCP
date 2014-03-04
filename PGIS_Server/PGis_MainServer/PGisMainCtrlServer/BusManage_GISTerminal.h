#pragma once

#include <string>
#include "../../IOCP/GSocket.h"	//bad
#include "../../IOCP/HashmapManager.h"

class CBusinessManage_GISTerminal
{
public:
	CBusinessManage_GISTerminal(void);
public:
	~CBusinessManage_GISTerminal(void);

public:
	BOOL	Start();
	DWORD	GetContext();

private:
	BOOL	LoadConfig();

private:
	static void OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned);
	static void OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnReceiveSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned);

public:
	static BOOL	GisTer_Context_DeptNum(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext);
	static BOOL	GisTer_All(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext);

	static BOOL	GISTer_DeptNum_Char(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext, const char *szchar);
	static BOOL GISTer_Subscription(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext, const char *szchar);

	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	void	SendData_Broadcast(char* pszSendData, const DWORD dwSendLen,const int nCmdType,const DWORD dwClientContext,PFN_ON_GSOCK_TRAVERSAL pfntravrsal);
	void	SendData_Broadcast_char(const char* pBuf,const DWORD dwBytes,const int nCmdType,const DWORD dwClientContext,char *szchar,PFN_ON_GSOCK_FINDDATA_CHAR pfntravrsal);

private:
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType);

	BOOL	CheckGisClientLoginProcess(const DWORD dwClientContext, const char *data);
	BOOL	EventHeartBeat_DispatchAckToAnswer(DWORD dwClientContext);
	void	EventLogin_DispatchAckToAnswer(const DWORD dwClientContext);

	BOOL	CheckGISTerminal(const char *pszLoginNum);
	void	AddGISTerminal(const DWORD dwClientContext,const char *pszLoginNum,const char *pszDeptNum,char *pszMac);

	void	GisClientLoginSuccessProcess(const DWORD dwClientContext);
	static	BOOL GisClientLoginSuccessProcess(const DWORD dwInstance,const DWORD dwClientContext,const DWORD dwDataContext);

	BOOL	GisClientSystemSnycProcess(const DWORD dwClientContext, const char *SnycData, int nDataLen);
	BOOL	ResetLoginTerminalBindInfo(const char *strDeviceCode);

public:
	void	ShowLoginGisClientInfo(void);
	void	ShowLoginGisNum(void);

private:
	static	BOOL GetOnLineGisClientInfo(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext);

private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

public:
	typedef struct _DATA_GISTERMINAL
	{
		char szLoginName[64];
		char szDeptNum[64];
		char szLoginMac[64];
	}DATA_GISTERMINAL,*PDATA_GISTERMINAL;	

	typedef CHashmapManager<DWORD,DATA_GISTERMINAL>  HashmapDataGISTerminal;
	HashmapDataGISTerminal m_HashmapDataGISTerminal;

	typedef CHashmapManager<string,DWORD>  HashmapDataGISLogin;
	HashmapDataGISLogin m_HashmapDataGISLogin;

private:
	DWORD m_dwGisServerContext;

	BOOL m_IsSuceeseStart;
	char m_szLocalIP[32];
	int m_nLocalPort;
	char m_szStartInfo[512];
	char m_szErrorInfo[512];
};

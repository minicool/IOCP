#pragma once

#include "../../IOCP/MapManager.h"
#include "../../IOCP/HashmapManager.h"
#include "../../IOCP/GLog.h"
#include "../../IOCP/GMemory.h"
#include "../../IOCP/GSocket.h"

typedef	void(*PFN_ON_EVENT_BUSINESS)(const DWORD dwClientContext,const DWORD dwInstaned);
typedef	void(*PFN_ON_DATA_EVENT_BUSINESS)(const DWORD dwClientContext, const char *pBuf, const DWORD Bytes,const DWORD dwInstaned);

typedef struct _GPFNDATA_SVR
{
	DWORD							dwInstaned;
	PFN_ON_DATA_EVENT_BUSINESS  pfnOnGSockConnectTcpSvr;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockReceiveSvr;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendedSvr;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendErrorSvr;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockDisconnectSvr;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockConnectionOverflow;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockIdleOvertime;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockCreateClientSvr;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockDestroyClientSvr;
}GPFNDATA_SVR, *PGPFNDATA_SVR;

typedef struct _GPFNDATA_CLT
{
	DWORD							dwInstaned;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockConnectClt;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockReceiveClt;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendedClt;
	PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendErrorClt;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockConnectError;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockDisconnectClt;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockHeartbeat;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockCreateClientClt;
	PFN_ON_EVENT_BUSINESS		pfnOnGSockDestroyClientClt;
}GPFNDATA_CLT, *PGPFNDATA_CLT;

class GIOCPsocket
{
public:
	GIOCPsocket(void);
public:
	~GIOCPsocket(void);


public:
	BOOL Start();
	void Stop();
	BOOL LoadConfig();

private:
	BOOL OnInitSocket();
	void OnUninitSocket();
	void GetDefualt_GSocketInfo(void);

	static void OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes);
	static void OnDisconnectSvr(const DWORD dwClientContext);
	static void OnReceiveSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnReceiveErrorSvr(const DWORD dwClientContext);
	static void OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnIdleOvertimeSvr(const DWORD dwClientContext);
	static void OnConnectionOverflowSvr(const DWORD dwClientContext);
	static void OnCreateClientSvr(const DWORD dwClientContext);
	static void OnDestroyClientSvr(const DWORD dwClientContext);

	static void OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes);
	static void OnDisconnectClt(const DWORD dwClientContext);
	static void OnReceiveClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnReceiveErrorClt(const DWORD dwClientContext);
	static void OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes);
	static void OnHeartbeatClt(const DWORD dwClientContext);
	static void OnConnectErrorClt(const DWORD dwClientContext);
	static void OnCreateClientClt(const DWORD dwClientContext);
	static void OnDestroyClientClt(const DWORD dwClientContext);

	BOOL ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	BOOL ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);

	BOOL OnStartIOCPSocket(	DWORD dwMaxConnection_SVR,		//最大连接量，默认仅需修改最大连接量即可
							DWORD dwAcceptOvertime_SVR,		//数据接受超时，ACCEPT后，必须第一次数据向服务器发送数据，否则被断开	
							DWORD dwIdleOvertime_SVR,		//接受超时，客户端连接后，必须在这个时长内向服务器发送数据，否则被断开
							DWORD dwHeartbeat_CLR,			//客户端发送心跳包
							DWORD dwTimeAutoConnect_CLR,	//客户端自动重连
							DWORD dwIoDataBufSize_SOCKET,	//数据Buffer
							int	  nSocketBufSize_SOCKET);	//socket buffer 设置

public:
	BOOL GTcpSvr_OpenListen(DWORD &dwClientContext, CString strListenIp,DWORD dwListenPort,DWORD dwProtocol,DWORD dwInstaned,
										 PFN_ON_DATA_EVENT_BUSINESS pfnOnGSockConnectTcpSvr,
										 PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockReceiveSvr,
										 PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendedSvr,
										 PFN_ON_EVENT_BUSINESS		pfnOnGSockDisconnectSvr,
										 PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendErrorSvr,
										 PFN_ON_EVENT_BUSINESS		pfnOnGSockConnectionOverflow,
										 PFN_ON_EVENT_BUSINESS		pfnOnGSockIdleOvertime,
										 PFN_ON_EVENT_BUSINESS		pfnOnGSockCreateClientSvr,
										 PFN_ON_EVENT_BUSINESS		pfnOnGSockDestroyClientSvr);

	BOOL GTcpSvr_Connect(DWORD &dwClientContext, CString strRemoteIp,DWORD dwRemotePort,CString strLocalIp,DWORD dwProtocol,DWORD dwInstaned,
									  PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockConnectClt,
									  PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockReceiveClt,
									  PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendedClt,
									  PFN_ON_DATA_EVENT_BUSINESS	pfnOnGSockSendErrorClt,
									  PFN_ON_EVENT_BUSINESS			pfnOnGSockConnectError,
									  PFN_ON_EVENT_BUSINESS			pfnOnGSockDisconnectClt,
									  PFN_ON_EVENT_BUSINESS			pfnOnGSockHeartbeat,
									  PFN_ON_EVENT_BUSINESS			pfnOnGSockCreateClientClt,
									  PFN_ON_EVENT_BUSINESS			pfnOnGSockDestroyClientClt);


public:
	BOOL m_IsIOCPActive;

private:
	DWORD m_dwMaxConnect;
	DWORD m_dwAcceptTime;
	DWORD m_dwIdleOverTime;
//	DWORD m_dwHeartBeatTime;
//	DWORD m_dwTimeAutoConnect;
	DWORD m_dwIODataCount;

	int	m_nSocketBuffer;
	char m_szStartInfo[512];
	char m_szErrorInfo[512];

	DWORD m_dwGMemPageSize;					//内存页大小
	DWORD m_dwGMemTotal;					//内存大小
	DWORD m_dwGBufSize;						//获取传输BUF大小
	DWORD m_dwIoDataTotal;					//IODATA大小
	DWORD m_dwHndDataTotal;					//HNDDATA大小
	DWORD m_dwPostRecvNumber;
	int m_nSocketBufSize;
	DWORD m_dwWorkerThreadNumber;
	DWORD m_dwConcurrentThreadNumber;
	DWORD m_dwAcceptOvertime;
	DWORD m_dwIdleOvertime;
	DWORD m_dwMaxConnection;
	DWORD m_dwPostAcceptNumber;

	DWORD m_dwPackSize;

	DWORD m_dwHeartBeatTime;
	DWORD m_dwTimeAutoConnect;

	CString m_strLocalIp;
	DWORD m_dwLocalPort;
	CString m_strRemoteIp;
	DWORD m_dwRemotePort;
	DWORD m_dwSendBytes;
	DWORD m_dwRecvBytes;
	DWORD m_dwByteTotal;
	DWORD m_dwLong;

	float m_fBeat;
	BOOL m_bCloseMtu;
	BOOL m_bZeroAccept;
	BOOL m_bZeroRecv;
	BOOL m_bSendSelf;
	BOOL m_bRandoDisconnect;
	BOOL m_bShutdownSend;

	typedef	CHashmapManager<DWORD,GPFNDATA_SVR> GPfnData_SVR_HashManager;
	GPfnData_SVR_HashManager m_GPfnData_SVR_HashManager;

	typedef	CHashmapManager<DWORD,GPFNDATA_CLT> GPfnData_CLT_HashManager;
	GPfnData_CLT_HashManager m_GPfnData_CLT_HashManager;
};

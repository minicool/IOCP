#pragma once

#include "../../IOCP/GSocket.h"
#include "../../IOCP/HashmapManager.h"
#include <string>

class CBusinessManage_NewTerminal
{
public:
	CBusinessManage_NewTerminal(void);
	virtual ~CBusinessManage_NewTerminal(void);

public:
	static void OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned);
	static void OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnReceiveSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnUnknownError(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned);

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

public:
	void	ShowLoginTerNum(void);

public:
	typedef struct _DATA_INFO
	{
		BOOL	Login;
		char	strDeviceCode[64];
		char	strDeviceType[64];
	}DATA_INFO,*PDATA_INFO;

	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;

	typedef CHashmapManager<DWORD,DATA_INFO> HashmapDatainfo;
	HashmapDatainfo m_HashmapDataInfo;

	typedef CHashmapManager<string,DWORD>  HashmapDevID;
	HashmapDevID m_HashmapDevid;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

	// 	typedef CHashmapManager<DWORD,DWORD>  HashmapDataMessageID;
	// 	HashmapDataMessageID m_HashmapMessageID;
	// 	DWORD g_dwMessageID;
	// 	DWORD GetMessageID();

private:
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	DWORD	TerminalLoginProccess( const DWORD dwClientContext, const char* strDeviceCode, const char* strDeviceType);
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType);

	BOOL GetDataInfo(const DWORD dwClientContext,const DWORD dwInstaned,DATA_INFO* const pDatainfo) const;
};

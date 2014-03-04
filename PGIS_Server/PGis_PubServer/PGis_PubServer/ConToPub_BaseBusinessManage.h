#ifndef __PUBNET_TO_CONTROL_H_INCLUDED_
#define __PUBNET_TO_CONTROL_H_INCLUDED_

#pragma once

#include "../../IOCP/GSocket.h"
#include "../../IOCP/HashmapManager.h"
#include <string>

class CPubToCon_BusinessManage
{
public:
	CPubToCon_BusinessManage(void);
public:
	virtual ~CPubToCon_BusinessManage(void);

public:
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
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
private:
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	DWORD	TerminalLoginProccess( const DWORD dwClientContext, const char* strDeviceCode, const char* strDeviceType);

	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType);
	static BOOL	OnTraversalClient(CPubToCon_BusinessManage* pSockInfo, const DWORD nIndex, const DWORD dwClientContext);
	void	MainStartSuccessResponse();

	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;
};

#endif

/*
**	FILENAME			TcpMainServer.h
**
*/
#ifndef __TCPMAIN_SERVER_H_
#define __TCPMAIN_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tcp_SelectSocket.h"

const int MAX_RECVICE_SERVER_BUF = 1024;
const int MAX_IPADDRESS_LEN = 32;

class CTcpMainServer
{
public:
    CTcpMainServer();
    ~CTcpMainServer();
	
	static DWORD WINAPI TcpClientCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance);

	BOOL Start(const char *strIPAddress,int nPort,int AppID);
	void Stop();

	void TcpDisConnect();
	void CheckTimerOver();
	void TcpServerDataProcess(const char *recvData, int nRecvLen);
	BOOL AnalyseRecvTcpServerData(const char *recvData ,int nRecvLen);

	int SendData(const char *sendBuf,int nSendLen);

	inline BOOL GetStartFlag(){return m_bStartOk;};

private:
	CTcpSelectSocket m_ConnSockServer;

	char m_RecvBuf[MAX_RECVICE_SERVER_BUF];
	int m_nRecvBufLen;

	int m_nAppID;

	BOOL m_bStartOk;
	char m_strServerIP[MAX_IPADDRESS_LEN];
	int m_nGpsServerPort;
	int m_nCheckTimeValue;
	int m_nTickCount;

	CRITICAL_SECTION  m_SendCriSec;   //该缓冲所需临界区
};

#endif
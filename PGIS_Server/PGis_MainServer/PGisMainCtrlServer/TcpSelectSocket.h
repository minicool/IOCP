// TcpSelectSocket.h: interface for the CTcpSelectSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TCPSELECTSOCKET_H_)
#define _TCPSELECTSOCKET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TcpInterface.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//
typedef DWORD (WINAPI * LPFTCPCLIENTCALLBACK)(UINT uMsg,DWORD dwParam,DWORD dwInstance);

const int TCPCLIENT_RECVDATALEN = 512;
const int TCPCLIENT_NONBLOCKING = 1;
const int TCPCLIENT_BLOCKING = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagThreadClass
{
	HANDLE hHandle;//线程句柄
	DWORD dwAddr;//线程地址
	BOOL bRun;//是否运行
}ThreadClass;

typedef struct tagTcpClientLink
{
	SOCKET sock;
	struct sockaddr_in Addr;
	char szData[TCPCLIENT_RECVDATALEN];
	int nDataLen;
	fd_set fd;
	TIMEVAL to;//time out
}TcpClientLink;


///////////////////////////////////////////////////////////////////////////////////////////////
//
class CTcpSelectSocket  
{
public:
	CTcpSelectSocket();
	virtual ~CTcpSelectSocket();

public:
	void Close();
	int Send(const char *pszData,int nDataLen);

	int Connect(const char *pszIP,
				    int nPort,
				    LPFTCPCLIENTCALLBACK lpfTcpClientCallBack,
				    DWORD dwInstance = 0,
				    int nTimeOut = 1);

protected:
	static UINT WINAPI RecvThreadDispatch(LPVOID pParam);
	int ConnectDetect(int nTimeOut);
	void DisConnect();
	void RecvThreadWork();
	int CreateRecvThread();
	void TcpClientCallBack(UINT uMsg,char *pszData,int nDataLen);
	
private:

	LPFTCPCLIENTCALLBACK m_lpfTcpClientCallBack;
    
    BOOL m_bRecvThreadRun ;
    UINT m_dwRecvThreadID;
    HANDLE m_hRecvThread;
    HANDLE m_hRecvThreadEndEvt;

	DWORD m_dwUserInstance;
	TcpClientLink m_TcpClient;

	BOOL m_bIsConnect;
};

#endif // !defined(_TCPSELECTSOCKET_H_)
































































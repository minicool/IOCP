// TcpSelectSocket.cpp: implementation of the CTcpSelectSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TcpSelectSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpSelectSocket::CTcpSelectSocket()
{
	m_lpfTcpClientCallBack = NULL;
	m_dwUserInstance = NULL;
	m_TcpClient.sock = INVALID_SOCKET;
    m_hRecvThread = NULL;
    m_hRecvThreadEndEvt = NULL;
	m_TcpClient.to.tv_sec = 0;
	m_TcpClient.to.tv_usec = 10;

	memset(m_TcpClient.szData,0,sizeof(m_TcpClient.szData));
	m_TcpClient.nDataLen = 0;
	
	m_bIsConnect = FALSE;
}

CTcpSelectSocket::~CTcpSelectSocket()
{
	Close();
}

int CTcpSelectSocket::Connect(const char *pszIP, 
							  int nPort, 
							  LPFTCPCLIENTCALLBACK lpfTcpClientCallBack, 
							  DWORD dwInstance,
							  int nTimeOut)
{
	int nRet;

	//Close();
	if (nTimeOut <= 0 || nTimeOut > 30)
	{
		TRACE("TcpClient TimeOut Error!!\n");
		return TCPCLIENT_ERROR_TIMEOUT;
	}

	if (nPort < 0 || nPort > 65535)
	{
		TRACE("TcpClient Port Error!!\n");
		return TCPCLIENT_ERROR_PORTEXCEEDRANGE;
	}

	m_TcpClient.sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (m_TcpClient.sock == INVALID_SOCKET)
	{
		TRACE("TcpClient Socket create fail!!\n");
		return TCPCLIENT_ERROR_CREATESOCK;
	}
	
	m_TcpClient.Addr.sin_port = htons(nPort);
	m_TcpClient.Addr.sin_family = AF_INET;
	m_TcpClient.Addr.sin_addr.s_addr = inet_addr(pszIP);

	nRet = ConnectDetect(nTimeOut);
	if(nRet != TCPCLIENT_CONNECTSUCCESS)
	{
		closesocket(m_TcpClient.sock);
		m_TcpClient.sock = INVALID_SOCKET;
		TRACE("TcpClient Connect Error!!\n");
		return nRet;
	}
	m_bIsConnect = TRUE;
	

	nRet = CreateRecvThread();
	if (nRet != TCPCLIENT_CREATERECVTHREADSUCCESS)
	{
		closesocket(m_TcpClient.sock);
		m_TcpClient.sock = INVALID_SOCKET;
		return nRet;
	}
	m_lpfTcpClientCallBack = lpfTcpClientCallBack;
	m_dwUserInstance = dwInstance;

	return TCPCLIENT_CONNECTSUCCESS;
}

/******************************************************************************
Function:    	Close;
Description：	关闭所有连接。
Parameter:		无;
Return			无;
*******************************************************************************/
void CTcpSelectSocket::Close()
{
	if (m_bRecvThreadRun) 
	{
		m_bRecvThreadRun = FALSE;
        if(WaitForSingleObject(m_hRecvThreadEndEvt,1000) == WAIT_TIMEOUT) //等待线程结束
        {
            TerminateThread(m_hRecvThread,0);
            TRACE(_T("Recv线程强制终止 \n"));
        }

        CloseHandle(m_hRecvThreadEndEvt);
        m_hRecvThreadEndEvt = NULL;
        CloseHandle(m_hRecvThread);
        m_hRecvThread = NULL;

	}
	DisConnect();
	
	m_lpfTcpClientCallBack = NULL;
	m_dwUserInstance = NULL;
}

/******************************************************************************
Function:    	Send;
Description：	发送数据。
Parameter:		const char *pszData:要发送的数据；
				const int nDataLen:数据长度
Return			TCPCLIENT_SENDSUCCESS:成功
				错误参考TcpClientInterface.h中TCPCLIENT_ERROR定义
*******************************************************************************/
int CTcpSelectSocket::Send(const char *pszData, int nDataLen)
{
	if (m_TcpClient.sock == INVALID_SOCKET)
	{
		return TCPCLIENT_ERROR_NOTRIGHTSOCK;
	}
	
	if ((pszData == NULL) || (nDataLen <= 0))
	{
		return TCPCLIENT_ERROR_SENDDATA;
	}
	
	int nSendResult;
	nSendResult = send(m_TcpClient.sock,pszData,nDataLen,0);
	if (nSendResult == SOCKET_ERROR)
	{
		return TCPCLIENT_ERROR_SEND;
	}
	return TCPCLIENT_SENDSUCCESS;
}

/******************************************************************************
Function:    	DisConnect;
Description：	除接收线程不关闭外 初使化所有信息,通过Connect可以正常重连。
Parameter:		无；
Return			无;
*******************************************************************************/
void CTcpSelectSocket::DisConnect()
{
	if (m_TcpClient.sock != INVALID_SOCKET)
	{
        shutdown(m_TcpClient.sock, SD_BOTH); //这将强迫所有的传输和接收失败

		closesocket(m_TcpClient.sock);
		m_TcpClient.sock = INVALID_SOCKET;
	}
	
	m_bIsConnect = FALSE;

}

/******************************************************************************
Function:    	CreateRecvThread;
Description：	创建数据接收线程；
Parameter：		无;
Return:			TCPSERVER_CREATERECVTHREADSUCCESS:Recv 线程创建成功;
TCPSERVER_ERROR_CREATERECVTHREADFAIL:Accepte 线程创建失败;
*******************************************************************************/
int CTcpSelectSocket::CreateRecvThread()
{
    m_bRecvThreadRun = TRUE;

    if(m_hRecvThreadEndEvt == 0)
    {
        m_hRecvThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);

    }

    m_hRecvThread = (HANDLE)_beginthreadex( NULL,0,&RecvThreadDispatch,this, 0,&m_dwRecvThreadID);
    if(!m_hRecvThread)
    {
        m_bRecvThreadRun = FALSE;
        TRACE("TcpClient RecvThread Create Fail!!\n");
        return TCPCLIENT_ERROR_CREATERECVTHREADFAIL;
    }

    
	TRACE("TcpClient RecvThread Create succeed!!\n");
	
	return TCPCLIENT_CREATERECVTHREADSUCCESS;
}

/******************************************************************************
Function:    	RecvThreadDispatch;
Description：	将Recv分配到相关函数进行处理；
Parameter：		LPVOID pParam:传入this指针;
Return:			无意义;
*******************************************************************************/
UINT WINAPI CTcpSelectSocket::RecvThreadDispatch(LPVOID pParam)
{
	if(pParam != NULL)//因为参数不会为空，所以不做处理
	{
		CTcpSelectSocket * pTcpSelectSocket = (CTcpSelectSocket *)pParam;
		pTcpSelectSocket ->RecvThreadWork();
	}
	return 0;
}

//接收线程函数
void CTcpSelectSocket::RecvThreadWork()
{
	int nRet;
    ResetEvent(m_hRecvThreadEndEvt);
	while(m_bRecvThreadRun) 
	{
		if(m_bIsConnect == TRUE)
		{
			FD_ZERO(&m_TcpClient.fd);
			FD_SET(m_TcpClient.sock,&m_TcpClient.fd);
			
			nRet = select(0,&m_TcpClient.fd,NULL,NULL,&m_TcpClient.to);
			if (nRet == SOCKET_ERROR)
			{
				DisConnect();
				TcpClientCallBack(TCPCLIENT_EVENT_DISCONNECT,NULL,0);
			}
			if (nRet > 0)
			{
				if (FD_ISSET(m_TcpClient.sock,&m_TcpClient.fd))
				{
					memset(m_TcpClient.szData,0,sizeof(m_TcpClient.szData));
					nRet = recv(m_TcpClient.sock,m_TcpClient.szData,sizeof(m_TcpClient.szData),0);
					//nRet = SOCKET_ERROR:说明网线断，对方不正常退出
					//nRet == 0:说明对方正常closesocket退出
					if (nRet == SOCKET_ERROR || nRet == 0)
					{
						DisConnect();
						TcpClientCallBack(TCPCLIENT_EVENT_DISCONNECT,NULL,0);
					}
					else
					{
						TcpClientCallBack(TCPCLIENT_EVENT_RECVDATA,m_TcpClient.szData,nRet);
						//注意下面开关可以看到输出结果，但Trace最大输出为512个字符，当接收太
						//大时报错正常
						//TRACE("TcpClient Recv data:%s,Datalen is:%d!!\n",m_TcpClient.szData,nRet);
					}
					Sleep(10);//当数据太快时有用
				}
			}
			
		}
		else
		{
			Sleep(10);
		}
	}
	
    SetEvent(m_hRecvThreadEndEvt);
}

/******************************************************************************
Function:    	TcpClientCallBack;
Description：	调用回调函数；
Parameter：		const UINT uMsg:回调消息
				const char *pszData:接收到的数据
				const int nDataLen:接收到数据的长度
Return:			无;
*******************************************************************************/
void CTcpSelectSocket::TcpClientCallBack(UINT uMsg,
										 char *pszData,int nDataLen)
{
	TcpConnect TcpConnectInfo;

	//char szData[TCPCLIENT_RECVDATALEN];
	//没必要做一次内存COPY kevin
	//if (pszData != NULL)
	//{
	//	memset(szData,0,sizeof(szData));
	//	memcpy(szData,pszData,nDataLen);
	//	TcpConnectInfo.pszRecvData = szData;
	//}
	//else
	//{
	//	TcpConnectInfo.pszRecvData = NULL;
	//}

	if(pszData != NULL)
	{
		TcpConnectInfo.pszRecvData = pszData;
		TcpConnectInfo.nRecvDataLen = nDataLen;
	}
	else
	{
		TcpConnectInfo.pszRecvData = NULL;
		TcpConnectInfo.nRecvDataLen = 0;
	}

	if (m_lpfTcpClientCallBack != NULL)
	{
		m_lpfTcpClientCallBack(uMsg,(DWORD)&TcpConnectInfo,m_dwUserInstance);
	}
}

/******************************************************************************
//Function:    	ConnectDetect;
//Description：	连接服务器检测,为防止connect函数出现阻塞，通过设置成为非阻塞模式
//				,进行connect连接，再用select进行检测，由于select函数可以自由设定
//				超时时间，因此可控制connect超时时间；
//Parameter：		const int nTimeOut:超时时间;
//Return:			TCPCLIENT_CONNECTSUCCESS:连接成功;
//				其它错误参考TCPCLIENT_ERROR;
*******************************************************************************/
int CTcpSelectSocket::ConnectDetect(int nTimeOut)
{
	int nRet;
	int nConnectRet;
	unsigned long ulLock;
	
	fd_set fd;
	TIMEVAL to;


	ulLock = TCPCLIENT_NONBLOCKING;
	nRet = ioctlsocket(m_TcpClient.sock,FIONBIO,&ulLock);
	if (nRet == SOCKET_ERROR)
	{
		TRACE("TcpClient  put the socket into nonblocking mode Fail!!\n");		
		return TCPCLIENT_ERROR_NONBLOCKING;
	}
	
	nRet = connect(m_TcpClient.sock, (LPSOCKADDR)&(m_TcpClient.Addr), sizeof(m_TcpClient.Addr));

	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{

			to.tv_sec = nTimeOut;
			to.tv_usec = 0;

			FD_ZERO(&fd);
			FD_SET(m_TcpClient.sock,&fd);
			nConnectRet = select(0, NULL, &fd, NULL, &to);
			if (nConnectRet <= 0)
			{
				return TCPCLIENT_ERROR_CONNECT;
			}
		}

	}

	//设回阻塞模式
	ulLock = TCPCLIENT_BLOCKING;
	nRet = ioctlsocket(m_TcpClient.sock,FIONBIO,&ulLock);
	if (nRet == SOCKET_ERROR)
	{
		TRACE("TcpClient  put the socket into blocking mode Fail!!\n");		
		return TCPCLIENT_ERROR_BLOCKING;
	}
	return TCPCLIENT_CONNECTSUCCESS;
}

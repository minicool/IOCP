// IOSocketUdp.cpp: implementation of the CIOSocketUdp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOSocketUdp.h"

extern CWriteLogThread gWriteLogApp;

int CIOSocketUdp::UdpSendTo(PBYTE pbData, int nDataLen,
	int nDestIP, unsigned short usDestPort)
{
	_ASSERT(m_hSocket != INVALID_SOCKET);

	SOCKADDR_IN remotesin;
	memset(&remotesin, 0, sizeof(remotesin));
	remotesin.sin_family = AF_INET;
	remotesin.sin_addr .s_addr = nDestIP;
	remotesin.sin_port = htons(usDestPort);
	
	if (sendto(m_hSocket, (char *) pbData, nDataLen, 0,
			(struct sockaddr *) &remotesin, sizeof(remotesin)) == SOCKET_ERROR)
		return -1;

	return nDataLen;
}

int CIOSocketUdp::UdpSendTo(PBYTE pbData, int nDataLen,
	char* szDestIP, unsigned short usDestPort)
{
	_ASSERT(m_hSocket != INVALID_SOCKET);
	int nDestIP = inet_addr(szDestIP);
	return UdpSendTo(pbData, nDataLen, nDestIP,usDestPort);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//CMemManager*	 CIOSocketUdp::g_hMem = NULL;
CIOSocketUdp::CIOSocketUdp()
{
	m_pfudpcallback = NULL;
	m_dwInstance = NULL;
	m_evStop = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hSocket = INVALID_SOCKET;
	m_bRun = FALSE;
}

CIOSocketUdp::~CIOSocketUdp()
{
	CloseHandle(m_evStop);
}

DWORD CIOSocketUdp::ThreadProcEx()
{
	SOCKADDR_IN m_saRemoteIP;
	int m_nsaRemoteIPLen = sizeof(SOCKADDR_IN);
	TIMEVAL timevals;
	timevals.tv_sec = 0;
	timevals.tv_usec = 200;

	FD_SET ReadSet;

	TcpServerClient ClientInfo;
	char tempdata[MAX_PACKET];

	m_bRun = TRUE;
	while (m_bRun)
	{
		if (WaitForSingleObject(m_evStop, 0) == WAIT_OBJECT_0)
		{
			gWriteLogApp.WriteLog(FOR_DEBUG, TEXT("CIOSocketUdp::Run break and return"));
			break;
		}
			
		FD_ZERO(&ReadSet);
		
		FD_SET(m_hSocket, &ReadSet);
		int nTotal = select(0, &ReadSet, NULL, NULL, &timevals);
		if (nTotal == SOCKET_ERROR || nTotal == 0)
			continue;
		
		if (!FD_ISSET(m_hSocket, &ReadSet))
			continue;

		int nRcvLen = recvfrom(m_hSocket,
			tempdata,
			MAX_PACKET, 0,
			(struct sockaddr*) &m_saRemoteIP, &m_nsaRemoteIPLen);
		
		if (nRcvLen == SOCKET_ERROR || nRcvLen <= 0)
		{
			continue;
		}

		ClientInfo.pszRecvData = tempdata;
		ClientInfo.nAppID = 0;
		ClientInfo.nID = 0;
		ClientInfo.nPort = ntohs(m_saRemoteIP.sin_port);
		ClientInfo.nRecvDataLen = nRcvLen;
		strncpy(ClientInfo.szIP,inet_ntoa(m_saRemoteIP.sin_addr),32);

		if (m_pfudpcallback != NULL)
		{
			m_pfudpcallback(UDP_DATA,(DWORD)&ClientInfo,m_dwInstance);
		}
		
	}
	
	SetEvent(m_evStop);
	return 10;	
}

BOOL CIOSocketUdp::Start(char* szIPAddr, unsigned short usPort, LPFUDPCALLBACK lpfUdpCallBack,DWORD dwInstance)
{
	_ASSERT(m_hSocket == INVALID_SOCKET);
	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		gWriteLogApp.WriteLog(FOR_DEBUG, TEXT("CIOSocketUdp::Start m_hSocket==INVALID_SOCKET"));
		return FALSE;
	}

	SOCKADDR_IN localaddr;
	memset(&localaddr,0,sizeof(localaddr));
	localaddr.sin_family =AF_INET;
	localaddr.sin_port =htons(usPort);

	if(szIPAddr != NULL)
	{
		localaddr.sin_addr.s_addr = inet_addr(szIPAddr);
	}
	else
	{
		localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	m_pfudpcallback = lpfUdpCallBack;
	m_dwInstance = dwInstance;

	if(bind(m_hSocket,(struct sockaddr*)&localaddr,sizeof(localaddr))==SOCKET_ERROR)
	{
		gWriteLogApp.WriteLog(FOR_DEBUG, TEXT("CIOSocketUdp::Start bind ip:%s,Port:%d failed"),szIPAddr, usPort);
		return FALSE;
	}

	if (StartThread() == 0)
		return FALSE;

	return TRUE;
}

void CIOSocketUdp::Stop()
{
	m_bRun = FALSE;
	StopThread();
	closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;
}

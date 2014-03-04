// CompletionRoutineServer.h: interface for the CCompletionRoutineServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPLETIONROUTINESERVER_H__CFFA89DE_ED0C_4BA8_9E22_70A5D5FA8425__INCLUDED_)
#define AFX_COMPLETIONROUTINESERVER_H__CFFA89DE_ED0C_4BA8_9E22_70A5D5FA8425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpInterface.h"
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagCompletionRoutineTcpLink
{
	SOCKET sock;
	struct sockaddr_in Addr;
}CompletionRoutineTcpLink;

typedef struct tagCompletionRoutineLink 
{
	WSAOVERLAPPED ol;//重叠结构
	WSABUF wsaBuff;//数据
	DWORD dwBytes;//数据长度
	DWORD dwFlags;
	SOCKET sock;
	int nIndex;
	struct sockaddr_in Addr;
	DWORD pAppThis;
}CompletionRoutineLink;

///////////////////////////////////////////////////////////////////////////////////////////////
//
//TcpServer事件回调，可处理TCPSERVER_EVENT中的事件
//UINT uMsg:事件编号，参考TCPSERVER_EVENT
//DWORD dwParam:强制转转换成tagTcpServerClient后使用
//DWORD dwInstance:可传入用户自定义，如this指针
typedef DWORD (WINAPI * LPFTCPSERVERCALLBACK)(UINT uMsg,DWORD dwParam,DWORD dwInstance);

const int TCPSERVER_LISTENSUCCESS = 1;
const int TCPSERVER_CREATEACCEPTTHREADSUCCESS = 1;
const int TCPSERVER_NONE_CANUSESOCK = -1;
const int TCPSERVER_RECVBUFLEN = 1024;
const int TCPSERVER_CREATERECVTHREADSUCCESS = 1;
const int TCPSERVER_NONE_RELATINGSOCK = -1;

///////////////////////////////////////////////////////////////////////////////////////////////
//
class CCompletionRoutineServer  
{
public:

	CCompletionRoutineServer();
	virtual ~CCompletionRoutineServer();

    BOOL InitRoutineServer(int nMalClientMum);
    void ReleaseRoutineServer();

public:	

	int Start(UINT nPort,const char *strLocalIP,
        LPFTCPSERVERCALLBACK lpfTcpServerCallBack,DWORD dwInstance = 0,int nAppID = -1);
	int Send(int nSockID,const char * pszData,int nDataLen);
	int CloseClient(int nSockID);
	void Close();

protected:

	void InitData();
	
	void TcpServerCallBack(UINT uMsg,const char * pszIP,
		int nID,
        int nPort,
        char *pszData,
        int nDataLen);
    
	static void CALLBACK CompletionROUTINE(
								IN DWORD dwError, 
								IN DWORD cbTransferred, 
								IN LPWSAOVERLAPPED lpOverlapped, 
								IN DWORD dwFlags
								);
    
	static UINT WINAPI RecvThreadDispatch(LPVOID pParam);
    static UINT WINAPI AcceptThreadDispatch(LPVOID pParam);

	void RecvThreadWork();
	int CreateRecvThread();
	int GetUsableSock();
	void AcceptThreadWork();
	int CreateAcceptThread();
	int InitListen(UINT nPort,const char *strLocalIP);

	void DealRecv(DWORD dwError,int nSockID,DWORD cbTransferred);

private:
    
    BOOL m_bWorkThreadRun;
    UINT m_dwWorkThreadID;
    HANDLE m_hWorkThread;
    HANDLE m_hWorkThreadEndEvt;

    BOOL m_bAcceptThreadRun ;
    UINT m_dwAcceptThreadID;
    HANDLE m_hAcceptThread;
    HANDLE m_hAcceptThreadEndEvt;

	CompletionRoutineTcpLink m_TcpListen;
	
    int m_nMalClientMum;
    
    CompletionRoutineLink *m_pListClient;

	WSAEVENT m_wsaEvent[1];

private:

	LPFTCPSERVERCALLBACK m_lpfTcpServerCallBack;
	DWORD m_dwUserInstance;

	CRITICAL_SECTION m_csIsAcceptClient;
	BOOL m_bIsAcceptClient;
	CRITICAL_SECTION m_csDealRecv;
	vector<int> m_vecAcceptClientTable;//sockid

    int m_nAppID;
    
};

#endif // !defined(AFX_COMPLETIONROUTINESERVER_H__CFFA89DE_ED0C_4BA8_9E22_70A5D5FA8425__INCLUDED_)

#pragma once
#include "../../IOCP/GSocket.h"
#include "../../IOCP/HashmapManager.h"
#include <string>

class CBusinessManage_ConToPub
{
public:
	CBusinessManage_ConToPub(void);
public:
	~CBusinessManage_ConToPub(void);

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
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);

	static BOOL	Pub_All(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext);
	void	Broadcast_SendData(const char* pBuf,const DWORD dwBytes,const int nCmdType,const DWORD dwClientContext,PFN_ON_GSOCK_TRAVERSAL pfnTravesal);

	void	SendData_Broadcast_GisClient_TerminalGpsData(const char *RecvData, int nLen);
	void	SendData_Broadcast_GisClient_TerminalTxtMsgAnswerTo(char *pszDeptNum,const char *RecvData, int nReturnIndex);

private:
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType);

	BOOL	SetPubTerminal_Line(const char *RecvData, int nLen);
	void	SendData_Broadcast_GisClient_TerminalLine(const char *strDrivceNum,const char *strDrivceType,int lineState);

public:
	void	ShowOnLineDeviceInfo(void);
	void	ShowOnLineDeviceNum(void);

private:
	static BOOL	GetOnLineDeviceInfo(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext);


private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;	

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;

public:
	typedef struct _TERMINAL_INFO
	{
		BOOL bQueryInfoOk;		//是否查询到了绑定关系
		BOOL bPatrolStart;		//是否在进行巡防
		int nQueryFailCount;	//查询失败计数
		int nLineState;			//上下线状态
		int nUserType;			//绑定的用户类型
		char szDeviceNum[64];	//终端号码
		char szDeviceType[64];	//终端类型
		char szUserCode[64];	//绑定的用户编码
		char szDeptNum[64];		//绑定的用户部门
	}TERMINAL_INFO,*PTERMINAL_INFO;

	typedef CHashmapManager<string,TERMINAL_INFO> HashmapDatainfo;
	HashmapDatainfo m_HashmapDataInfo;

public:
	void Update_OnlineDeviceList_PatrolState(CString strNumber);
	BOOL FindPubTerminal(const char *szDeviceNum);
	void AddPubTerminal(const char *pszDeviceNum,const char *pszDeviceType,const char *pszDeptNum,const char *pszUserCode,int nUserType,BOOL bLineState,BOOL bPatrolStart);
	BOOL DelPubTerminal(const char *pszDeviceNum);

private:
	DWORD m_dwConToPubServerContext;

	BOOL m_IsSuceeseStart;
	char m_szLocalIP[32];
	int m_nLocalPort;
	char m_szStartInfo[512];
	char m_szErrorInfo[512];

public:
	BOOL m_bConnPubServer;
};

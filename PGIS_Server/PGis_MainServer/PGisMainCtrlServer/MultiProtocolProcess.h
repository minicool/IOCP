/*******************************************************************************
* 版权所有(C) 2013All right reserved
* 文件名称 : MultiProtocolProcess.h
* 当前版本 : 1.0.0.1
* 作    者 : WANGZH (wangzhenhua@szhitom.com)
* 设计日期 : 2013年4月23日星期二
* 内容摘要 : 
* 修改记录 : 
* 日    期  版    本  修改人   修改摘要

********************************************************************************/
#ifndef HT_MULTIPROTOCOLPROCESS
#define HT_MULTIPROTOCOLPROCESS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**********************************  头文件 ************************************/
#include "IOSocketUdp.h"
#include "CompletionRoutineServer.h"
#include "TcpClient_All.h"
#include "TcpInterface.h"
#include "FTPConveyFile.h"
#include "rwini.h"
#include "TcpSelectSocket.h"
/********************************** 常量和宏 **********************************/
const int MAX_COUNT = 64;
const int MAX_FILEPATH = 260;
const int MAX_IPADDRESS = 32;
/********************************** 数据类型 **********************************/

/********************************** 函数声明 **********************************/

/********************************** 类定义 ***********************************/

class MultiProtocolProcess
{
public:
	MultiProtocolProcess(void);
	~MultiProtocolProcess(void);

	BOOL Init(DWORD dwInstance,DWORD dwThread,UINT nAppID = 0,UINT Protocol = NULL_PROTOCOL);
	BOOL Loadconfig();
	BOOL Start();
	BOOL SendData(int nType,const char* szData,int ndatalen,int nSocketID = -1);
	BOOL Close();

private:
    static DWORD WINAPI DataCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance);
	BOOL ProcessData(int nSocketID,const char *recvData ,int nRecvLen);
	BOOL AnalyseData(int nSocketID,const char *recvData ,int nRecvLen);
	BOOL ProceSendData(const char* szData,int ndatalen,int nSocketID);

	void TcpClientConnected(int typeID,int nSocketID,const char *strIP,int nPort);
	void TcpClientDisconnect(int typeID,int nSocketID,const char *strIP,int nPort);
	void TcpServerDisconnect(int typeID,int nSocketID,const char *strIP,int nPort);
	BOOL ProcessData(int nAppID,int nSocketID,const char *recvData ,int nRecvLen);

public:
	DWORD m_dwInstance;
	CTcpClient_All m_PubServerTcp[MAX_COUNT];
	int g_nRecvConntClientCount;

private:
	CIOSocketUdp m_iosocketudp;
	int m_nLocalPort;
	int m_nRemotePort;
	char m_szRemoteIP[MAX_IPADDRESS];

	CFTPConveyFile m_ftpconvery;
	char m_strFTPDownLoadPatch[MAX_FILEPATH];
	int m_FtpFileReadTimeOut;

public:
	CCompletionRoutineServer m_completionRoutineServer;

private:
	int m_nPubSocketPort;

	Crwini rwIniTemp;

	UINT m_nProtocol;

	//包分析
	char m_RecvBuf[MAX_RECVICE_BUFFER];
	int m_nBufLenCount;	//用于数据包处理

	DWORD m_dwThreadID;	//用于投递的线程ID
	UINT m_nAppID;		//协议集ID

public:
	CTcpSelectSocket m_TcpClient; 
};

#endif
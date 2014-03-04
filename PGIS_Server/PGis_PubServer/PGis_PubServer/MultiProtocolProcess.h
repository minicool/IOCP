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
#include "FTPConveyFile.h"
/********************************** 常量和宏 **********************************/
const int MAX_COUNT = 4096;
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

	BOOL Start(DWORD dwThread,UINT Protocol,const char* szRemoteIP,int nRemotePort,const char* szLocalIP,int nLocalPort);
	BOOL SendData(int nType,const char* szData,int ndatalen,int nSocketID = -1);
	BOOL Close();

private:
    static DWORD WINAPI DataCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance);
	BOOL WINAPI AnalyseData(int nSocketID,const char *recvData ,int nRecvLen,DWORD dwInstance);
	BOOL DataProcessData(int nSocketID,const char *recvData ,int nRecvLen);

public:
	BOOL ProceSendData(const char* szData,int ndatalen,int nSocketID);

public:
	CIOSocketUdp m_iosocketudp;

	int m_nRemotePort;
	char m_szRemoteIP[MAX_IPADDRESS];
	int m_nLocalPort;
	char m_szLocalIP[MAX_IPADDRESS];

	CFTPConveyFile m_ftpconvery;
	char m_strFTPDownLoadPatch[MAX_FILEPATH];
	int m_FtpFileReadTimeOut;
	BOOL OnReadMainFtpConveyFile();
	BOOL FTP_MainDataAnalyse(const char *pstrMainDataPacket);

private:
	DWORD m_dwThreadID;	//用于投递的线程ID
	UINT m_nProtocol;

	char m_RecvBuf[MAX_RECVICE_BUFFER];
	int m_nBufLenCount;	//用于数据包处理
};

#endif
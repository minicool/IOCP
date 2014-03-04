/*
**	FILENAME			TcpClient.h
**
*/
#ifndef __TCPCLIENT_H_
#define __TCPCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTcpClient
{
public:
    CTcpClient();
    ~CTcpClient();
public:
    void StartTcpTickTimer(int nConnState,int nTimerVal);
    BOOL CheckTimerOver();
    void StopTcpTickTimer();
    
    void Reset();
    //分析接受的数据包
    BOOL AnalyseRecvData(const char *recvData ,int nRecvLen,int nAppID);
    
    inline int GetSocketID(){return m_nSocketID;}
	inline char * GetConnectIP(){return strIpAddress;};
    void SetSocketConn(int nSocketID,const char *strIp = NULL);
    
    inline int GetConnectState(){return m_nConnState;};
	inline int GetClientType(){return m_nGISClientType;};
	inline void SetClientType(int nGISClientType){m_nGISClientType = nGISClientType;};

private:
	BOOL TcpClientDataProcess(const char *recvData, int nRecvLen,int nAppID);

	void DispatchGisClientRecvData(int CmdType,const char *RecvData, int nLen);
private:
    int m_nSocketID;
    char strIpAddress[MAX_IPADDRESS_LEN];

    BOOL m_bLoginOk;

    BOOL m_bStartTimerFlag;
    int m_nConnState;
	int m_nGISClientType;
    int m_nNowTimerVal;
    int m_nTimerVal;

    char m_RecvBuf[MAX_RECVICE_BUFFER];
    int m_nBufLenCount;
};

#endif
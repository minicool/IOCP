#pragma once
#include "../../IOCP/HashmapManager.h"
#include <string>

class CBusinessManage_OldTerminal
{
public:
	CBusinessManage_OldTerminal(void);
public:
	virtual ~CBusinessManage_OldTerminal(void);

private:
	typedef struct _DATA_RECVBUFFER
	{
		char	RecvBuf[MAX_RECVICE_BUFFER];
		int		nBufLenCount;
	}DATA_RECVBUFFER,*PDATA_RECVBUFFER;

public:
	static void OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned);
	static void OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnReceiveSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned);
	static void OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned);
	static void OnUnknownError(const DWORD dwClientContext, const DWORD dwInstaned);
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

private:
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwBytes);
	BOOL	ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	DWORD	TerminalLoginProccess( const DWORD dwClientContext, const char* strDeviceCode, const char* strDeviceType);
	BOOL	SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	SendDataToGisMain(const char* RecvData, const DWORD nRecvLen, int nCmdType);
	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType);
	BOOL	DispatchAckToAnswer(DWORD dwClientContext, int nCmdType, const char*  RecvData);
	int		GetChksum(const char *strMsg);
	BOOL	PublicGpsDataProcess(GPS_DATA_TYPE gpsDataType,
			int nClientID,
			const char *SIMNum, //SIM卡号
			const char * strTimer, //时间
			const char *nsFlag,   //N/S
			const char *latitude, //纬度
			const char *ewFlag,  //E/W
			const char *longitude, //经度
			const char *avFlag,  //是否有效
			const char *speed,   //速度
			const char *Direction,   //方向
			const char *Date,   //日期;
			const char *state,
			const char *strAllData
			);

	void SendTxtMessageAnswerOkProcess(DWORD dwClientContext,const char *pcDeviceNum,int nAckValue);
	void ReportSetTempGroupAckToGisMain(DWORD dwClientContext,const char *strTerminalNum,int nFlag,int nAck);

	void DispatchSetFixGpsDtCmdToTerminal(DWORD dwClientContext);

//--------------------------HashMap DATA----------------------------------
public:
	static BOOL OnCheckSendMessageWaitAnswerTimerOut(CBusinessManage_OldTerminal* pSockInfo, const DWORD nIndex, const DWORD dwClientContext);
	static BOOL OnCheckGetSendMsgTxtTimerOut(CBusinessManage_OldTerminal* pSockInfo, const DWORD nIndex, const DWORD dwClientContext);
	BOOL SendDataToPubNetTerminal(DWORD dwClientContext, int nType, const char *DataBuf, int nBufLen);
	BOOL ResendTxtMessgToPubDevice(const DWORD dwClientContext,const char *pcSendMsgInfo,int nAckVaule);
	BOOL ResendSetTempGroupToPubDevice(const DWORD dwClientContext,const char *pcGroupNum,int nSetFlag,int nAckVaule);
	void OnCheck();

	typedef CHashmapManager<DWORD,DWORD> HashmapDatainfo;
	HashmapDatainfo m_HashmapDataInfo;

	typedef CHashmapManager<string,DWORD>  HashmapDevID;
	HashmapDevID m_HashmapDevid;

	typedef CHashmapManager<DWORD,DATA_RECVBUFFER>  HashmapDataRecvBuffer;
	HashmapDataRecvBuffer m_HashmapDataRecvBuffer;
};

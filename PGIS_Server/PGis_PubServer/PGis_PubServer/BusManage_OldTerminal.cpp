#include "stdafx.h"
#include "BusManage_OldTerminal.h"
#include "TcpInterface.h"
#include "GIOCPsocket.h"
#include "rwini.h"
#include "PubTerminalManage.h"
#include "GPSProc.h"

extern CGPSProc gPGisPubNetProc; 
extern CWriteLogThread gWriteLogApp;
extern GIOCPsocket m_iocpsocket;

extern int gRecvGpsCount; //接收GPS数据统计
extern int g_nSendTxtMsgInterval;

CBusinessManage_OldTerminal::CBusinessManage_OldTerminal(void)
{
}

CBusinessManage_OldTerminal::~CBusinessManage_OldTerminal(void)
{
}

void CBusinessManage_OldTerminal::OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	TRACE("CBaseBusinessManage::OnConnectSvr \n");
	OnReceiveSvr(dwClientContext,pBuf,dwBytes,dwInstaned);
}

void CBusinessManage_OldTerminal::OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	
}

void CBusinessManage_OldTerminal::OnReceiveSvr(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	TRACE(_T("[THREAD:%d] 整体包长为 %s %d \n"),GetCurrentThreadId(),pRecvData,nRecvLen);
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;

	DATA_RECVBUFFER temprecvbufferinfo;
	ZeroMemory(&temprecvbufferinfo,sizeof(DATA_RECVBUFFER));	
	if (!temp_this->m_HashmapDataRecvBuffer.FindItem(GetCurrentThreadId(),temprecvbufferinfo))
	{
		temprecvbufferinfo.nBufLenCount = 0;
		temp_this->m_HashmapDataRecvBuffer.AddItem(GetCurrentThreadId(),temprecvbufferinfo);
	}

	if(temprecvbufferinfo.nBufLenCount + nRecvLen > MAX_RECVICE_BUFFER)
	{
		memset(temprecvbufferinfo.RecvBuf,0,MAX_RECVICE_BUFFER);
		TRACE(_T("[THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[THREAD:%d] 需要处理的数据包长为%d %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

	int nPackageLen = 0;
	char *m_strHead =  Cmdtypelist[2].HeadData;
	unsigned int temp_nHeadDataLen = strlen(m_strHead);
	char *m_strTail =  Cmdtypelist[2].TailData;
	unsigned int temp_nTailDataLen = strlen(m_strTail);

	while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
	{
		int nHeadPoint = -1;
		char *temppHeadCurPoint = pCurPoint;
		for (int i = 0;i<nSpaceLen;i++)
		{
			if(strncmp(pCurPoint,m_strHead,temp_nHeadDataLen) != 0) //检查包头
			{
				++pCurPoint;
			}
			else
			{
				nHeadPoint = i;
				break;
			}
		}

		if (-1 == nHeadPoint)
		{
			TRACE(_T("[THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [OldTerminal] 整包%s 截取后的包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,m_strHead);
			nSpaceLen = 0;
			break;
		}

		char *ptempTailCurPoint = pCurPoint;
		int nTailPoint = 0;
		for(int i = 0 ; i < nSpaceLen; i++ )
		{
			if(strncmp(ptempTailCurPoint,m_strTail,temp_nTailDataLen) != 0) //检查包尾
			{
				++ptempTailCurPoint;
			}
			else
			{
				nTailPoint = i;
				break;
			}
		}
		if( nTailPoint > 0 )
		{
			nPackageLen = nTailPoint + temp_nTailDataLen;
			if( nPackageLen < MAX_DATA_PACKAGE_LEN )
			{
				ZeroMemory(pCurPoint + nTailPoint,temp_nTailDataLen);
				temp_this->ProessAnalyseData(dwClientContext,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
				TRACE(_T("[THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [OldTerminal][ID:%d] 完整包长为%s %d \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
			}
			else
			{
				TRACE(_T("[THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			//剩下的数据继续分包
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [OldTerminal] 整包%s 截取后的包尾错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,m_strTail);
			}
			nSpaceLen = 0;
			break;
		}
	}

	//是否有没有处理的数据
	if( nSpaceLen > 0 )
	{
		temprecvbufferinfo.nBufLenCount = nSpaceLen;
		if( pCurPoint != temprecvbufferinfo.RecvBuf )
		{
			TRACE(_T("[THREAD:%d] 没有处理的数据包长为%d %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
			memcpy(temprecvbufferinfo.RecvBuf,pCurPoint,temprecvbufferinfo.nBufLenCount);
			memset(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,0,MAX_RECVICE_BUFFER-temprecvbufferinfo.nBufLenCount);
		}
	}
	else
	{
		temprecvbufferinfo.nBufLenCount = 0;
		memset(temprecvbufferinfo.RecvBuf,0,MAX_RECVICE_BUFFER);
	}
	temp_this->m_HashmapDataRecvBuffer.AddItem(GetCurrentThreadId(),temprecvbufferinfo);
	return /*TRUE*/;
}

void CBusinessManage_OldTerminal::OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_OldTerminal::OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_TRACE,"%s [SYS] [ID:%d] [PUB->OLDTER] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_OldTerminal::OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;
	char tempSendBuf[512] = {0};
	DWORD dwPubContext;
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
	{
		return;
	}
	sprintf(tempSendBuf,"%s,%s,0",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
	temp_this->SendData(/*dwClientContext*/gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
	temp_this->m_HashmapDevid.Delete(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [%d] %s %s 发送错误退出 [旧终端登陆总数：%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,
		((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType,temp_this->m_HashmapDataInfo.GetSize());
	delete ((CPubTerminalManage*)dwPubContext);
}

void CBusinessManage_OldTerminal::OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;
	char tempSendBuf[512] = {0};
	DWORD dwPubContext;
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
	{
		return;
	}
	sprintf(tempSendBuf,"%s,%s,0",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
	temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
	temp_this->m_HashmapDevid.Delete(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [%d] %s %s 连接超时退出 [旧终端登陆总数：%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,
		((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType,temp_this->m_HashmapDataInfo.GetSize());
	delete ((CPubTerminalManage*)dwPubContext);
}

void CBusinessManage_OldTerminal::OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;
	char tempSendBuf[512] = {0};
	DWORD dwPubContext;
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
	{
		return;
	}
	sprintf(tempSendBuf,"%s,%s,0",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
	temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
	temp_this->m_HashmapDevid.Delete(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [%d] %s %s 连接数量超出退出 [旧终端登陆总数：%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,
		((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType,temp_this->m_HashmapDataInfo.GetSize());
	delete ((CPubTerminalManage*)dwPubContext);
}

void CBusinessManage_OldTerminal::OnUnknownError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;
	char tempSendBuf[512] = {0};
	DWORD dwPubContext;
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
	{
		return;
	}
	sprintf(tempSendBuf,"%s,%s,0",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
	temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
	temp_this->m_HashmapDevid.Delete(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [%d] %s %s 客户端断开该终端连接 [旧终端登陆总数：%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,
		((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType,temp_this->m_HashmapDataInfo.GetSize());
	delete ((CPubTerminalManage*)dwPubContext);
}

void CBusinessManage_OldTerminal::OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
// 	PDATA_INFO pDataInfo = new DATA_INFO;
// 	ZeroMemory(pDataInfo, sizeof(DATA_INFO));
// 	GHndDat_SetData(dwClientContext, pDataInfo);
}

void CBusinessManage_OldTerminal::OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
//	PDATA_INFO pDataInfo = (PDATA_INFO)GHndDat_GetData(dwClientContext);
//	GHndDat_SetData(dwClientContext, NULL);
// 	delete pDataInfo;
}

//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CBusinessManage_OldTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char copyBuf[MAX_GPS_MESSAGE_LEN] = {0};
	strncpy(copyBuf,RecvData,nRecvLen);

	if(copyBuf == NULL)
	{
		return FALSE;
	}

	if(nRecvLen < PACKAGE_PUB_LEN)
	{
		return FALSE;
	}

	int nCmdType = 0;
	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	char *pData[20];
	for(int i = 0; i < 20; i++)
	{
		pData[i] = NULL;
	}

	strncpy(tempBuf,copyBuf,nRecvLen);
	char *pBuf = tempBuf;

	int nCount = 0;
	while((pData[nCount]=strtok(pBuf,",")) != NULL)
	{
		nCount++;
		pBuf = NULL;
	}

	if(nCount < 1)
	{
		return FALSE;
	}

	sscanf(pData[1], "%2x",&nCmdType);

	char tempSendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	for (int i = 0,len =0; i < nCount-1; i++)
	{	
		if (1 != i)
		{
			strncpy(tempSendBuf+len,pData[i],strlen(pData[i]));
			len += strlen(pData[i]);
			if(nCount-2 != i)
			{
				strncpy(tempSendBuf+len,",",1);
				len += 1;
			}
		}
	}

	ProessAnalyseData(dwClientContext,tempSendBuf,strlen(tempSendBuf),nCmdType);

	return TRUE;
}

BOOL CBusinessManage_OldTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, int nCmdType)
{
	if(RecvData == NULL)
	{
		return FALSE;
	}

	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	char *pData[MAX_PUBLICNET_DATE_TYPE];
	for(int i = 0; i < 20; i++)
	{
		pData[i] = NULL;
	}

	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	int nCount = 0;
	while((pData[nCount]=strtok(pBuf,",")) != NULL)
	{
		nCount++;
		pBuf = NULL; 
	}

	if(nCount < 1)
	{
		return FALSE;
	}

	DWORD dwPubContext;

	char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
	switch(nCmdType)
	{
	case TERMINAL_LOGIN_REPORT:
		{
			int nRes = TerminalLoginProccess(dwClientContext,pData[0],pData[2]);
			if(nRes > 0)
			{
				DispatchAckToAnswer(dwClientContext,nCmdType,pData[3]);
				DispatchSetFixGpsDtCmdToTerminal(dwClientContext);
				if(nRes == 1) 
				{
					m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);
					sprintf(tempSendBuf,"%s,%s,1",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
					SendData(/*dwClientContext*/gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
				}
			}
		}
		break;
	case TCPCONNECT_TICK_REPORT:
		{
			DispatchAckToAnswer(dwClientContext,nCmdType,pData[1]);
		}
		break;
	case PRIVATENET_LINEON_REPORT:  //专网上线协议-14
	case PRIVATENET_LINEOFF_REPORT: //专网下线协议-15
		{
			int nAckType;
			GPS_DATA_TYPE nGpsType;
			if(nCmdType == PRIVATENET_LINEON_REPORT)
			{
				nGpsType = GPS_DATA_PRIVATENET_LINEON;
				nAckType = ACK_PRIVATENET_LINEON;
			}
			else
			{
				nGpsType = GPS_DATA_PRIVATENET_LINEOFF;
				nAckType = ACK_PRIVATENET_LINEOFF;
			}
			DispatchAckToAnswer(dwClientContext,nAckType,pData[11]);
			break;
		}
	case GPS_TINMEROUT_REPORT:  //固定时间间隔主动上传GPS信息-00
		{ 
			char strTempState[9] = {0};
			if(nCount == 11) //新协议
			{
				if( (FALSE == m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext)) || (CONNECT_GPRSLOGIN_OK != ((CPubTerminalManage*)dwPubContext)->m_nConnectState))
				{
//					gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误,终端未登记,DeviceCode:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pData[0]);   
//					return FALSE;
					if(TRUE == TerminalLoginProccess(dwClientContext,pData[0],pData[2]))
					{
						char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
						sprintf(tempSendBuf,"%s,%s,1",pData[0],pData[2]);
						SendDataToGisMain(tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
					}
				}
			}
			else if(nCount == 10)
			{
				if( (FALSE == m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext)) || (CONNECT_GPRSLOGIN_OK != ((CPubTerminalManage*)dwPubContext)->m_nConnectState))
				{		
					if(TRUE == TerminalLoginProccess(dwClientContext,pData[0],pData[2]))
					{
						char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
						sprintf(tempSendBuf,"%s,%s,1",pData[0],pData[2]);
						SendDataToGisMain(tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
					}
				}
			}
			else
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误的定时上传GPS消息,字段个数有误\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
				return FALSE;
			}

			memcpy(strTempState,pData[10],8);
			if(!PublicGpsDataProcess(GPS_DATA_TINMEROUT,dwClientContext,
				pData[0], //SIM卡号
				pData[1], //时间
				pData[2], //N/S 纬度标志
				pData[3], //纬度
				pData[4], //E/W 经度标志
				pData[5], //经度
				pData[6], //定位标志 是否有效
				pData[7],  //速度
				pData[8],  //方向
				pData[9],  //日期
				strTempState,  //状态
				RecvData
				))
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(定时上传GPS消息),字段格式错\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
				return FALSE;
			}
			break;
		}
	case GPS_HISTORY_REPORT: //历史GPS数据
		//if(!PubNetHistoryGpsDataProcess(pData[1], pData[3],pData[4]))
		//{
		//    gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(历史GPS消息),字段格式错\n",strCurrTime,RecvPubMsg.nClientID);
		//    return FALSE;
		//}
		break;
	case GPS_MONITOR_RESPONSE:  //即时跟踪GPS协议-01
		{	
			if( (FALSE == m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext)) || (CONNECT_GPRSLOGIN_OK != ((CPubTerminalManage*)dwPubContext)->m_nConnectState))
			{
// 				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][GPS_MONITOR_RESPONSE]错误,终端未登记,DeviceCode:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pData[0]);   
// 				return FALSE;
				if(TRUE == TerminalLoginProccess(dwClientContext,pData[0],pData[2]))
				{
					char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
					sprintf(tempSendBuf,"%s,%s,1",pData[0],pData[2]);
					SendDataToGisMain(tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
				}
			}

			if(!PublicGpsDataProcess(GPS_DATA_MONITOR,dwClientContext,
				pData[0], //SIM卡号
				pData[1], //时间
				pData[2], //N/S 纬度标志
				pData[3], //纬度
				pData[4], //E/W 经度标志
				pData[5], //经度
				pData[6], //定位标志 是否有效
				pData[7],  //速度
				pData[8],  //方向
				pData[9],  //日期
				pData[10],  //状态
				RecvData
				) )
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(即时跟踪GPS信息),字段格式错\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
				return FALSE;
			}
			break;
		}
	case GPS_BOUNDCALL_RESPONSE:  //应答区域点名GPS协议-02
		{	
			if( (FALSE == m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext)) || (CONNECT_GPRSLOGIN_OK != ((CPubTerminalManage*)dwPubContext)->m_nConnectState))
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]错误(区域点名GPS信息),终端未登记,DeviceCode:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pData[0]);
				return FALSE;
			}
			break;
		}
	case SEND_MESSAGE_ANSWER:
		{
			int nMsgAck = 0;
			sscanf(pData[1], "%4x",&nMsgAck);
			SendTxtMessageAnswerOkProcess(dwClientContext,pData[0],nMsgAck);
			break;
		}
	case QUERY_MY_POSITION://上传请求查询“我的位置”信息协议-16
		{
			//写文件,请求我的位置
			break;
		}
	case QUERY_DESTINATION_POSITION: //上传请求查询目的地经纬度协议-17
		{
			//写文件,上传请求查询目的地经纬度协议
			break;
		}
	case CTL_TERMINAL_MOUNTGUARD_RESPONSE:  //终端上班下班-18
		{
			int nTmpGuardState = atoi(pData[10]);

			//应答上班下班
			DispatchAckToAnswer(dwClientContext,ACK_TERMINAL_MOUNTGUARD,pData[11]);

			GPS_DATA_TYPE nGpsType;
			if(nTmpGuardState == 1)
			{
				nGpsType = GPS_DATA_MY_ONDUTY;
			}
			else
			{
				nGpsType = GPS_DATA_MY_OFFDUTY;
			}

			//上班下班GPS数据
			//         PublicGpsDataProcess(nGpsType,RecvPubMsg.nClientID,
			// 			    pData[1], //SIM卡号
			// 			    pData[3], //时间
			// 			    pData[4], //N/S 纬度标志
			// 			    pData[5], //纬度
			// 			    pData[6], //E/W 经度标志
			// 			    pData[7], //经度
			// 			    pData[8], //定位标志 是否有效
			// 			    pData[9],  //速度
			//                 pData[10],  //方向
			//                 pData[11],  //日期
			//                 pData[14]  //状态
			// 			);
			break;
		}
		/*****************************上行应答控制命令协议**************************************/
	case CTL_PRIVATENET_LINE_RESPONSE:		//应答控制专网上线和专网下线命令-20
	case CTL_TERMINAL_WAKEUP_RESPONSE:		//应答控制终端集群遥毙和摇醒-21
		break;
		/***********************************上行报警协议****************************************/
	case GPS_ALERTING_REPORT:  //报警协议-30
		break;
		/*********************************上行应答设置参数协议**********************************/
	case SET_COMMUNICATION_MODEL_RESPONSE: //应答设置通讯方式参数-60
	case SET_GPRS_PARAMETER_RESPONSE://应答设置GPRS参数-61
	case SET_GPS_PARAMETER_RESPONSE://应答设置GSM参数-62
	case SET_TERMINAL_NUM_RESPONSE:			//应答设置终端专网号码-63
		break;
	case SET_TEMP_NUM_RESPONSE:	//应答设置动态编组号码-64
		{
			int nFlag = atoi(pData[1]);
			if(nFlag > 0)
			{
				nFlag = 1;
			}

			int nAck = 0;
			sscanf(pData[2], "%4x",&nAck);

			ReportSetTempGroupAckToGisMain(dwClientContext,pData[0],nFlag,nAck);	//未发送
			break;
		}
	case SET_ALERT_NUM_RESPONSE:          //应答设置专网倒放告警号码-65
	case SET_SAFETYCHECK_NUM_RESPONSE:		//应答设置专网安检号码-66
	case SET_GPSCENTER_NUM_RESPONSE:      //应答设置专网GPS中心号码-67
	case SET_BACK_MODEL_RESPONSE:			//应答设置专网背景模式-68
		break;
		/*********************************上行应答读取参数协议**********************************/
	case QUERY_COMMUNICATION_MODEL_RESPONSE://应答读取通讯方式参数-80
	case QUERY_GPS_PARAMETER_RESPONSE:	    //应答读取GPRS参数-81
	case QUERY_GSM_PARAMETER_RESPONSE: //应答读取GSM参数-82
	case QUERY_TERMINAL_NUM_RESPONSE:		//应答读取终端专网号码-83 
	case QUERY_TEMP_NUM_RESPONSE:           //应答读取动态编组号码-84   
	case QUERY_ALERT_NUM_RESPONSE:          //应答读取专网倒放告警号码-85
	case QUERY_SAFETYCHECK_NUM_RESPONSE:    //应答读取专网安检号码-86
	case QUERY_GPSCENTER_NUM_RESPONSE: //应答读取专网GPS中心号码-87
	case QUERY_BACK_MODEL_RESPONSE: //应答读取专网背景扫描模式-88
	case QUERY_MAGNETIC_BOUND_RESPONSE:     //应答读取专网场强参数-89
	case QUERY_TERMINAL_TYPE_RESPONSE:      //应答读取终端机型参数-8A
	case QUERY_WAKEUP_RESPONSE:  //应答读取终端遥毙摇醒状态-8B
	case QUERY_GPS_SEND_TINMEROUT: //应答读取终端固定上传GPS信息时间间隔-8C
		break;
		/***************************************************************************************/
	default:
		break;
	}

	return TRUE;
}

DWORD CBusinessManage_OldTerminal::TerminalLoginProccess( const DWORD dwClientContext, const char* strDeviceCode, const char* strDeviceType)
{ 
	DWORD dwRes = 0;
	//判断连接是否以及被登记
	DWORD dwPubContext;

	if( (TRUE == m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext)) && (((CPubTerminalManage*)dwPubContext)->m_nConnectState == CONNECT_GPRSLOGIN_OK))
	{		
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d]终端GPRS连接已经登记\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);    
		return dwRes;
	}

	dwRes = 1;
	DWORD temp_dwClientContext;
	if((TRUE == m_HashmapDevid.FindItem(strDeviceCode,temp_dwClientContext)) && (TRUE == m_HashmapDataInfo.FindItem(temp_dwClientContext,dwPubContext)) && (((CPubTerminalManage*)dwPubContext)->m_nConnectState == CONNECT_GPRSLOGIN_OK))
	{	
		if ((dwClientContext != temp_dwClientContext))
		{   
			m_HashmapDevid.Delete(strDeviceCode);
			m_HashmapDataInfo.Delete(temp_dwClientContext);
			delete ((CPubTerminalManage*)dwPubContext);
			GTcpSvr_CloseClient(temp_dwClientContext);  
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d]相同DeviceCode终端登录，强制断开之前的连接 DeviceCode:%s 总登陆人数：%d\n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				temp_dwClientContext,((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,m_HashmapDataInfo.GetSize());
			dwRes = 2;
		}
	}
	
	m_HashmapDevid.AddItem(strDeviceCode,dwClientContext);

	dwPubContext = DWORD(new CPubTerminalManage); 
	((CPubTerminalManage*)dwPubContext)->m_nConnectState = CONNECT_GPRSLOGIN_OK;
	strncpy(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,strDeviceCode,strlen(strDeviceCode));
	strncpy(((CPubTerminalManage*)dwPubContext)->m_csDeviceType,strDeviceType,strlen(strDeviceType));
	m_HashmapDataInfo.AddItem(dwClientContext,dwPubContext);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] 旧终端登录 DeviceCode:%s [旧终端在线总数:%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,m_HashmapDataInfo.GetSize());

	return dwRes;
}

BOOL CBusinessManage_OldTerminal::SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",strlen(RecvData),nCmdType,RecvData);
	//gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,sendBuf);
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_OldTerminal::SendDataToGisMain(const char* RecvData, const DWORD nRecvLen, int nCmdType)
{
	if (gPGisPubNetProc.m_Udp)
	{
		return gPGisPubNetProc.m_MppPubToCon.SendData(nCmdType,RecvData,nRecvLen);	
	}
	else
	{
		return SendData(gPGisPubNetProc.m_dwPubToCon,RecvData,nRecvLen,nCmdType);
	}
}

BOOL CBusinessManage_OldTerminal::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType)
{
	TRACE("[DispatchAckToAnswer] \n");
	DWORD dwPubContext;
	char sendBuf[512] = {0};
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);
	sprintf(sendBuf,"%s,%s,NULL",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);
	return SendData(dwClientContext,sendBuf,strlen(sendBuf),nCmdType);
}

BOOL CBusinessManage_OldTerminal::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType, const char*  AckValue)
{

	char tempStr[128] = {0};
	char sendBuf[256] = {0};
	CString StrTrace;
	switch(nCmdType)
	{
	case ACK_TERMINAL_LOGIN: //应答终端GPRS登录信-10
		sprintf(tempStr,"HYT,10,%s,",AckValue);
		StrTrace = _T("应答终端GPRS登录");
		break;
	case ACK_TERMINAL_OPEN: //应答终端开机信息-11
		sprintf(tempStr,"HYT,11,%s,",AckValue);
		StrTrace = _T("应答终端开机");
		break;
	case ACK_GPS_TERMINAL_OPEN: //应答终端开机GPS信息-12
		sprintf(tempStr,"HYT,12,%s,",AckValue);
		StrTrace = _T("应答终端开机GPS");
		break;
	case ACK_TCPCONNECT_TICK: //应答握手
		sprintf(tempStr,"HYT,13,%s,",AckValue);
		StrTrace = _T("应答握手");
		break;
	case ACK_PRIVATENET_LINEON: //向终端应答专网络上线确认
		sprintf(tempStr,"HYT,14,%s,",AckValue);
		StrTrace = _T("应答专网上线确认");
		break;
	case ACK_PRIVATENET_LINEOFF: //向终端应答专网络下线确认
		sprintf(tempStr,"HYT,15,%s,",AckValue);
		StrTrace = _T("应答专网络下线确认");
		break;
	case ACK_GPS_ALERTING: //向终端应答报警确认
		sprintf(tempStr,"HYT,30,00,%s,",AckValue);
		StrTrace = _T("应答报警确认");
		break;
	case ACK_TERMINAL_MOUNTGUARD: //应答上班下班
		sprintf(tempStr,"HYT,18,00,%s,",AckValue);
		StrTrace = _T("应答上班下班");
		break;
	default:
		return FALSE;
		break;
	}

	CString strCurrTime;
	strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	//取效验和
	int nChksum = GetChksum(tempStr);
	sprintf(sendBuf,"*%s$%X#",tempStr,nChksum);

	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

int CBusinessManage_OldTerminal::GetChksum(const char *strMsg)
{
	int nChk ;
	int nLen = strlen(strMsg);
	nChk = 0;
	for(int i = 0; i < nLen; i++ )
	{
		nChk = nChk ^ strMsg[i];
	}

	return nChk;
}

BOOL CBusinessManage_OldTerminal::PublicGpsDataProcess(GPS_DATA_TYPE gpsDataType,
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
									)
{
	//数据检测
	if(strlen(strTimer) != 6)
	{
		return FALSE;
	}

	if(strlen(avFlag) != 1 || strlen(nsFlag) != 1 || strlen(ewFlag) != 1)
	{
		return FALSE;
	}   
	if(latitude[4] != '.' || strlen(latitude) != 9)
	{
		return FALSE;
	}
	if(longitude[5] != '.' || strlen(longitude) != 10)
	{
		return FALSE;
	}

	if(!gPGisPubNetProc.m_bDispatchZeroGPS)	//是否传输0GPS数据
	{
		float nlatitude = atol(latitude);
		if(nlatitude < 1 )
		{
			return TRUE;
		}
	}

	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	sprintf(tempBuf,"%s,%.2X,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",SIMNum,gpsDataType,strTimer,
		nsFlag,latitude,ewFlag,longitude,
		avFlag,speed,Direction,Date,
		state);

	gRecvGpsCount++;

	SendDataToGisMain(tempBuf,strlen(tempBuf),A_TYPE_TERMINAL_GPS);

	return TRUE;
}

void CBusinessManage_OldTerminal::SendTxtMessageAnswerOkProcess(DWORD dwClientContext,const char *pcDeviceNum,int nAckValue)
{
	DWORD dwPubContext;
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);

	if(!((CPubTerminalManage*)dwPubContext)->SendMessageAnswerOkCheck(nAckValue))
	{
		return;
	}

	CString strTemp;
	int nSendMsgID = ((CPubTerminalManage*)dwPubContext)->GetSendMsgID();
	if(nSendMsgID <= 0)
	{
		return;
	}
	strTemp.Format("%d",nSendMsgID);
	char TempMsgPacket[MAX_GPS_MESSAGE_LEN] = {0};

	sprintf(TempMsgPacket,"%s,03,%d", //03表示下发警情文字信息
		pcDeviceNum,
		nSendMsgID);

	//写文件，向主控报告发送文字消息应答,MsgID
	SendDataToGisMain(TempMsgPacket,strlen(TempMsgPacket),A_TYPE_APPCMD_TERMINAL);

	((CPubTerminalManage*)dwPubContext)->StopSendMessageWaitAnswer();
}

void CBusinessManage_OldTerminal::ReportSetTempGroupAckToGisMain(DWORD dwClientContext,const char *strTerminalNum,int nFlag,int nAck)
{
	DWORD dwPubContext;
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);
	
	if(((CPubTerminalManage*)dwPubContext)->m_nGroupSetAckValue == nAck)
	{
		((CPubTerminalManage*)dwPubContext)->StopGroupSetWaitAnswer();	        //停止临时编组重发定时
	}

	char TempMsgPacket[MAX_GPS_MESSAGE_LEN] = {0};
	sprintf(TempMsgPacket,"%s,%d",strTerminalNum,nFlag);
	//SendDataToGisMain(TempMsgPacket,strlen(TempMsgPacket),A_TYPE_SET_TEMPGROUP_ACK);
}

BOOL CBusinessManage_OldTerminal::SendDataToPubNetTerminal(DWORD dwClientContext, int nType, const char *DataBuf, int nBufLen)
{
	DWORD dwPubContext;
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);

	char tempStr[MAX_GPS_MESSAGE_LEN] = {0};

	CString StrTrace;
	CString strCurrTime;

	CString strSIMNum = _T(" ");
	BOOL bErrorFlag = FALSE; 
	int nTempValue;

	strSIMNum.Format(_T("%s"),((CPubTerminalManage*)dwPubContext)->GetDeviceNum());

	strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	if(((CPubTerminalManage*)dwPubContext)->m_bOldProtocol)
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>终端为老的协议版本,不支持下行数据,DeviceCode:%s\n",strCurrTime,dwClientContext,strSIMNum);
		return FALSE;
	}

	int nChksum;
	char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
	switch(nType)
	{
		/*************************中心下发应用命令信息******************************/    
	case SET_GPS_TINMEROUT://设置固定上传GPS时间间隔命令-00
		{
			int nTimeFix = atoi(DataBuf);
			if(nTimeFix > 9999)
			{
				nTimeFix = 9999;
			}
			if(nTimeFix < 60)
			{
				nTimeFix = 60;
			}

			StrTrace = _T("设置固定上传GPS时间间隔");
			sprintf(tempStr,"HYT,%.2X,%.4d,",nType,nTimeFix);

			break;
		}  
	case SET_GPS_MONITOR://中心下发即时跟踪指令-01
		{
			int nCount = 0;
			int nTimeSlot = 0;
			sscanf(DataBuf,"%d,%d",&nTimeSlot,&nCount);
			if(nTimeSlot < 5)
			{
				nTimeSlot = 5;
			}

			if(nCount > 99999)
			{
				nCount = 99999;
			}
			sprintf(tempStr,"HYT,%.2X,%.4d,%.5d,",nType,nTimeSlot,nCount);
			StrTrace = _T("即时跟踪命令");

			break;
		}  
	case SET_GPS_BOUNDCALL: //区域点名信息-02
		{
			StrTrace = _T("设置区域点名");
			sprintf(tempStr,"HYT,%.2X,%s,",nType,DataBuf);
			break;
		}
	case APP_SEND_MESSAGE: //下发文字信
		{
			StrTrace = _T("发送文字消息(立即)");
			char tmpMsg[MAX_GPS_MESSAGE_LEN] = {0};
			memcpy(tmpMsg,DataBuf,nBufLen);

			strtok(tmpMsg,",");
			char *pStrMsgID = tmpMsg;
			char *ptmpMsg = strtok(NULL,",");
			if(ptmpMsg == NULL || pStrMsgID == NULL)
			{
				return FALSE;
			}
			int nMsgID = atoi(pStrMsgID);

			if(nMsgID > 0) //nMsgID = 0 表示是发送巡防告警消息或者是查询我的位置回应消息,这两类消息不用重发
			{
				if(g_nSendTxtMsgInterval > 0)
				{
					//文字消息延时发送
					((CPubTerminalManage*)dwPubContext)->AddMsgTxtToBuffer(nMsgID,ptmpMsg);
					return TRUE;
				}
				else
				{
					((CPubTerminalManage*)dwPubContext)->StartSendMessageWaitAnswer( gPGisPubNetProc.m_nAckCount[nType],	//启动重发处理
						nMsgID,
						ptmpMsg,
						gPGisPubNetProc.m_nReSendWaitAckTimeOut);
				}
			}

			_snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",nType,ptmpMsg,gPGisPubNetProc.m_nAckCount[nType]);

			break;
		}
		/*************************中心下发控制命令信息******************************/
	case CTL_PRIVATENET_LINE://控制专网上线和专网下线命令-20
		{
			nTempValue = atoi(DataBuf);
			if(nTempValue > 0)
			{
				nTempValue = 1;
			}

			StrTrace = _T("控制专网上线和专网下线");
			sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,gPGisPubNetProc.m_nAckCount[nType]);

			break;
		}
	case CTL_TERMINAL_WAKEUP://控制终端集群遥毙和摇醒-21
		{
			nTempValue = atoi(DataBuf);
			if(nTempValue > 0)
			{
				nTempValue = 1;
			}
			StrTrace = _T("控制终端集群遥毙和摇醒");
			sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,gPGisPubNetProc.m_nAckCount[nType]);
			break;
		}
		/*************************中心下发应答报警信息******************************/
	case ACK_GPS_ALERTING://应答报警及解除报警-30
		{
			StrTrace = _T("应答报警及解除报警");
			nTempValue = atoi(DataBuf);
			sprintf(tempStr,"HYT,%.2X,%.2X,%.4X,",nType,nTempValue,gPGisPubNetProc.m_nAckCount[nType]);
			break;
		}
		/*************************中心下发应答报警信息******************************/
	case CFG_COMMUNICATION_MODEL_CMD://设置通讯方式参数-60
		{
			nTempValue = atoi(DataBuf);
			if(nTempValue > 0)
			{
				nTempValue = 1;
			}
			StrTrace = _T("设置通讯方式参数");
			sprintf(tempStr,"HYT,%.2X,%d,%.4X,",nType,nTempValue,gPGisPubNetProc.m_nAckCount[nType]);
			break;
		}
		/*************************中心下发应答应用信息******************************/


		/*************************中心下发设置命令**********************************/
	case CFG_GPRS_PARAMETER_CMD://设置GPRS参数-61
		{
			StrTrace = _T("设置GPRS参数");
			sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}
	case CFG_GSM_PARAMETER_CMD://设置GSM参数-62
		{
			StrTrace = _T("设置GSM参数");
			sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}
	case CFG_TERMINAL_NUM_CMD://设置终端专网号码-63
		{    
			StrTrace = _T("设置终端专网号码");
			sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]);
			break;
		}
	case CFG_TEMPGROUP_NUM_CMD://设置动态临时编组号码-64
		{
			char tempCps[16] = {0};
			char tempCrateFlag[2] = {0};
			tempCrateFlag[0] = DataBuf[0];
			int nCreatFlag = atoi(tempCrateFlag);
			if(nCreatFlag > 0)
			{
				StrTrace = _T("设置动态编组号码(创建)");
				nCreatFlag = 1;
			}
			else
			{
				StrTrace = _T("设置动态编组号码(删除)");
			}

			if( !gPGisPubNetProc.GetCpsxIdValue(DataBuf+2,tempCps))
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s <下行>错误的号码格式,设置动态临时编组号码,DeviceCode:%s,cmdData:%s\n",strCurrTime,strSIMNum,DataBuf);
				return FALSE;
			}
			sprintf(tempStr,"HYT,%.2X,%d,%s,%.4X,",nType,nCreatFlag,tempCps,gPGisPubNetProc.m_nAckCount[nType]); 

			//启动动态编组重发处理
			((CPubTerminalManage*)dwPubContext)->StartGroupSetWaitAnswer( gPGisPubNetProc.m_nAckCount[nType],
				tempCps,
				nCreatFlag,
				gPGisPubNetProc.m_nReSendWaitAckTimeOut);

			break;
		}
	case CFG_ALERT_NUM_CMD://设置专网倒放告警号码-65
		{
			StrTrace = _T("设置专网倒放告警号码");
			sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}
	case CFG_SAFETYCHECK_NUM_CMD://设置专网安检号码-66
		{
			StrTrace = _T("设置专网安检号码");
			sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}
	case CFG_GPSCENTER_NUM_CMD://设置专网GPS中心号码-67
		{
			StrTrace = _T("设置专网GPS中心号码");
			sprintf(tempStr,"HYT,%.2X,1,1,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}
	case CFG_BACK_MODEL_CMD://设置专网背景模式-68
		{
			StrTrace = _T("设置专网背景模式");
			sprintf(tempStr,"HYT,%.2X,%s,%.4X,",nType,DataBuf,gPGisPubNetProc.m_nAckCount[nType]); 
			break;
		}

		/************************中心下发读取参数命令********************************/
	case QUERY_COMMUNICATION_MODEL_CMD://读取通讯方式参数-80
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取通讯方式参数");
			break;
		}
	case QUERY_GPS_PARAMETER_CMD://读取GPRS参数-81
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取GPRS参数");
			break;
		}
	case QUERY_GSM_PARAMETER_CMD://读取GSM参数-82
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取GSM参数");
			break;
		}
	case QUERY_TERMINAL_NUM_CMD://读取终端专网号码-83
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取终端专网号码");
			break;
		}
	case QUERY_TEMPGROUP_NUM_CMD://读取动态临时编组号码-84
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取动态临时编组号码");
			break;
		}
	case QUERY_ALERT_NUM_CMD://读取专网倒放告警号码-85
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取专网倒放告警号码");
			break;
		}
	case QUERY_SAFETYCHECK_NUM_CMD://读取专网安检号码-86
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取终端安全检测号码");
			break;
		} 
	case QUERY_GPSCENTER_NUM_CMD://读取专网GPS中心号码-87
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取专网GPS中心号码");
			break;
		}
	case QUERY_BACK_MODEL_CMD://读取专网背景扫描模式-88
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取专网背景扫描模式");
			break;
		}
	case QUERY_MAGNETIC_BOUND_CMD://读取专网场强参数-89
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取专网场强参数");
			break;
		}
	case QUERY_TERMINAL_TYPE_CMD://读取终端机型参数-8A
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取终端机型参数");
			break;
		}
	case QUERY_WAKEUP_CMD ://读取终端遥毙摇醒状态-8B
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取终端遥毙摇醒状态");
			break;
		}
	case QUERY_GPSSEND_INTERVAL_CMD: //读取GPS固定上传时间间隔参数
		{
			sprintf(tempStr,"HYT,%.2X,%.4X,",nType,gPGisPubNetProc.m_nAckCount[nType]);
			StrTrace = _T("读取GPS固定上传时间间隔参数");
			break;
		}
	default:
		return FALSE;
	}

	nChksum = GetChksum(tempStr);
	sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);

	gPGisPubNetProc.m_nAckCount[nType]++;
	if(gPGisPubNetProc.m_nAckCount[nType] >= 0xFFFF)
	{
		gPGisPubNetProc.m_nAckCount[nType] = 0;
	}

	if(nType != SET_GPS_BOUNDCALL )
	{
		if(!GCommProt_PostSendBuf(dwClientContext,MessagePacket,strlen(MessagePacket)))
		{            
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][ID:%d][PUB->OldTer] 发送数据给终端失败,关闭TCP连接,DeviceCode:%s\n",strCurrTime,dwClientContext,strSIMNum);

// 			m_HashmapDevid.Delete(((CPubTerminalManage*)dwPubContext)->m_csDeviceCode);
// 			m_HashmapDataInfo.Delete(dwClientContext);
// 			delete ((CPubTerminalManage*)dwPubContext);
// 			GTcpSvr_CloseClient(dwClientContext); 

			return FALSE;
		}
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][PUB->OldTer] %s,DeviceCode:%s,数据:%s\n",strCurrTime,dwClientContext,StrTrace,strSIMNum,MessagePacket);
	}
	return TRUE;
}

void CBusinessManage_OldTerminal::DispatchSetFixGpsDtCmdToTerminal(DWORD dwClientContext)
{
	if(gPGisPubNetProc.m_nOpenGpsModel == OPEN_GPS_GPRSLOGIN)
	{
		char tempStr[8] ={0};
		sprintf(tempStr,"%d",gPGisPubNetProc.m_nInitFixGpsDt);
		SendDataToPubNetTerminal(dwClientContext,SET_GPS_TINMEROUT ,tempStr,strlen(tempStr));
	}
	else if(gPGisPubNetProc.m_nOpenGpsModel == OPEN_GRPS_SOON )
	{
		char tempStr[64] ={0};
		sprintf(tempStr,"%d,%d",gPGisPubNetProc.m_nInitFixGpsDt,gPGisPubNetProc.m_nMonitorGPSCount);
		SendDataToPubNetTerminal(dwClientContext,SET_GPS_MONITOR ,tempStr,strlen(tempStr)); 
	}
}

void CBusinessManage_OldTerminal::OnCheck()
{ 
	if (GTcpSvr_GetClientCount())
	{
		GTcpSvr_TraversalClient((DWORD)this, 0, GTcpSvr_GetClientCount(), (PFN_ON_GSOCK_TRAVERSAL)&OnCheckGetSendMsgTxtTimerOut);
		GTcpSvr_TraversalClient((DWORD)this, 0, GTcpSvr_GetClientCount(), (PFN_ON_GSOCK_TRAVERSAL)&OnCheckSendMessageWaitAnswerTimerOut);
	}
}

//文字消息延时发送
BOOL CBusinessManage_OldTerminal::OnCheckGetSendMsgTxtTimerOut(CBusinessManage_OldTerminal* pSockInfo, const DWORD nIndex, const DWORD dwClientContext)
{
	DWORD dw = GHndDat_GetOwner(dwClientContext);
	if(GHndDat_GetOwner(dwClientContext) == gPGisPubNetProc.m_dwOldTerminal_BusinessManage)
	{
		int i = 0;
		char csSendMsgTxt[MAX_GPS_MESSAGE_LEN] ={0};
		int nMsgID = 0;
		int nAckValue = gPGisPubNetProc.m_nAckCount[APP_SEND_MESSAGE];

		char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
		char tempStr[MAX_GPS_MESSAGE_LEN] = {0};

		DWORD dwPubContext;
		if (!pSockInfo->m_HashmapDataInfo.GetSize() ||  !pSockInfo->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
		{
			return FALSE;
		}
		
		if(((CPubTerminalManage*)dwPubContext)->GetMsgTxtTimerCheck(csSendMsgTxt,nMsgID))
		{
			_snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",APP_SEND_MESSAGE,csSendMsgTxt,nAckValue);

			int nChksum = pSockInfo->GetChksum(tempStr);
			sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);

			//发送数据
			if( !GCommProt_PostSendBuf(dwClientContext,MessagePacket,strlen(MessagePacket)))
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>发送文字消息失败,关闭TCP连接,DeviceCode:%s,数据:%s\n",
					CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->GetDeviceNum(),MessagePacket);
			}
			else
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>发送文字消息(延时),DeviceCode:%s,数据:%s\n",
					CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->GetDeviceNum(),MessagePacket);
				((CPubTerminalManage*)dwPubContext)->StartSendMessageWaitAnswer( nAckValue,nMsgID,csSendMsgTxt,gPGisPubNetProc.m_nReSendWaitAckTimeOut);				//启动重发定时
			}

			memset(MessagePacket,0,MAX_GPS_MESSAGE_LEN);

			gPGisPubNetProc.m_nAckCount[APP_SEND_MESSAGE]++;
			if(gPGisPubNetProc.m_nAckCount[APP_SEND_MESSAGE] >= 0xFFFF)
			{
				gPGisPubNetProc.m_nAckCount[APP_SEND_MESSAGE] = 0;
			}
		}
	}
	return(TRUE);
}

//检测是否有需要重发的消息
BOOL CBusinessManage_OldTerminal::OnCheckSendMessageWaitAnswerTimerOut(CBusinessManage_OldTerminal* pSockInfo, const DWORD nIndex, const DWORD dwClientContext)
{
	if(GHndDat_GetOwner(dwClientContext) == gPGisPubNetProc.m_dwOldTerminal_BusinessManage)
	{
		int i = 0;
		if(gPGisPubNetProc.m_nReSendWaitAckTimeOut <= 0)
		{
			return(FALSE);
		}

		DWORD dwPubContext;
		if (!pSockInfo->m_HashmapDataInfo.GetSize() ||  !pSockInfo->m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext))
		{
			return FALSE;
		}

		//检测文字消息发送等待应答是否超时
		int nRes = ((CPubTerminalManage*)dwPubContext)->CheckResendMsgInfoTimerOut();
		if(nRes == 1)
		{
			pSockInfo->ResendTxtMessgToPubDevice(dwClientContext,((CPubTerminalManage*)dwPubContext)->GetMessageInfo(),((CPubTerminalManage*)dwPubContext)->GetMsgAckValue());
		}
		else if( nRes == 2)
		{
			((CPubTerminalManage*)dwPubContext)->StopSendMessageWaitAnswer();

			gWriteLogApp.WriteLog(FOR_ALARM,"%s <GIS>向终端发送文字消息三次均未收到回复应答,%s\n",
				CTime::GetCurrentTime().Format("%H:%M:%S"),((CPubTerminalManage*)dwPubContext)->GetDeviceNum());

		}

		//检测动态编组消息发送等待应答是否超时
		nRes = ((CPubTerminalManage*)dwPubContext)->CheckResendTempGroupSetTimerout();
		if(nRes == 1)
		{
			pSockInfo->ResendSetTempGroupToPubDevice(dwClientContext,((CPubTerminalManage*)dwPubContext)->m_csGroupSetNum,
				((CPubTerminalManage*)dwPubContext)->m_bGroupSetFlag,
				((CPubTerminalManage*)dwPubContext)->m_nGroupSetAckValue);
		}
		else if( nRes == 2)
		{
			((CPubTerminalManage*)dwPubContext)->StopGroupSetWaitAnswer();

			gWriteLogApp.WriteLog(FOR_ALARM,"%s <GIS>向终端发送设置动态编组信息三次均未收到回复,%s\n",
				CTime::GetCurrentTime().Format("%H:%M:%S"),((CPubTerminalManage*)dwPubContext)->GetDeviceNum());
		}	
	}
	return(TRUE);
}

//重发文字消息
BOOL CBusinessManage_OldTerminal::ResendTxtMessgToPubDevice(const DWORD dwClientContext,const char *pcSendMsgInfo,int nAckVaule)
{
	char tempStr[MAX_GPS_MESSAGE_LEN] = {0};
	char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
	if(pcSendMsgInfo == NULL)
	{
		return FALSE;
	}

	if(strlen(pcSendMsgInfo) == 0)
	{
		return FALSE;
	}

	_snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%s,%.4X,",APP_SEND_MESSAGE,pcSendMsgInfo,nAckVaule);

	int nChksum = GetChksum(tempStr);
	sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);

	DWORD dwPubContext;
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);

	//发送数据
	if( GCommProt_PostSendBuf(dwClientContext,MessagePacket,strlen(MessagePacket)) == TCPSERVER_ERROR_SEND)
	{            
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>重发文字消息失败,关闭TCP连接,DeviceCode:%s\n",
			CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->GetDeviceNum());
		return FALSE;
	}

	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>重发文字消息,DeviceCode:%s,数据:%s\n",
		CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->GetDeviceNum(),MessagePacket);

	return TRUE;
}

BOOL CBusinessManage_OldTerminal::ResendSetTempGroupToPubDevice(const DWORD dwClientContext,const char *pcGroupNum,int nSetFlag,int nAckVaule)
{
	char tempStr[MAX_GPS_MESSAGE_LEN] = {0};
	char MessagePacket[MAX_GPS_MESSAGE_LEN] = {0};
	if(pcGroupNum == NULL)
	{
		return FALSE;
	}

	if(strlen(pcGroupNum) == 0)
	{
		return FALSE;
	}

	_snprintf(tempStr,MAX_GPS_MESSAGE_LEN-6,"HYT,%.2X,%d,%s,%.4X,",CFG_TEMPGROUP_NUM_CMD,nSetFlag,pcGroupNum,nAckVaule); 

	int nChksum = GetChksum(tempStr);
	sprintf(MessagePacket,"*%s$%X#",tempStr,nChksum);

	DWORD dwPubContext;
	m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);

	//发送数据
	if( !GCommProt_PostSendBuf(dwClientContext,MessagePacket,strlen(MessagePacket)))
	{            
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>重发动态组设置消息失败,关闭TCP连接,DeviceCode:%s\n",
			CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,((CPubTerminalManage*)dwPubContext)->GetDeviceNum());
		return FALSE;
	}
	CString strTemp;
	if(nSetFlag > 0)
	{
		strTemp = _T("创建");
	}
	else
	{
		strTemp = _T("删除");
	}
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d]<下行>重发动态组设置消息(%s),DeviceCode:%s,数据:%s\n",
		CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,strTemp,((CPubTerminalManage*)dwPubContext)->GetDeviceNum(),MessagePacket);

	return TRUE;

}

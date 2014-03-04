#include "stdafx.h"
#include "ConToPub_BaseBusinessManage.h"
#include "BusManage_NewTerminal.h"
#include "BusManage_OldTerminal.h"
#include "TcpInterface.h"

#include "GPSProc.h"
extern CGPSProc gPGisPubNetProc; 
extern CWriteLogThread gWriteLogApp;

CPubToCon_BusinessManage::CPubToCon_BusinessManage(void)
{

}

CPubToCon_BusinessManage::~CPubToCon_BusinessManage(void)
{
}

void CPubToCon_BusinessManage::OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	CPubToCon_BusinessManage* temp_this = (CPubToCon_BusinessManage*)dwInstaned;
	temp_this->MainStartSuccessResponse();

	char TempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	sprintf(TempBuf,"%d,00,00,NULL",dwClientContext);
	if (gPGisPubNetProc.m_Udp)
	{
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_LOGIN,TempBuf,strlen(TempBuf));	
	}
	else
	{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,TempBuf,strlen(TempBuf),A_TYPE_LOGIN);
	}
}

void CPubToCon_BusinessManage::OnDisconnectClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CPubToCon_BusinessManage::OnReceiveClt(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	CPubToCon_BusinessManage* temp_this = (CPubToCon_BusinessManage*)dwInstaned;

	DATA_RECVBUFFER temprecvbufferinfo;
	ZeroMemory(&temprecvbufferinfo,sizeof(DATA_RECVBUFFER));	
	if (temp_this->m_HashmapDataRecvBuffer.IsEmpty() || !temp_this->m_HashmapDataRecvBuffer.FindItem(GetCurrentThreadId(),temprecvbufferinfo))
	{
		temprecvbufferinfo.nBufLenCount = 0;
		temp_this->m_HashmapDataRecvBuffer.AddItem(GetCurrentThreadId(),temprecvbufferinfo);
	}

	if(temprecvbufferinfo.nBufLenCount + nRecvLen > MAX_RECVICE_BUFFER)
	{
		memset(temprecvbufferinfo.RecvBuf,0,MAX_RECVICE_BUFFER);
		TRACE(_T("[CPubToCon][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[CPubToCon][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

	int nPackageLen = 0;
	char *m_strHead =  Cmdtypelist[0].HeadData;
	unsigned int temp_nHeadDataLen = strlen(m_strHead);
	char *m_strTail =  Cmdtypelist[0].TailData;
	unsigned int temp_nTailDataLen = strlen(m_strTail);
	char *ptempCurPoint = pCurPoint;

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
			TRACE(_T("[CPubToCon][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [CPubToCon] 整包%s 截取后的包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,m_strHead);
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
				TRACE(_T("[CPubToCon][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
			}
			else
			{
				TRACE(_T("[CPubToCon][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}

			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[CPubToCon][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[CPubToCon][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [CPubToCon] 整包%s 截取后的包尾错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,m_strTail);
			}
			nSpaceLen = 0;
			break;
		}
	}

	if( nSpaceLen > 0 )
	{
		temprecvbufferinfo.nBufLenCount = nSpaceLen;
		if( pCurPoint != temprecvbufferinfo.RecvBuf )
		{
			TRACE(_T("[CPubToCon][THREAD:%d][ID:%d] 没有处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
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

void CPubToCon_BusinessManage::OnReceiveErrorClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CPubToCon_BusinessManage::OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_TRACE,"%s [SYS] [ID:%d] [PUB->CON] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CPubToCon_BusinessManage::OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_OldTerminal* temp_this = (CBusinessManage_OldTerminal*)dwInstaned;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [CPubToCon][%d] %s 发送错误退出 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CPubToCon_BusinessManage::OnHeartbeat(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CPubToCon_BusinessManage* temp_this = (CPubToCon_BusinessManage*)dwInstaned;
	char TempBuf[MAX_GPS_MESSAGE_LEN] = {0};
 	sprintf(TempBuf,"%d,00,00,NULL",dwClientContext);
	if (gPGisPubNetProc.m_Udp)
	{
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TICK,TempBuf,strlen(TempBuf));	
	}
	else
	{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,TempBuf,strlen(TempBuf),A_TYPE_TICK);
	}
}

void CPubToCon_BusinessManage::OnConnectError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [CPubToCon][ID:%d] OnConnectError 继续重连\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
}

void CPubToCon_BusinessManage::OnCreateClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{
// 	PDATA_INFO pDataInfo = new DATA_INFO;
// 	ZeroMemory(pDataInfo, sizeof(DATA_INFO));
//	GHndDat_SetData(dwClientContext, pDataInfo);
}

void CPubToCon_BusinessManage::OnDestroyClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{
// 	PDATA_INFO pDataInfo = (PDATA_INFO)GHndDat_GetData(dwClientContext);
// 	GHndDat_SetData(dwClientContext, NULL);
// 	delete pDataInfo;
}

//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CPubToCon_BusinessManage::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,nRecvLen);

	if(tempBuf == NULL)
	{
		return FALSE;
	}

	if(nRecvLen < PACKAGE_PUB_LEN)
	{
		return FALSE;
	}

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(tempBuf, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype

	if(nTempLen != nRecvLen -6)
	{
		return FALSE;
	}

	ProessAnalyseData(dwClientContext,tempBuf+6,nTempLen,nCmdType);

	return TRUE;
}

typedef enum enumGisMainAppCmd
{
	aAPP_CMD_SetFixGpsDt = 0, //设置固定上传GPS信息时间间隔
	aAPP_CMD_GpsTrace = 1, //即时跟踪命令
	aAPP_CMD_AreaGpsCall = 2,//区域点名
	aAPP_CMD_SendMessage = 3,
	app_MAX = 4,
}GISMAIN_APP_CMD;

static int g_AppCmdTB[app_MAX] = {SET_GPS_TINMEROUT,SET_GPS_MONITOR,SET_GPS_BOUNDCALL,APP_SEND_MESSAGE};

BOOL CPubToCon_BusinessManage::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, int nCmdType)
{
	if(NULL == RecvData)
	{
		return FALSE;
	}

	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	char *pData[2] = {0};
	strtok(pBuf,",");
	pData[0] = pBuf;
	pData[1] = strtok(NULL,",");

	char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
	CString strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");

	switch(nCmdType)
	{
	case TYPE_LOGIN_ACK:
		break;
	case TYPE_TICK_ACK:
		break;
	case A_TYPE_APPCMD_TERMINAL: //下发应用命令给终端,固定上传,即时跟踪,文字信息
		{	
			DWORD dwClientContext_old;
			if (gPGisPubNetProc.m_OldTerminal_BusinessManage.m_HashmapDevid.FindItem(pData[0],dwClientContext_old))
			{
				int nTempValue = atoi(pData[1]);
				int nMoveLen = strlen(pData[0]) + strlen(pData[1]);
				gPGisPubNetProc.m_OldTerminal_BusinessManage.SendDataToPubNetTerminal(dwClientContext_old,
					g_AppCmdTB[nTempValue],
					RecvData + nMoveLen, //为截去了终端号码、和信息类型的数据
					dwRecvLen - nMoveLen);
			}
			else
			{
				DWORD dwClientContext_new;		
				if (!gPGisPubNetProc.m_BaseBusinessManage.m_HashmapDevid.FindItem(pData[0],dwClientContext_new))
				{
					gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] 发送数据给终端失败,没有找到 DeviceCode:%s\n",strCurrTime,dwClientContext_new,pData[0]);
					return FALSE;
				}
				if(!SendData(dwClientContext_new,RecvData,dwRecvLen,nCmdType))
				{  
					gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] 发送数据给终端失败,关闭TCP连接,DeviceCode:%s\n",strCurrTime,dwClientContext_new,pData[0]);
					return FALSE;
				}
			}
			break;
		}
// 	case A_TYPE_SETCMD_TERMINAL: //下发设置命令给终端
// 		{
// 			DWORD dwClientContext_old;
// 			gPGisPubNetProc.m_OldTerminal_BusinessManage.m_HashmapDevid.FindItem(pData[0],dwClientContext_old); 
// 			int nTempValue = atoi(pData[1]);
// 			int nMoveLen = strlen(pData[0]) + strlen(pData[1]);
// 
// 			if(nTempValue == aSET_CMD_TEMPGROUP_NUM) //为设置动态编组
// 			{
// 				gPGisPubNetProc.m_OldTerminal_BusinessManage.SendDataToPubNetTerminal(dwClientContext_old,
// 					CFG_TEMPGROUP_NUM_CMD,
// 					RecvData + nMoveLen, //为截去了终端号码、和信息类型的数据
// 					dwRecvLen - nMoveLen);
// 			}
// 			break;
// 		}
	default:

		DWORD dwClientContext_new;		
		if (!gPGisPubNetProc.m_BaseBusinessManage.m_HashmapDevid.FindItem(pData[0],dwClientContext_new))
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] 发送数据给终端失败,没有找到 DeviceCode:%s\n",strCurrTime,dwClientContext_new,pData[0]);
			return FALSE;
		}
		if(!SendData(dwClientContext_new,RecvData,dwRecvLen,nCmdType))
		{  
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] 发送数据给终端失败,关闭TCP连接,DeviceCode:%s\n",strCurrTime,dwClientContext_new,pData[0]);
			return FALSE;
		}
		break;
	}

	return TRUE;
}

void CPubToCon_BusinessManage::MainStartSuccessResponse()
{ 
	if (GTcpSvr_GetClientCount())
	{
		GTcpSvr_TraversalClient((DWORD)this, 0, GTcpSvr_GetClientCount(), (PFN_ON_GSOCK_TRAVERSAL)&OnTraversalClient);
	}
}

BOOL CPubToCon_BusinessManage::OnTraversalClient(CPubToCon_BusinessManage* pSockInfo, const DWORD nIndex, const DWORD dwClientContext)
{	
	char TempBuf[MAX_GPS_MESSAGE_LEN] = {0};

	if (GHndDat_GetOwner(dwClientContext) == gPGisPubNetProc.m_dwBaseBusinessManage)
	{
		CBusinessManage_NewTerminal::DATA_INFO tempdatainfo;
		ZeroMemory(&tempdatainfo,sizeof(CBusinessManage_NewTerminal::DATA_INFO));
		gPGisPubNetProc.m_BaseBusinessManage.m_HashmapDataInfo.FindItem(dwClientContext,tempdatainfo);
		if(tempdatainfo.Login)
		{
 			sprintf(TempBuf,"%s,%s,1",tempdatainfo.strDeviceCode,tempdatainfo.strDeviceType);
			if (gPGisPubNetProc.m_Udp)
			{
				gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,TempBuf,strlen(TempBuf));	
			}
			else
			{
 				pSockInfo->SendData(gPGisPubNetProc.m_dwPubToCon,TempBuf,strlen(TempBuf),A_TYPE_TERMINAL_LINE);   
			}
		}
	}
	else if(GHndDat_GetOwner(dwClientContext) == gPGisPubNetProc.m_dwOldTerminal_BusinessManage)
	{
		DWORD dwPubContext;
		gPGisPubNetProc.m_OldTerminal_BusinessManage.m_HashmapDataInfo.FindItem(dwClientContext,dwPubContext);
		if (((CPubTerminalManage*)dwPubContext)->m_nConnectState == CONNECT_GPRSLOGIN_OK)
		{
			sprintf(TempBuf,"%s,%s,1",((CPubTerminalManage*)dwPubContext)->m_csDeviceCode,((CPubTerminalManage*)dwPubContext)->m_csDeviceType);

			if (gPGisPubNetProc.m_Udp)
			{
				gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,TempBuf,strlen(TempBuf));	
			}
			else
			{			
				pSockInfo->SendData(gPGisPubNetProc.m_dwPubToCon,TempBuf,strlen(TempBuf),A_TYPE_TERMINAL_LINE);  
			}
		}
	}
	return(TRUE);
}

BOOL CPubToCon_BusinessManage::SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",strlen(RecvData),nCmdType,RecvData);
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CPubToCon_BusinessManage::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%d,00,NULL",dwClientContext);
	return SendData(dwClientContext,sendBuf,strlen(sendBuf),nCmdType);
}
#include "stdafx.h"
#include "BusManage_NewTerminal.h"
#include "TcpInterface.h"
#include "GIOCPsocket.h"

#include "GPSProc.h"
extern CGPSProc gPGisPubNetProc; 

extern CWriteLogThread gWriteLogApp;
extern GIOCPsocket m_iocpsocket;

CBusinessManage_NewTerminal::CBusinessManage_NewTerminal(void)
{
//	g_dwMessageID = 0;
}

CBusinessManage_NewTerminal::~CBusinessManage_NewTerminal(void)
{

}

// DWORD CBaseBusinessManage::GetMessageID()
// {
// 	if (DWORD(-1) == g_dwMessageID)
// 	{
// 		g_dwMessageID = 0;
// 	}
// 
// 	return InterlockedIncrement((volatile LONG *)&g_dwMessageID);
// }

BOOL CBusinessManage_NewTerminal::GetDataInfo(const DWORD dwClientContext,const DWORD dwInstaned,DATA_INFO* const pDatainfo) const
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	return temp_this->m_HashmapDataInfo.FindItem(dwClientContext,(*pDatainfo));
}

//-------------------------------------------------------Server--------------------------------------------------------------

void CBusinessManage_NewTerminal::OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	TRACE("CBaseBusinessManage::OnConnectSvr \n");
	OnReceiveSvr(dwClientContext,pBuf,dwBytes,dwInstaned);
}

void CBusinessManage_NewTerminal::OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_NewTerminal::OnReceiveSvr(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	TRACE(_T("[THREAD:%d] 整体包长为 %s %d \n"),GetCurrentThreadId(),pRecvData,nRecvLen);
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;

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
		TRACE(_T("[THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[THREAD:%d] 需要处理的数据包长为%d %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

	int nPackageLen = 0;
	char *m_strHead =  Cmdtypelist[0].HeadData;
	unsigned int temp_nHeadDataLen = strlen(m_strHead);
	char *m_strTail =  Cmdtypelist[0].TailData;
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
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [NewTerminal] 整包%s 截取后的包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,m_strHead);
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
			}
			else
			{
				TRACE(_T("[THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
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
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [NewTerminal] 整包%s 截取后的包尾错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,m_strTail);
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

void CBusinessManage_NewTerminal::OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if( !temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;
	gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("数据接收错误"));	
}

void CBusinessManage_NewTerminal::OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if( !temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;
	gWriteLogApp.WriteLog(FOR_TRACE,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] [Data:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("数据发送成功"),pBuf);
}

void CBusinessManage_NewTerminal::OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;
	
	CString tmp_sendbuf;
	tmp_sendbuf.Format(_T("%s,%s,0"),tmp_datainfo.strDeviceCode,tmp_datainfo.strDeviceType);
	if (gPGisPubNetProc.m_Udp){
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength());	
	}
	else{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength(),A_TYPE_TERMINAL_LINE);
	}
	temp_this->m_HashmapDevid.Delete(tmp_datainfo.strDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] [Data:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("发送数据失败 断开该终端连接"),pBuf);
}

void CBusinessManage_NewTerminal::OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;

	CString tmp_sendbuf;
	tmp_sendbuf.Format(_T("%s,%s,0"),tmp_datainfo.strDeviceCode,tmp_datainfo.strDeviceType);
	if (gPGisPubNetProc.m_Udp){
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength());	
	}
	else{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength(),A_TYPE_TERMINAL_LINE);
	}
	temp_this->m_HashmapDevid.Delete(tmp_datainfo.strDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("心跳超时 断开该终端连接"));
}

void CBusinessManage_NewTerminal::OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;

	CString tmp_sendbuf;
	tmp_sendbuf.Format(_T("%s,%s,0"),tmp_datainfo.strDeviceCode,tmp_datainfo.strDeviceType);
	if (gPGisPubNetProc.m_Udp){
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength());	
	}
	else{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength(),A_TYPE_TERMINAL_LINE);
	}
	temp_this->m_HashmapDevid.Delete(tmp_datainfo.strDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("连接数超过上限 断开该终端连接"));
}

void CBusinessManage_NewTerminal::OnUnknownError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_NewTerminal* temp_this = (CBusinessManage_NewTerminal*)dwInstaned;
	DATA_INFO tmp_datainfo;
	ZeroMemory(&tmp_datainfo,sizeof(DATA_INFO));
	if (!temp_this->m_HashmapDataInfo.FindItem(dwClientContext,tmp_datainfo))
		return;

	CString tmp_sendbuf;
	tmp_sendbuf.Format(_T("%s,%s,0"),tmp_datainfo.strDeviceCode,tmp_datainfo.strDeviceType);
	if (gPGisPubNetProc.m_Udp){
		gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength());	
	}
	else{
		temp_this->SendData(gPGisPubNetProc.m_dwPubToCon,tmp_sendbuf.GetBuffer(),tmp_sendbuf.GetLength(),A_TYPE_TERMINAL_LINE);
	}
	temp_this->m_HashmapDevid.Delete(tmp_datainfo.strDeviceCode);
	temp_this->m_HashmapDataInfo.Delete(dwClientContext);
	gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [DevID:%s Info:%s] \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
		tmp_datainfo.strDeviceCode,_T("客户端断开 断开该终端连接"));
}

void CBusinessManage_NewTerminal::OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
// 	PDATA_INFO pDataInfo = new DATA_INFO;
// 	ZeroMemory(pDataInfo, sizeof(DATA_INFO));
// 	GHndDat_SetData(dwClientContext, pDataInfo);
}

void CBusinessManage_NewTerminal::OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
//	PDATA_INFO pDataInfo = (PDATA_INFO)GHndDat_GetData(dwClientContext);
//	GHndDat_SetData(dwClientContext, NULL);
// 	delete pDataInfo;
}

//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CBusinessManage_NewTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,nRecvLen);

	if(tempBuf == NULL)
		return FALSE;
	if(nRecvLen < PACKAGE_PUB_LEN)
		return FALSE;

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(tempBuf, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype
	if(nTempLen != nRecvLen -6)
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("[%s Fun:%s ThdID:%04x MemID:%08x] [PUB->NEWTER] [Info:%s] [Data:%s] \n"),
			CTime::GetCurrentTime().Format("%H:%M:%S"),__FUNCTION__,GetCurrentThreadId(),dwClientContext,
			_T("数据包长度校验错误"),tempBuf);
		return FALSE;
	}

	ProessAnalyseData(dwClientContext,tempBuf+6,nTempLen,nCmdType);
	return TRUE;
}

BOOL CBusinessManage_NewTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, int nCmdType)
{
	if(NULL == RecvData)
		return FALSE;

	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	char *pData[2] = {0};
	strtok(pBuf,",");
	pData[0] = pBuf;
	pData[1] = strtok(NULL,",");

	char tempSendBuf[MAX_GPS_MESSAGE_LEN] = {0};
	switch(nCmdType)
	{
	case TYPE_LOGIN:
		{
			DWORD dwReult = TerminalLoginProccess(dwClientContext,pData[0],pData[1]);
			if(dwReult > 0)
			{
				DispatchAckToAnswer(dwClientContext,nCmdType);
				if(dwReult == 1) 
				{
					DATA_INFO tempdatainfo;
					ZeroMemory(&tempdatainfo,sizeof(DATA_INFO));
					m_HashmapDataInfo.FindItem(dwClientContext,tempdatainfo);
					sprintf(tempSendBuf,"%s,%s,1",tempdatainfo.strDeviceCode,tempdatainfo.strDeviceType);
					if (gPGisPubNetProc.m_Udp)
					{
						gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tempSendBuf,strlen(tempSendBuf));	
					}
					else
					{
						SendData(gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
					}
				}
			}
		}
		break;
	case TYPE_TICK:
		{
			DispatchAckToAnswer(dwClientContext,nCmdType);
		}
		break;
	default:
		{
			DATA_INFO tempdatainfo;
			ZeroMemory(&tempdatainfo,sizeof(DATA_INFO));
			m_HashmapDataInfo.FindItem(dwClientContext,tempdatainfo);
			if(TRUE != tempdatainfo.Login)
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] 未登记，进行登记\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
				DWORD dwReult = TerminalLoginProccess(dwClientContext,pData[0],pData[1]);
				if(1 == dwReult)
				{
					sprintf(tempSendBuf,"%s,%s,1",pData[0],pData[1]);
					if (gPGisPubNetProc.m_Udp)
					{
						gPGisPubNetProc.m_MppPubToCon.SendData(A_TYPE_TERMINAL_LINE,tempSendBuf,strlen(tempSendBuf));	
					}
					else
					{
						SendData(gPGisPubNetProc.m_dwPubToCon,tempSendBuf,strlen(tempSendBuf),A_TYPE_TERMINAL_LINE);
					}
				}
			}
			else
			{	
				if (gPGisPubNetProc.m_Udp)
				{
					gPGisPubNetProc.m_MppPubToCon.SendData(nCmdType,RecvData,dwRecvLen);	
				}
				else
				{
					SendData(gPGisPubNetProc.m_dwPubToCon,RecvData,dwRecvLen,nCmdType);
				}
			}

		}
		break;
	}

	return TRUE;
}
 
DWORD CBusinessManage_NewTerminal::TerminalLoginProccess( const DWORD dwClientContext, const char* strDeviceCode, const char* strDeviceType)
{ 
	DWORD dwRes = 0;
	//判断连接是否以及被登记
	DATA_INFO tempdatainfo;
	ZeroMemory(&tempdatainfo,sizeof(DATA_INFO));	

	if( (TRUE == m_HashmapDataInfo.FindItem(dwClientContext,tempdatainfo)) && (tempdatainfo.Login == TRUE))
	{		
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS][NEW][ID:%d] 终端已经登录 DeviceCode:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,tempdatainfo.strDeviceCode);    
		return dwRes;
	}

	dwRes = 1;
	DWORD temp_dwClientContext;
	if((TRUE == m_HashmapDevid.FindItem(strDeviceCode,temp_dwClientContext))&&(TRUE == m_HashmapDataInfo.FindItem(temp_dwClientContext,tempdatainfo)) && (tempdatainfo.Login == TRUE))
	{	
		if ((dwClientContext != temp_dwClientContext))
		{   
			GTcpSvr_CloseClient(temp_dwClientContext);
			m_HashmapDevid.Delete(tempdatainfo.strDeviceCode);
			m_HashmapDataInfo.Delete(temp_dwClientContext);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS][NEW][ID:%d] 相同DeviceCode终端登录，强制断开之前连接 DeviceCode:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),
				temp_dwClientContext,tempdatainfo.strDeviceCode);
			dwRes = 2;
		}
	}
	
	m_HashmapDevid.AddItem(strDeviceCode,dwClientContext);

	ZeroMemory(&tempdatainfo,sizeof(DATA_INFO));
	tempdatainfo.Login = TRUE;
	strncpy(tempdatainfo.strDeviceCode,strDeviceCode,strlen(strDeviceCode));
	strncpy(tempdatainfo.strDeviceType,strDeviceType,strlen(strDeviceType));
	m_HashmapDataInfo.AddItem(dwClientContext,tempdatainfo);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS][NEW][ID:%d] 新终端登录 DeviceCode:%s [新终端在线总数:%d]\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,tempdatainfo.strDeviceCode,m_HashmapDataInfo.GetSize());

	return dwRes;
}

BOOL CBusinessManage_NewTerminal::SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",strlen(RecvData),nCmdType,RecvData);
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_NewTerminal::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType)
{
	DATA_INFO tempdatainfo;
	ZeroMemory(&tempdatainfo,sizeof(DATA_INFO));
	m_HashmapDataInfo.FindItem(dwClientContext,tempdatainfo);

	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s,%s,NULL",tempdatainfo.strDeviceCode,tempdatainfo.strDeviceType);
	return SendData(dwClientContext,sendBuf,strlen(sendBuf),nCmdType);
}

void CBusinessManage_NewTerminal::ShowLoginTerNum(void)
{
	gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 在线Gis信息 数据连接数:%d SOCKET连接数：%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_HashmapDevid.GetSize(),GTcpSvr_GetListenerConnectCount(gPGisPubNetProc.m_dwBaseBusinessManage));
}
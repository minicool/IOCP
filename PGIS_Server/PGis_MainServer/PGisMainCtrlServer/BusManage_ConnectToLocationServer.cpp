#include "StdAfx.h"
#include "BusManage_ConnectToLocationServer.h"

#include "PGisMainCtrlServer.h"
#include "MemoryPool.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;
extern CMemoryPool gWebServiceList;
extern UINT g_dwWebThreadID;

CBusinessManage_ConnectToLocationServer::CBusinessManage_ConnectToLocationServer(void)
{
	m_IsSuceeseStart = FALSE;
	m_bConnLocationServer = FALSE;
}

CBusinessManage_ConnectToLocationServer::~CBusinessManage_ConnectToLocationServer(void)
{
	m_HashmapDataRecvBuffer.ClearAll();
}

BOOL CBusinessManage_ConnectToLocationServer::Start()
{	
	if (!gPGisMainCtrlProc.m_iocpsocket.m_IsIOCPActive || !LoadConfig() || !m_IsConnLocationServe)
	{
		return FALSE;
	}
	
	if (gPGisMainCtrlProc.m_iocpsocket.GTcpSvr_Connect(m_dwLocationServerContext,m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,0,(DWORD)this,
		&CBusinessManage_ConnectToLocationServer::OnConnectClt,
		&CBusinessManage_ConnectToLocationServer::OnReceiveClt,
		&CBusinessManage_ConnectToLocationServer::OnSendedClt,
		&CBusinessManage_ConnectToLocationServer::OnSendErrorClt,
		&CBusinessManage_ConnectToLocationServer::OnConnectError,
		&CBusinessManage_ConnectToLocationServer::OnDisconnectClt,
		&CBusinessManage_ConnectToLocationServer::OnHeartbeat,
		&CBusinessManage_ConnectToLocationServer::OnCreateClientClt,
		&CBusinessManage_ConnectToLocationServer::OnDestroyClientClt))
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szStartInfo);
		m_IsSuceeseStart = TRUE;
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szErrorInfo);  
	}

	return m_IsSuceeseStart;
}

DWORD CBusinessManage_ConnectToLocationServer::GetContext()
{
	return m_dwLocationServerContext;
}

BOOL CBusinessManage_ConnectToLocationServer::LoadConfig()
{
	Crwini rwIniTemp;

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}	

	if(NULL == (m_IsConnLocationServe = rwIniTemp.GetInt(_T("LocationServer"),_T("IsConnLocationServer"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_IsConnLocationServe = FALSE;
	}
	if(NULL == rwIniTemp.ReadString(_T("LocationServer"),_T("LocalIP"),gPGisMainCtrlProc.g_strConfigFile,m_szLocalIP)) 
	{
		strncpy(m_szLocalIP,"",sizeof(m_szLocalIP));
	}
	if(NULL == rwIniTemp.ReadString(_T("LocationServer"),_T("LocationServerIP"),gPGisMainCtrlProc.g_strConfigFile,m_szConRemoteIP)) 
	{
		strncpy(m_szConRemoteIP,"",sizeof(m_szConRemoteIP));
	}
	if(NULL == (m_nConRemotePort = rwIniTemp.GetInt(_T("LocationServer"),_T("LocationServerPort"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_nConRemotePort = 3130;
	}
	if(NULL == rwIniTemp.ReadString(_T("LocationServer"),_T("StartInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szStartInfo)) 
	{
		strncpy(m_szStartInfo,_T("服务启动成功!"),sizeof(m_szStartInfo));
	}
	if(NULL == rwIniTemp.ReadString(_T("LocationServer"),_T("ErrorInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szErrorInfo)) 
	{
		strncpy(m_szErrorInfo,_T("服务启动失败!"),sizeof(m_szErrorInfo));
	} 

	return TRUE;
}

/****************************************************************************************************************************/
/*													CALLBACK BUSINESS														*/
/****************************************************************************************************************************/

void CBusinessManage_ConnectToLocationServer::OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToLocationServer* temp_this = (CBusinessManage_ConnectToLocationServer*)dwInstaned;
	temp_this->m_bConnLocationServer = TRUE;
	temp_this->EventLogin(dwClientContext);
}

void CBusinessManage_ConnectToLocationServer::OnDisconnectClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToLocationServer::OnReceiveClt(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToLocationServer* temp_this = (CBusinessManage_ConnectToLocationServer*)dwInstaned;

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
		TRACE(_T("[Location][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[Location][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

	int nPackageLen = 0;
	char *ptempCurPoint = pCurPoint;

	while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
	{
		int nHeadPoint = -1;
		char *temppHeadCurPoint = pCurPoint;
		for (int i = 0;i<nSpaceLen;i++)
		{
			if(PACKAGE_HEAD_DATA_STX != pCurPoint[0]) //检查包头
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
			TRACE(_T("[Location][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location] 整包%s 截取后的包头错 %s,没有找到 %x\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,PACKAGE_HEAD_DATA_STX);
			nSpaceLen = 0;
			break;
		}

		char *ptempTailCurPoint = pCurPoint;
		int nTailPoint = 0;
		for(int i = 0 ; i < nSpaceLen; i++ )
		{
			if(PACKAGE_TAIL_DATA_ETX != ptempCurPoint[i]) //检查包头
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
			nPackageLen = nTailPoint + 1;
			if( nPackageLen < MAX_DATA_PACKAGE_LEN )
			{
				pCurPoint[nTailPoint] = NULL;
				temp_this->ProessAnalyseData(dwClientContext,pCurPoint+1,nPackageLen-2);
				TRACE(_T("[Location][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+2,nPackageLen-2);
			}
			else
			{
				TRACE(_T("[Location][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}

			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[Location][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[Location][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location] 整包%s 截取后的包尾错 %s,没有找到 %X \n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,PACKAGE_TAIL_DATA_ETX);
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

void CBusinessManage_ConnectToLocationServer::OnReceiveErrorClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToLocationServer::OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [SYS] [ID:%d] [PUB->CON] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConnectToLocationServer::OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location][ID:%d] %s 发送错误退出 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConnectToLocationServer::OnHeartbeat(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToLocationServer* temp_this = (CBusinessManage_ConnectToLocationServer*)dwInstaned;
	temp_this->EventHeartBeat(dwClientContext);
}

void CBusinessManage_ConnectToLocationServer::OnConnectError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToLocationServer* temp_this = (CBusinessManage_ConnectToLocationServer*)dwInstaned;
	temp_this->m_bConnLocationServer = FALSE;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location][ID:%d] OnConnectError 继续重连\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
}

void CBusinessManage_ConnectToLocationServer::OnCreateClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToLocationServer::OnDestroyClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

//------------------------------------BASE EVENT-----------------------------------------------------------------------------

BOOL CBusinessManage_ConnectToLocationServer::EventHeartBeat(const DWORD dwClientContext)
{
	char sendBuf[16] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"000801");
	sendBuf[7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));	
}

BOOL CBusinessManage_ConnectToLocationServer::EventLogin(const DWORD dwClientContext)
{
	char sendBuf[16] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"000801");
	sendBuf[7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));	
}


//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CBusinessManage_ConnectToLocationServer::GSendData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen,const int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",strlen(RecvData),nCmdType,RecvData);
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_ConnectToLocationServer::SendData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen,const int nCmdType)
{
	if ((nRecvLen + 7)>SEND_DATA_PACKAGE_LEN)
	{
		return FALSE;
	}
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"%.4X%.2X%s",nRecvLen+8,nCmdType,RecvData);
	sendBuf[nRecvLen + 7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_ConnectToLocationServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,nRecvLen);

	if(tempBuf == NULL)
	{
		return FALSE;
	}

	if(nRecvLen < MAIN_DATA_PACKAGE_LEN)
	{
		return FALSE;
	}

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(RecvData, "%4x%2x",&nTempLen,&nCmdType);

	if(nTempLen != nRecvLen + 2)
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Location] 数据长度不一致,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
		return FALSE;
	}

	ProessAnalyseData(dwClientContext,tempBuf+6,nTempLen,nCmdType);

	return TRUE;
}

BOOL CBusinessManage_ConnectToLocationServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, const int nCmdType)
{
	if(NULL == RecvData)
	{
		return FALSE;
	}

	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	char tempSendBuf[MAX_DATA_PACKAGE_LEN] = {0};

	switch(nCmdType)
	{
	case E_TYPE_LOGIN_ACK:
		break;
	case E_TYPE_TICK_ACK:
		break;
	case E_TYPE_LOCATION_EVENT_ACK: 
		{
			strtok(pBuf,",");
			char *pszQueryIndex = pBuf; //编号
			char *pszDeviceCode = strtok(NULL,",");
			char *pszLocation = strtok(NULL,",");

			if(NULL == pszQueryIndex || NULL == pszDeviceCode|| NULL == pszLocation)
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location] 无法识别的地理位置服务器数据\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
				break;
			}

			DWORD dwQIndex = atol(pszQueryIndex);
			if(dwQIndex > 0)
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [Location] 地理位置服务器回应位置,deviceCode:%s,%s,%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pszDeviceCode,pszQueryIndex,pszLocation);
				gPGisMainCtrlProc.m_DBManage_WebServer.ReportWebServer_UpDataLocation(dwQIndex,pszDeviceCode,pszLocation);
				return SendUpDataLocation(dwClientContext,dwQIndex,pszDeviceCode,pszLocation);	
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL CBusinessManage_ConnectToLocationServer::SendUpDataLocation(const DWORD dwClientContext,const DWORD dwQIndex,const char *pszDeviceCode,const char *pszLocation)
{
	CString strDeviceCode;
	strDeviceCode.Format("%s",pszDeviceCode);

	CString strLocation;
	strLocation.Format("%s",pszLocation);

	CString strQIndex;
	strQIndex.Format("%d",dwQIndex);

	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	int nSendLen = 0;

	if (m_returnIndexDataList.FindItem(dwQIndex))
	{
		CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
		if (gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(pszDeviceCode,temp_TermainalInfo))
		{
			sprintf(sendBuf,"%s,%d,%s",pszDeviceCode,dwQIndex,pszLocation);	//未按照查询标准消息返回
			gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(sendBuf,strlen(sendBuf),C_TYPE_GPSANGLY,DWORD(&temp_TermainalInfo),"",CBusinessManage_GISTerminal::GISTer_Subscription);	//广播所有GIS用户
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [Location]向相关订阅设备所属部门GIS客户端广播地理位置,%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
		}
		m_returnIndexDataList.Delete(dwQIndex);
		return TRUE;
	}

	if(GHndDat_GetOwner(dwQIndex) == gPGisMainCtrlProc.g_dwConToPubContext)				//将查询到的地理位置以文字消息发给终端
	{
		char strTextInfo[MAX_LEN_TEXT_MSG] = {0};
		if(gPGisMainCtrlProc.GetUnCodeTextInfo(strLocation,strTextInfo))							
		{
			sprintf(sendBuf,"%s,%.2X,%s,0",pszDeviceCode,c_APPCMD_SendMessage,strTextInfo);			//change by kevin 2009-12-22 在文字信息之后加信息ID =0,	协议不同
			gPGisMainCtrlProc.m_BusinessManage_ConToPub.Broadcast_SendData(sendBuf,strlen(sendBuf),A_TYPE_APPCMD_TERMINAL,gPGisMainCtrlProc.g_dwConToPubContext,CBusinessManage_ConToPub::Pub_All);
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [Location] 向查询终端返回地理位置,%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
		}
	}
	else if (GHndDat_GetOwner(dwQIndex) == gPGisMainCtrlProc.g_dwGISTerminalContext)	//将查询到的地理位置发送给GIS
	{		
		CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
		if (gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(pszDeviceCode,temp_TermainalInfo))
		{
			sprintf(sendBuf,"%s,%d,%s",pszDeviceCode,dwQIndex,pszLocation);	//未按照查询标准消息返回
			gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(sendBuf,strlen(sendBuf),C_TYPE_GPSANGLY,dwQIndex,temp_TermainalInfo.szDeptNum,CBusinessManage_GISTerminal::GISTer_DeptNum_Char);	//广播所有GIS用户
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [Location] 向查询GIS端返回地理位置,%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
		}
	}
	else	//标准信息返回
	{
		sprintf(sendBuf,"%s,%s",pszDeviceCode,pszLocation);
		GSendData(dwQIndex,sendBuf,strlen(sendBuf),E_TYPE_LOCATION_EVENT_ACK);
	}

	return TRUE;
}

BOOL CBusinessManage_ConnectToLocationServer::QueryLocationProcess(const DWORD dwClientContext_Search,const char *RecvData,const int nLen)
{
	if(!m_IsSuceeseStart)
	{
		return FALSE;
	}

	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"%d,%s",dwClientContext_Search,RecvData);

	if( TRUE == SendData(m_dwLocationServerContext,sendBuf,strlen(sendBuf),E_TYPE_LOCATION_EVENT))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [Location] 发送GPS数据给地理信息服务器,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
	}

	return TRUE;
}

BOOL CBusinessManage_ConnectToLocationServer::QueryLocationProcess(const DWORD dwClientContext_Search,const char *szDeviceCode,const char *szLongitude,const char *szLatitude)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"%s,%s,%s",szDeviceCode,szLongitude,szLatitude);
	m_returnIndexDataList.AddItem(dwClientContext_Search);
	return QueryLocationProcess(dwClientContext_Search,sendBuf,strlen(sendBuf));
}
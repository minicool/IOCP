#include "StdAfx.h"
#include "BusManage_ConnectToGPSCheckServer.h"

#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;
extern int g_nPatrAlertCount;

CBusinessManage_ConnectToGPSCheckServer::CBusinessManage_ConnectToGPSCheckServer(void)
{
	m_IsSuceeseStart = FALSE;
	m_bConnGpsCheckServer = FALSE;
}

CBusinessManage_ConnectToGPSCheckServer::~CBusinessManage_ConnectToGPSCheckServer(void)
{
	m_HashmapDataRecvBuffer.ClearAll();
	m_PatrolDevListData.ClearAll();
}

BOOL CBusinessManage_ConnectToGPSCheckServer::Start()
{	
	if (!gPGisMainCtrlProc.m_iocpsocket.m_IsIOCPActive || !LoadConfig() || !m_IsConnLocationServe)
	{
		return FALSE;
	}

	if (gPGisMainCtrlProc.m_iocpsocket.GTcpSvr_Connect(m_dwGPSCheckServerContext,m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,0,(DWORD)this,
		&CBusinessManage_ConnectToGPSCheckServer::OnConnectClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnReceiveClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnSendedClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnSendErrorClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnConnectError,
		&CBusinessManage_ConnectToGPSCheckServer::OnDisconnectClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnHeartbeat,
		&CBusinessManage_ConnectToGPSCheckServer::OnCreateClientClt,
		&CBusinessManage_ConnectToGPSCheckServer::OnDestroyClientClt))
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szStartInfo);
		m_IsSuceeseStart = TRUE;
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szErrorInfo);  
	}

//	ReportWebServer_LoadPatrolDeviceData();
	return m_IsSuceeseStart;
}

DWORD CBusinessManage_ConnectToGPSCheckServer::GetContext()
{
	return m_dwGPSCheckServerContext;
}

BOOL CBusinessManage_ConnectToGPSCheckServer::LoadConfig()
{
	Crwini rwIniTemp;

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}	

	if(0 == (m_IsConnLocationServe = rwIniTemp.GetInt(_T("GPSCheckServer"),_T("IsConnGPSCheckServer"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_IsConnLocationServe = FALSE;
	}
	else
	{
		m_IsConnLocationServe = TRUE;
	}

	if(0 == rwIniTemp.ReadString(_T("GPSCheckServer"),_T("LocalIP"),gPGisMainCtrlProc.g_strConfigFile,m_szLocalIP)) 
	{
		strncpy(m_szLocalIP,"",sizeof(m_szLocalIP));
	}
	if(0 == rwIniTemp.ReadString(_T("GPSCheckServer"),_T("GPSCheckServerIP"),gPGisMainCtrlProc.g_strConfigFile,m_szConRemoteIP)) 
	{
		strncpy(m_szConRemoteIP,"",sizeof(m_szConRemoteIP));
	}
	if(0 == (m_nConRemotePort = rwIniTemp.GetInt(_T("GPSCheckServer"),_T("GPSCheckServerPort"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_nConRemotePort = 6000;
	}
	if(0 == rwIniTemp.ReadString(_T("GPSCheckServer"),_T("StartInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szStartInfo)) 
	{
		strncpy(m_szStartInfo,_T("服务启动成功!"),sizeof(m_szStartInfo));
	}
	if(0 == rwIniTemp.ReadString(_T("GPSCheckServer"),_T("ErrorInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szErrorInfo)) 
	{
		strncpy(m_szErrorInfo,_T("服务启动失败!"),sizeof(m_szErrorInfo));
	} 

	return TRUE;
}

/*****************************************************************************************/
/*										Base Event CallBack
/*****************************************************************************************/
void CBusinessManage_ConnectToGPSCheckServer::OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToGPSCheckServer* temp_this = (CBusinessManage_ConnectToGPSCheckServer*)dwInstaned;
	temp_this->m_bConnGpsCheckServer = TRUE;
	temp_this->EventLogin(dwClientContext);
}

void CBusinessManage_ConnectToGPSCheckServer::OnDisconnectClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToGPSCheckServer::OnReceiveClt(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToGPSCheckServer* temp_this = (CBusinessManage_ConnectToGPSCheckServer*)dwInstaned;

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
		TRACE(_T("[GPSCheck][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[GPSCheck][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

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
			TRACE(_T("[GPSCheck][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck] 整包%s 截取后的包头错 %s,没有找到 %x\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,PACKAGE_HEAD_DATA_STX);
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
				TRACE(_T("[GPSCheck][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+2,nPackageLen-2);
			}
			else
			{
				TRACE(_T("[GPSCheck][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}

			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[GPSCheck][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[GPSCheck][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck] 整包%s 截取后的包尾错 %s,没有找到 %x\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,PACKAGE_TAIL_DATA_ETX);
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
			TRACE(_T("[GPSCheck][THREAD:%d][ID:%d] 没有处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
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

void CBusinessManage_ConnectToGPSCheckServer::OnReceiveErrorClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToGPSCheckServer::OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [SYS] [ID:%d] [PUB->CON] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConnectToGPSCheckServer::OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck][ID:%d] %s 发送错误退出 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConnectToGPSCheckServer::OnHeartbeat(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToGPSCheckServer* temp_this = (CBusinessManage_ConnectToGPSCheckServer*)dwInstaned;
	temp_this->EventHeartBeat(dwClientContext);
}

void CBusinessManage_ConnectToGPSCheckServer::OnConnectError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToGPSCheckServer* temp_this = (CBusinessManage_ConnectToGPSCheckServer*)dwInstaned;
	temp_this->m_bConnGpsCheckServer = FALSE;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck][ID:%d] OnConnectError 继续重连\n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
}

void CBusinessManage_ConnectToGPSCheckServer::OnCreateClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToGPSCheckServer::OnDestroyClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

/*****************************************************************************************/
/*										Base Event
/*****************************************************************************************/
BOOL CBusinessManage_ConnectToGPSCheckServer::EventHeartBeat(const DWORD dwClientContext)
{
	char sendBuf[16] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"000801");
	sendBuf[7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));	
}

BOOL CBusinessManage_ConnectToGPSCheckServer::EventLogin(const DWORD dwClientContext)
{
	char sendBuf[16] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"000801");
	sendBuf[7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));	
}
/*****************************************************************************************/
/*										Data Process
/*****************************************************************************************/
BOOL CBusinessManage_ConnectToGPSCheckServer::SendData(const char* RecvData, const DWORD nRecvLen,const int nCmdType)
{
	if ((nRecvLen + 7)>SEND_DATA_PACKAGE_LEN)
	{
		return FALSE;
	}
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"%.4X%.2X%s",nRecvLen,nCmdType,RecvData);
	sendBuf[nRecvLen + 7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(m_dwGPSCheckServerContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_ConnectToGPSCheckServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
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

	if(nTempLen != nRecvLen -2)
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s 数据长度不一致,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
		return FALSE;
	}

	return ProessAnalyseData(dwClientContext,tempBuf+6,nTempLen,nCmdType);
}

BOOL CBusinessManage_ConnectToGPSCheckServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, const int nCmdType)
{
	if(NULL == RecvData)
	{
		return FALSE;
	}

	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	switch(nCmdType)
	{
	case F_TYPE_LOGIN_ACK:
		m_bConnGpsCheckServer = TRUE;
		break;
	case F_TYPE_TICK_ACK:
		break;
	case F_TYPE_PATRALERT_EVENT_ACK: 
		{
			strtok(pBuf,",");
			char *pszDeviceCode = pBuf; //编号
			char *pszPatrAlert = strtok(NULL,",");

			if(NULL == pszDeviceCode || NULL == pszPatrAlert)
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck] 无法识别的巡防管理服务器数据 Data:%d \n",CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
				break;
			}

			g_nPatrAlertCount++;
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [GPSCheck] 巡防管理服务器报警提示数据 Data:%d \n",CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
			SendPatroAlertToPubServer(pszDeviceCode,pszPatrAlert);	
		}
		break;
	default:
		break;
	}

	return TRUE;
}
/************************************************************************************************************************************************/
/*												Send Business
/************************************************************************************************************************************************/
void CBusinessManage_ConnectToGPSCheckServer::SendPatroAlertToPubServer(char *pszDeviceCode,const char* pszPatrAlert)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"%s,%.2X,%s,0",pszDeviceCode,c_APPCMD_SendMessage,gPGisMainCtrlProc.m_PatroAlertTextInfo);		
	//return	gPGisMainCtrlProc.m_BusinessManage_ConToPub.SendData(gPGisMainCtrlProc.g_dwConToPubContext,sendBuf,strlen(sendBuf),A_TYPE_APPCMD_TERMINAL);	//BUG PubServer 
	gPGisMainCtrlProc.m_BusinessManage_ConToPub.Broadcast_SendData(sendBuf,strlen(sendBuf),A_TYPE_APPCMD_TERMINAL,gPGisMainCtrlProc.g_dwConToPubContext,CBusinessManage_ConToPub::Pub_All);
}

BOOL CBusinessManage_ConnectToGPSCheckServer::SendSnycDataToGPSCheckServer(const char *SnycData,const int nDataLen)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"%s",SnycData);		
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GPSCheck] 发送同步数据给巡防管理服务,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
	return	SendData(sendBuf,strlen(sendBuf),F_TYPE_SNYCBROADCAST);
}

BOOL CBusinessManage_ConnectToGPSCheckServer::SendUpGPSDataToGPSCheckServer(const char *szDeviceCode,const char *szLongitude,const char *szLatitude)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"%s,%s,%s",szDeviceCode,szLongitude,szLatitude);
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GPSCheck] 发送GPS数据位置给巡防管理,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
	return SendData(sendBuf,strlen(sendBuf),F_TYPE_GPSUPATE_EVENT);
}

/************************************************************************************************************************************************/
/*												Persistent data
/************************************************************************************************************************************************/

BOOL CBusinessManage_ConnectToGPSCheckServer::ReportWebServer_LoadPatrolDeviceData()
{
	return gPGisMainCtrlProc.m_DBManage_WebServer.WebServer_LoadDB_PatrolDeviceInfo(FALSE);
}

/************************************************************************************************************************************************/
/*												Cache data
/************************************************************************************************************************************************/
BOOL CBusinessManage_ConnectToGPSCheckServer::FindDevicePatrol(const char *szDeviceNum)
{
	if (!m_PatrolDevListData.IsEmpty())
	{
		return m_PatrolDevListData.FindItem(szDeviceNum);
	}

	return FALSE;
}
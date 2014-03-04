#include "StdAfx.h"
#include "BusManage_ConnectToShanhaiServer.h"

#include <sstream>

#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;

CBusinessManage_ConnectToShanhaiServer::CBusinessManage_ConnectToShanhaiServer(void)
{
	m_IsSuceeseStart = FALSE;
	m_bConnShanhaiServer = FALSE;
	m_nIndexServer = 0;
}

CBusinessManage_ConnectToShanhaiServer::~CBusinessManage_ConnectToShanhaiServer(void)
{
	if (m_HashmapDataRecvBuffer.GetSize())
	{
		m_HashmapDataRecvBuffer.ClearAll();
	}
}


BOOL CBusinessManage_ConnectToShanhaiServer::Start(int conut)
{	
	m_nIndexServer = conut;
	if (!gPGisMainCtrlProc.m_iocpsocket.m_IsIOCPActive || !LoadConfig(conut) || !m_IsConnLocationServe)
	{
		return FALSE;
	}

	if (gPGisMainCtrlProc.m_iocpsocket.GTcpSvr_Connect(m_dwShanhaiServerContext,m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,0,(DWORD)this,
		&CBusinessManage_ConnectToShanhaiServer::OnConnectClt,
		&CBusinessManage_ConnectToShanhaiServer::OnReceiveClt,
		&CBusinessManage_ConnectToShanhaiServer::OnSendedClt,
		&CBusinessManage_ConnectToShanhaiServer::OnSendErrorClt,
		&CBusinessManage_ConnectToShanhaiServer::OnConnectError,
		&CBusinessManage_ConnectToShanhaiServer::OnDisconnectClt,
		&CBusinessManage_ConnectToShanhaiServer::OnHeartbeat,
		&CBusinessManage_ConnectToShanhaiServer::OnCreateClientClt,
		&CBusinessManage_ConnectToShanhaiServer::OnDestroyClientClt))
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s [%s]%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szSubscriptionID,m_szStartInfo);
		m_IsSuceeseStart = TRUE;
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] RemoteIP:%s RemotePort:%d LocalIP:%s [%s]%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szConRemoteIP,m_nConRemotePort,m_szLocalIP,m_szSubscriptionID,m_szErrorInfo);  
	}

	return m_IsSuceeseStart;
}

DWORD CBusinessManage_ConnectToShanhaiServer::GetContext()
{
	return m_dwShanhaiServerContext;
}

BOOL CBusinessManage_ConnectToShanhaiServer::LoadConfig(int nCount)
{
	Crwini rwIniTemp;
	CString strCount;
	strCount.Format("_%d",nCount);

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}
	if(NULL ==(m_IsConnLocationServe = rwIniTemp.GetInt(_T("ShanHaiServer" + strCount),_T("IsConnShanHaiServer"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_IsConnLocationServe = FALSE;
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("LocalIP"),gPGisMainCtrlProc.g_strConfigFile,m_szLocalIP)) 
	{
		strncpy(m_szLocalIP,"",sizeof(m_szLocalIP));
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("SHServerIP"),gPGisMainCtrlProc.g_strConfigFile,m_szConRemoteIP)) 
	{
		strncpy(m_szConRemoteIP,"",sizeof(m_szConRemoteIP));
	}
	if(NULL == (m_nConRemotePort = rwIniTemp.GetInt(_T("ShanHaiServer" + strCount),_T("SHServerPort"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_nConRemotePort = 7200;
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("LoginID"),gPGisMainCtrlProc.g_strConfigFile,m_strSHLoginID)) 
	{
		strncpy(m_szConRemoteIP,"gis",sizeof(m_strSHLoginID));
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("LoginPSWD"),gPGisMainCtrlProc.g_strConfigFile,m_strSHLoginPSWD)) 
	{
		strncpy(m_strSHLoginPSWD,"123456",sizeof(m_strSHLoginPSWD));
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("StartInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szStartInfo)) 
	{
		strncpy(m_szStartInfo,_T("服务启动成功!"),sizeof(m_szStartInfo));
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("ErrorInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szErrorInfo)) 
	{
		strncpy(m_szErrorInfo,_T("服务启动失败!"),sizeof(m_szErrorInfo));
	} 
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("SubscriptionID"),gPGisMainCtrlProc.g_strConfigFile,m_szSubscriptionID)) 
	{
		strncpy(m_szSubscriptionID,_T("ShanHai"),sizeof(m_szSubscriptionID));
	}
	if(NULL == rwIniTemp.ReadString(_T("ShanHaiServer" + strCount),_T("Subscription"),gPGisMainCtrlProc.g_strConfigFile,m_szSubscription)) 
	{
		strncpy(m_szSubscription,_T("ALL,0000"),sizeof(m_szSubscription));
	}

	return TRUE;
}

/****************************************************************************************************************************/
/*													CALLBACK BUSINESS														*/
/****************************************************************************************************************************/

void CBusinessManage_ConnectToShanhaiServer::OnConnectClt(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;
	//temp_this->SendDataToShanHai(dwClientContext,SH_TYPE_LOGIN,NULL,0);
	temp_this->m_bConnShanhaiServer = TRUE;
	temp_this->EventLogin(dwClientContext);
}

void CBusinessManage_ConnectToShanhaiServer::OnDisconnectClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToShanhaiServer::OnReceiveClt(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;

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
		TRACE(_T("[Shanhai][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[Shanhai][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

	int nPackageLen = 0;
	unsigned short us_LocationHead = 0xAAAA;
	char *ptempCurPoint = pCurPoint;

	while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
	{
		int nHeadPoint = -1;
		char *temppHeadCurPoint = pCurPoint;
		for (int i = 0;i<nSpaceLen;i++)
		{
			if(us_LocationHead != ((SH_Head*)pCurPoint)->us_Header) //检查包头
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
			TRACE(_T("[Shanhai][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [Shanhai][%s] 整包%s 截取后的包头错 %s,没有找到%2X\n",CTime::GetCurrentTime().Format("%H:%M:%S"),temp_this->m_szSubscriptionID,pRecvData,temppHeadCurPoint,us_LocationHead);
			nSpaceLen = 0;
			break;
		}

		char *ptempTailCurPoint = pCurPoint+2; //偏移包头
		int nTailPoint = 0;
		for(int i = 0 ; i < nSpaceLen; i++ )
		{
			if(us_LocationHead != ((SH_Head*)ptempTailCurPoint)->us_Header) //检查包头
			{
				++ptempTailCurPoint;
			}
			else
			{
				nTailPoint = i;
				break;
			}
			nTailPoint = i;	//没有找到包尾，表示为整体包
		}
		if( nTailPoint > 0 )
		{
			nPackageLen = nTailPoint+2; //偏移包头
			if( nPackageLen < MAX_DATA_PACKAGE_LEN )
			{
				temp_this->ProessAnalyseData(dwClientContext,pCurPoint,nPackageLen);
				TRACE(_T("[Shanhai][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+2,nPackageLen-2);
			}
			else
			{
				TRACE(_T("[Shanhai][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}

			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[Shanhai][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[Shanhai][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [Shanhai] 整包%s 截取后的包尾错 %s,没有找到%4x\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,us_LocationHead);
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
			TRACE(_T("[Shanhai][THREAD:%d][ID:%d] 没有处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
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

void CBusinessManage_ConnectToShanhaiServer::OnReceiveErrorClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToShanhaiServer::OnSendedClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;
	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [Shanhai][%s][ID:%d] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),temp_this->m_szSubscriptionID,dwClientContext,pBuf);
}

void CBusinessManage_ConnectToShanhaiServer::OnSendErrorClt(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [Shanhai][%s][ID:%d] SendErrorData:%s 发送错误退出 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),temp_this->m_szSubscriptionID,dwClientContext,pBuf);
}

void CBusinessManage_ConnectToShanhaiServer::OnHeartbeat(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;
	//temp_this->SendDataToShanHai(dwClientContext,SH_TYPE_TICK,NULL,0);
	temp_this->EventHeartBeat(dwClientContext);
}

void CBusinessManage_ConnectToShanhaiServer::OnConnectError(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_ConnectToShanhaiServer* temp_this = (CBusinessManage_ConnectToShanhaiServer*)dwInstaned;
	temp_this->m_bConnShanhaiServer = FALSE;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [Shanhai][%s][ID:%d] OnConnectError 继续重连\n",CTime::GetCurrentTime().Format("%H:%M:%S"),temp_this->m_szSubscriptionID,dwClientContext);
}

void CBusinessManage_ConnectToShanhaiServer::OnCreateClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConnectToShanhaiServer::OnDestroyClientClt(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

/****************************************************************************************************************************/
/*														DATA PROCESS														*/
/****************************************************************************************************************************/
BOOL CBusinessManage_ConnectToShanhaiServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	memcpy(tempBuf,RecvData,nRecvLen);

	if(tempBuf == NULL)
	{
		return FALSE;
	}

	if(nRecvLen < MIN_SHANHAI_PACKAGE_LEN)
	{
		return FALSE;
	}

	SH_Head* ptemp_head = (SH_Head*)tempBuf;
// 	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [Shanhai][%s][ID:%d] ProessAnalyseData:%X %X %X %X\n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szSubscriptionID,dwClientContext,
// 		ptemp_head->us_Header,ptemp_head->us_cmd,ptemp_head->us_version,ptemp_head->un_Len);

	ProessAnalyseData(dwClientContext,tempBuf+sizeof(SH_Head),nRecvLen-sizeof(SH_Head),ptemp_head->us_cmd);

	return TRUE;
}

BOOL CBusinessManage_ConnectToShanhaiServer::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, unsigned short us_CmdType)
{
	if(NULL == RecvData)
	{
		return FALSE;
	}

	switch(us_CmdType)
	{
	case SH_TYPE_LOGIN_ACK:
		{
			m_bConnShanhaiServer = TRUE;
			int tmp_usReslut;
			char tmp_sz[2] = {0};
			memcpy(tmp_sz,RecvData,2);
			tmp_usReslut = atoi(tmp_sz);
			char tmp_szMsg[50];
			memcpy(tmp_szMsg,RecvData+2,50);
			gPGisMainCtrlProc.m_SubscriptionManager.Shanhai_Subscription_Config(m_szSubscriptionID,m_szSubscription);
			gWriteLogApp.WriteLog(FOR_INFO,"%s [Shanhai][%s][ID:%d] 山海经登陆回应 登录标记:%X 登录信息:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szSubscriptionID,dwClientContext,tmp_usReslut,tmp_szMsg);
		}
		break;
	case SH_TYPE_TICK_ACK:
		EventHeartBeat(dwClientContext);
		gWriteLogApp.WriteLog(FOR_DEBUG,"%s [Shanhai][%s][ID:%d] 山海经心跳回应 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szSubscriptionID,dwClientContext);
		break;
	case SH_TYPE_DISCONNET: //断开
		GTcpClt_DisconnectClient(dwClientContext);
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [Shanhai][%s][ID:%d] 山海经通知退出 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szSubscriptionID,dwClientContext);
		break;
	default:
		break;
	}

	return TRUE;
}

// BOOL CBusinessManage_ConnectToShanhaiServer::SendDataToShanHai(const DWORD dwClientContext,int nType,const char *DataBuf,const DWORD dwBufLen)
// {
// 	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
// 	SH_TLoginFram *pLoginFram = NULL; 
// 	SH_TGpsFram *pSHGpsData = NULL;
// 	int nLen = 0;
// 	switch(nType)
// 	{
// 	case SH_TYPE_LOGIN: //发送登陆
// 		pLoginFram = (SH_TLoginFram *)sendBuf;
// 		pLoginFram->Head.wHeader = 0xAA;
// 		pLoginFram->Head.cmdFlag = 0xBB;
// 		sprintf(pLoginFram->UserId,"%s",m_strSHLoginID);
// 		sprintf(pLoginFram->UserPwd,"%s",m_strSHLoginPSWD);
// 		nLen = sizeof(SH_TLoginFram);
// 		break;
// 	case SH_TYPE_TICK:
// 		pLoginFram = (SH_TLoginFram *)sendBuf;
// 		pLoginFram->Head.wHeader = 0xAA;		 
// 		pLoginFram->Head.cmdFlag = 0xEE;//0xEE 检测报文标志; 
// 		sprintf(pLoginFram->UserId,"%s",m_strSHLoginID);
// 		sprintf(pLoginFram->UserPwd,"%s",m_strSHLoginPSWD);
// 		nLen = sizeof(SH_TLoginFram);
// 		break;
// 	case SH_TYPE_GPSDATA: //发送GPS数据
// 		{
// 			if(DataBuf == NULL)
// 				return FALSE;
// 
// 			pSHGpsData = (SH_TGpsFram *)sendBuf;
// 			pSHGpsData->Head.wHeader = 0xAA;
// 			pSHGpsData->Head.cmdFlag = 0xCC;
// 			memcpy(&pSHGpsData->GpsData,DataBuf,sizeof(SH_TGpsData));
// 			nLen = sizeof(SH_TGpsFram);
// 			break;
// 		}
// 	default:
// 		return FALSE;
// 		break;
// 	}
// 
// 	return	GCommProt_PostSendBuf(dwClientContext,sendBuf,nLen);
// }


BOOL CBusinessManage_ConnectToShanhaiServer::EventLogin(const DWORD dwClientContext)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN];
	ZeroMemory(sendBuf,MAX_DATA_PACKAGE_LEN);

	SH_LoginData *pLoginData = NULL; 
	pLoginData = (SH_LoginData *)sendBuf;
	pLoginData->Head.us_Header = SH_HEAD;
	pLoginData->Head.us_cmd= SH_TYPE_LOGIN;
	pLoginData->Head.us_version = SH_VERION;
	pLoginData->Head.un_Len = /*sizeof(SH_LoginData)*/ 0x32;
//	strncpy(pLoginData->UserId,m_strSHLoginID,sizeof(m_strSHLoginID));
	strncpy(pLoginData->UserPwd,m_strSHLoginPSWD,sizeof(m_strSHLoginPSWD));
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,sizeof(SH_LoginData));	
}

BOOL CBusinessManage_ConnectToShanhaiServer::EventHeartBeat(const DWORD dwClientContext)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN];
	ZeroMemory(sendBuf,MAX_DATA_PACKAGE_LEN);

	SH_Head *pEventDate = NULL; 
	pEventDate = (SH_Head *)sendBuf;
	pEventDate->us_Header = SH_HEAD;
	pEventDate->us_cmd= SH_TYPE_TICK;
	pEventDate->us_version = SH_VERION;
	pEventDate->un_Len = /*sizeof(SH_LoginData)*/0;
//	strncpy(pLoginData->UserId,m_strSHLoginID,sizeof(m_strSHLoginID));
//	strncpy(pLoginData->UserPwd,m_strSHLoginPSWD,sizeof(m_strSHLoginPSWD));
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,sizeof(SH_LoginData));	
}

BOOL CBusinessManage_ConnectToShanhaiServer::SendGPSData(const char *DataBuf,const DWORD dwBufLen)
{	
	if(DataBuf == NULL)
		return FALSE;

	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [Shanhai][%s][SendGPSData] DataBuf：%s dwBufLen:%d \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szSubscriptionID,DataBuf,dwBufLen);

	char sendBuf[MAX_DATA_PACKAGE_LEN];
	ZeroMemory(sendBuf,MAX_DATA_PACKAGE_LEN);

	SH_GpsData *pGpsData = NULL; 
	pGpsData = (SH_GpsData *)sendBuf;
	pGpsData->Head.us_Header = SH_HEAD;
	pGpsData->Head.us_cmd= SH_TYPE_GPSDATA;
	pGpsData->Head.us_version = SH_VERION;
	pGpsData->Head.un_Len = htonl(sizeof(SH_GpsData));
	memcpy(&pGpsData->Gpsdata,DataBuf,sizeof(SH_Gps));
	return GCommProt_PostSendBuf(m_dwShanhaiServerContext,sendBuf,sizeof(SH_GpsData));	
}
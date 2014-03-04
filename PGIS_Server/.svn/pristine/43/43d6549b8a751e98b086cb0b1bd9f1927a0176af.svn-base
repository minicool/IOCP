#include "StdAfx.h"
#include "BusManage_ConToPub.h"

#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;


CBusinessManage_ConToPub::CBusinessManage_ConToPub(void)
{
	m_bConnPubServer = FALSE;
}

CBusinessManage_ConToPub::~CBusinessManage_ConToPub(void)
{
	m_HashmapDataInfo.ClearAll();
	m_HashmapDataRecvBuffer.ClearAll();
}

BOOL CBusinessManage_ConToPub::Start()
{	
	if (!gPGisMainCtrlProc.m_iocpsocket.m_IsIOCPActive || !LoadConfig())
	{
		return FALSE;
	}

	if (gPGisMainCtrlProc.m_iocpsocket.GTcpSvr_OpenListen(m_dwConToPubServerContext,m_szLocalIP,m_nLocalPort,0,(DWORD)this,
		&CBusinessManage_ConToPub::OnConnectSvr,
		&CBusinessManage_ConToPub::OnReceiveSvr,
		&CBusinessManage_ConToPub::OnSendedSvr,
		&CBusinessManage_ConToPub::OnDisconnectSvr,
		&CBusinessManage_ConToPub::OnSendErrorSvr,
		&CBusinessManage_ConToPub::OnConnectionOverflow,
		&CBusinessManage_ConToPub::OnIdleOvertime,
		&CBusinessManage_ConToPub::OnCreateClientSvr,
		&CBusinessManage_ConToPub::OnDestroyClientSvr))
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szLocalIP,m_nLocalPort,m_szStartInfo);
		m_IsSuceeseStart = TRUE;
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_INFO,"%s [SYS] LocalIP:%s LocalPort:%d %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szLocalIP,m_nLocalPort,m_szErrorInfo);  
	}

	return m_IsSuceeseStart;
}

DWORD CBusinessManage_ConToPub::GetContext()
{
	return m_dwConToPubServerContext;
}

BOOL CBusinessManage_ConToPub::LoadConfig()
{
	Crwini rwIniTemp;

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}	

	if(NULL == rwIniTemp.ReadString(_T("ConToPubServer"),_T("LocalIP"),gPGisMainCtrlProc.g_strConfigFile,m_szLocalIP)) 
	{
		strncpy(m_szLocalIP,"",sizeof(m_szLocalIP));
	}
	if(NULL == (m_nLocalPort = rwIniTemp.GetInt(_T("ConToPubServer"),_T("LocalPort"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_nLocalPort = 9003;
	}
	if(NULL == rwIniTemp.ReadString(_T("ConToPubServer"),_T("StartInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szStartInfo)) 
	{
		strncpy(m_szStartInfo,_T("服务启动成功!"),sizeof(m_szStartInfo));
	}
	if(NULL == rwIniTemp.ReadString(_T("ConToPubServer"),_T("ErrorInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szErrorInfo)) 
	{
		strncpy(m_szErrorInfo,_T("服务启动失败!"),sizeof(m_szErrorInfo));
	} 

	return TRUE;
}

/****************************************************************************************************************************/
/*													CALLBACK BUSINESS														*/
/****************************************************************************************************************************/

void CBusinessManage_ConToPub::OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	TRACE("CBaseBusinessManage::OnConnectSvr \n");
	CBusinessManage_ConToPub* temp_this = (CBusinessManage_ConToPub*)dwInstaned;
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub][ID:%d] 接入至分发服务器成功 \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
	temp_this->m_bConnPubServer = TRUE;
	OnReceiveSvr(dwClientContext,pBuf,dwBytes,dwInstaned);
}

void CBusinessManage_ConToPub::OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub][ID:%d] OnDisconnectSvr \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
	CBusinessManage_ConToPub* temp_this = (CBusinessManage_ConToPub*)dwInstaned;
	temp_this->m_bConnPubServer = FALSE;
}

void CBusinessManage_ConToPub::OnReceiveSvr(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	TRACE(_T("[ConToPub][THREAD:%d] 整体包长为 %d %s \n"),GetCurrentThreadId(),nRecvLen,pRecvData);
	CBusinessManage_ConToPub* temp_this = (CBusinessManage_ConToPub*)dwInstaned;

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
		TRACE(_T("[ConToPub][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[ConToPub][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

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
			TRACE(_T("[ConToPub][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] 整包%s 截取后的包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,m_strHead);
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
				TRACE(_T("[ConToPub][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
			}
			else
			{
				TRACE(_T("[ConToPub][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[ConToPub][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[ConToPub][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s\n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] 整包%s 截取后的包尾错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,m_strTail);
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
			TRACE(_T("[ConToPub][THREAD:%d] 没有处理的数据包长为%d %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
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

void CBusinessManage_ConToPub::OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] CON->PUB] OnReceiveErrorSvr \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_ConToPub::OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [ConToPub] [CON->PUB] OnSendedSvr:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConToPub::OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] [CON->PUB] OnSendErrorSvr:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_ConToPub::OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] [CON->PUB] OnIdleOvertime \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_ConToPub::OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [ConToPub] [CON->PUB] OnConnectionOverflow \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_ConToPub::OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_ConToPub::OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CBusinessManage_ConToPub::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
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
	sscanf(tempBuf, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype

	if(nTempLen != nRecvLen -6)
	{
		return FALSE;
	}

	ProessAnalyseData(dwClientContext,tempBuf+6,nTempLen,nCmdType);

	return TRUE;
}

BOOL CBusinessManage_ConToPub::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, int nCmdType)
{
	if(RecvData == NULL)
	{
		return FALSE;
	}

	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,dwRecvLen);
	char *pBuf = tempBuf;

	char *pData[3] = {0};
	strtok(pBuf,",");
	pData[0] = pBuf;
	pData[1] = strtok(NULL,",");
	pData[2] = strtok(NULL,",");

	BOOL bRes = TRUE;
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	switch(nCmdType)
	{
	case A_TYPE_LOGIN: //登陆
		DispatchAckToAnswer(dwClientContext,A_TYPE_LOGIN_ACK);
		bRes = FALSE;
		break;
	case A_TYPE_TICK: //握手
		DispatchAckToAnswer(dwClientContext,A_TYPE_TICK);
		bRes = FALSE;
		break;
	case A_TYPE_TERMINAL_LINE: //终端公网（GPRS/CDMA）上下线协议	写上下线数据信息
		SetPubTerminal_Line(RecvData,dwRecvLen);
		bRes = FALSE;
		break;
	case A_TYPE_TERMINAL_GPS: //终端上传GPS协议
		{
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [PubTer]接收终端上传GPS数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
			gPGisMainCtrlProc.m_DBManage_WebServer.ReportWebServiceRecvGps(GPS_LOCATION_GIS,RecvData,dwRecvLen);				//GPS数据 写数据库	0xFF LOCATION位置查询 群发数据
			//bRes = FALSE;
			//SendData_Broadcast_GisClient_TerminalGpsData(RecvData,dwRecvLen);
		}
		break;
	case A_TYPE_APPCMD_TERMINAL:
		{
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [PubNet]应用命令应答:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
			TERMINAL_INFO temp_TermainalInfo;
			if (bRes && m_HashmapDataInfo.FindItem(pData[0],temp_TermainalInfo))
			{
				SendData_Broadcast_GisClient_TerminalTxtMsgAnswerTo(temp_TermainalInfo.szDeptNum,RecvData,dwRecvLen);
			}
			bRes = FALSE;
		}
		break;
	case A_TYPE_TERMINALNEW_GPS:
		{
			gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [New_PubNet]接收终端上传GPS数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);

			string temp_str = RecvData;
			std::vector<std::string> temp_vector;
			temp_vector = gPGisMainCtrlProc.m_SubscriptionManager.split(temp_str,",");

			sprintf(sendBuf,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,00800000",
				temp_vector[0].c_str(),temp_vector[1].c_str(),temp_vector[3].c_str(),temp_vector[4].c_str(),temp_vector[5].c_str(),temp_vector[6].c_str(),temp_vector[7].c_str(),temp_vector[8].c_str(),temp_vector[9].c_str(),temp_vector[10].c_str(),temp_vector[11].c_str());
			gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [New_PubNet]接收终端上传GPS数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
			gPGisMainCtrlProc.m_DBManage_WebServer.ReportWebServiceRecvGps(GPS_LOCATION_GIS,sendBuf,strlen(sendBuf));				//GPS数据 写数据库	0xFF LOCATION位置查询 群发数据
			//bRes = FALSE;
		}
		break;
		//     case A_TYPE_CTRLCMD_TERMINAL_ACK: //终端上传应答控制命令协议
		//         strTrace.Format(_T("%s [PubNet->]终端上传应答控制命令:%s\n"),strCurrTime,RecvData); 
		//         gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		//         break;
		//     case A_TYPE_TERMINAL_ALERT://终端报警协议
		//         strTrace.Format(_T("%s [PubNet->]终端报警数据:%s\n"),strCurrTime,RecvData);
		//         gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		//         break;
		//     case A_TYPE_SETCMD_TERMINAL_ACK: //终端应答参数设置协议
		//         strTrace.Format(_T("%s [PubNet->]终端应答参数设置数据:%s\n"),strCurrTime,RecvData);
		//         gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		//         break;
		//     case A_TYPE_QUERYCFG_TERMINAL_ACK: //终端应答参数读取协议
		//         strTrace.Format(_T("%s [PubNet->]终端应答参数读取数据:%s\n"),strCurrTime,RecvData);
		//         gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		//         break;

	case A_TYPE_QUERY_MY_LOCATION: //终端查询我的位置
		gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [PubTer]终端查询我的位置数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
		gPGisMainCtrlProc.m_BusinessManage_ConnectToLocationServer.QueryLocationProcess(dwClientContext,RecvData,dwRecvLen);	//终端查询我的位置处理，将数据发给地理位置服务器
		bRes = FALSE;
		break;
	}
	
	TERMINAL_INFO temp_TermainalInfo;
	if (bRes && m_HashmapDataInfo.FindItem(pData[0],temp_TermainalInfo))
	{
		gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(RecvData,dwRecvLen,nCmdType,DWORD(&temp_TermainalInfo),pData[2],CBusinessManage_GISTerminal::/*GISTer_DeptNum_Char*/GISTer_Subscription);	//广播所有GIS用户
	}
	return TRUE;
}

BOOL CBusinessManage_ConToPub::SendData(DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",strlen(RecvData),nCmdType,RecvData);
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_ConToPub::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s,%s,NULL","00000000","00");
	return SendData(dwClientContext,sendBuf,strlen(sendBuf),nCmdType);
}

void CBusinessManage_ConToPub::Update_OnlineDeviceList_PatrolState(CString strNumber)
{	
	TERMINAL_INFO temp_HashmapDatainfo;
	m_HashmapDataInfo.FindItem(strNumber.GetBuffer(),temp_HashmapDatainfo);
	temp_HashmapDatainfo.bPatrolStart = TRUE;
	m_HashmapDataInfo.AddItem(strNumber.GetBuffer(),temp_HashmapDatainfo);
}

/************************************************************************************************************************************************/
/*												Persistent data
/************************************************************************************************************************************************/
BOOL CBusinessManage_ConToPub::FindPubTerminal(const char *szDeviceNum)
{
	if (!m_HashmapDataInfo.IsEmpty())
	{
		return m_HashmapDataInfo.Find(szDeviceNum);
	}

	return FALSE;
}

void CBusinessManage_ConToPub::AddPubTerminal(const char *pszDeviceNum,const char *pszDeviceType,const char *pszDeptNum,const char *pszUserCode,int nUserType,BOOL bLineState,BOOL bPatrolStart)
{
	TERMINAL_INFO temp_TermainalInfo;
	ZeroMemory(&temp_TermainalInfo,sizeof(TERMINAL_INFO));
	strncpy(temp_TermainalInfo.szDeviceNum,pszDeviceNum,strlen(pszDeviceNum));
	strncpy(temp_TermainalInfo.szDeviceType,pszDeviceType,strlen(pszDeviceType));
	strncpy(temp_TermainalInfo.szDeptNum,pszDeptNum,strlen(pszDeptNum));
	strncpy(temp_TermainalInfo.szUserCode,pszUserCode,strlen(pszUserCode));
	temp_TermainalInfo.nLineState = bLineState;
	temp_TermainalInfo.nUserType = 0;
	temp_TermainalInfo.bQueryInfoOk = FALSE;
	temp_TermainalInfo.nQueryFailCount = 0;
	temp_TermainalInfo.bPatrolStart = bPatrolStart;
	m_HashmapDataInfo.AddItem(pszDeviceNum,temp_TermainalInfo);
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [PubTer] DeviceNum:%s DeviceType:%s DeptNum:%s UserCode:%s LineState:%d PatrolStart:%d Count:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
		pszDeviceNum,pszDeviceType,pszDeptNum,pszUserCode,bLineState,bPatrolStart,m_HashmapDataInfo.GetSize());
}

BOOL CBusinessManage_ConToPub::DelPubTerminal(const char *pszDeviceNum)
{
	if (!m_HashmapDataInfo.IsEmpty())
	{
		gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [PubTer] DeviceNum:%s Count:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),pszDeviceNum,m_HashmapDataInfo.GetSize());
		return m_HashmapDataInfo.Delete(pszDeviceNum);
	}

	return FALSE;
}

void CBusinessManage_ConToPub::ShowOnLineDeviceNum(void)
{
	gWriteLogApp.WriteLog(FOR_INFO,_T("%s [SYS] 在线终端信息 总连接数:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_HashmapDataInfo.GetSize());
}

void CBusinessManage_ConToPub::ShowOnLineDeviceInfo(void)
{
	m_HashmapDataInfo.TraversalItem((DWORD)(this),NULL,GetOnLineDeviceInfo);
	ShowOnLineDeviceNum();
}

BOOL CBusinessManage_ConToPub::GetOnLineDeviceInfo(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	CBusinessManage_ConToPub* temp_this = (CBusinessManage_ConToPub*)dwInstaned;
	PTERMINAL_INFO ptemp_datainfo = (PTERMINAL_INFO)dwDataContext;
	gWriteLogApp.WriteLog(FOR_INFO,_T("%s [SYS] 在线终端信息 DeviceNum:%s DeviceType:%s DeptNum:%s UserCode:%s UserType:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
		ptemp_datainfo->szDeviceNum,ptemp_datainfo->szDeviceType,ptemp_datainfo->szDeptNum,ptemp_datainfo->szUserCode,ptemp_datainfo->nUserType);
	return FALSE;
}

/************************************************************************************************************************************************/
/*																	BUSSINESS EVENT MANGER														*/
/************************************************************************************************************************************************/
// BOOL CBusinessManage_ConToPub::AnalysGpsData(const char *RecvData, int nLen)
// {
// 	char *pData[16];
// 	for(int i = 0; i < 16; i++)
// 	{
// 		pData[i] = NULL;
// 	}
// 
// 	char temp[MAX_DATA_PACKAGE_LEN] ={0};
// 	memcpy(temp,RecvData,nLen);
// 
// 	char *pBuf = temp;
// 	int i = 0;
// 	while((pData[i]=strtok(pBuf,",")) != NULL)
// 	{
// 		i++;
// 		pBuf = NULL;
// 	}
// 	if(i != 12)
// 	{
// 		return FALSE ;
// 	}
// }

void CBusinessManage_ConToPub::SendData_Broadcast_GisClient_TerminalTxtMsgAnswerTo(char *pszDeptNum,const char *RecvData, int nReturnIndex)
{
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS]向GIS客户端广播发送文字信息应答消息,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),RecvData);
	gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(RecvData,strlen(RecvData),A_TYPE_APPCMD_TERMINAL,NULL,pszDeptNum,CBusinessManage_GISTerminal::GISTer_DeptNum_Char);	
}

//GPS数据发给GIS客户端
void CBusinessManage_ConToPub::SendData_Broadcast_GisClient_TerminalGpsData(const char *RecvData, int nReturnIndex)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] ={0};
	sprintf(sendBuf,"%s,%d",RecvData,nReturnIndex);

	string temp_str = RecvData;
	std::vector<std::string> temp_vector;
	temp_vector = gPGisMainCtrlProc.m_SubscriptionManager.split(temp_str,",");	

	TERMINAL_INFO temp_TermainalInfo;
	if (m_HashmapDataInfo.FindItem(temp_vector[0],temp_TermainalInfo))
	{
		gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(sendBuf,strlen(sendBuf),C_TYPE_TERMINAL_GPS,DWORD(&temp_TermainalInfo),"",CBusinessManage_GISTerminal::GISTer_Subscription);	//广播所有GIS用户
	}
}

BOOL CBusinessManage_ConToPub::SetPubTerminal_Line(const char *RecvData, int nLen)
{
	char TempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	memcpy(TempBuf,RecvData,nLen);

	strtok(TempBuf,",");
	char *pStrDeviceNum = TempBuf; 
	char *pStrType = strtok(NULL,",");
	char *pStrLineState = strtok(NULL,",");

	if( pStrDeviceNum == NULL || pStrType == NULL || pStrLineState == NULL )
	{
		return FALSE;
	}

	BOOL nLineState = atoi(pStrLineState);
	BOOL bPatrolFlag = FALSE;
	BOOL bLoginFlag = FindPubTerminal(pStrDeviceNum);

	if( nLineState > 0 ) //上线
	{
		bPatrolFlag = gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.FindDevicePatrol(pStrDeviceNum);		//查询巡防 处于巡防列表	
		AddPubTerminal(pStrDeviceNum,pStrType,"0000","",0,nLineState,bPatrolFlag);
	}
	else //下线
	{
		DelPubTerminal(pStrDeviceNum);
	}

	gPGisMainCtrlProc.m_DBManage_WebServer.ReportWebServiceTerminalLine(pStrDeviceNum,nLineState);
	SendData_Broadcast_GisClient_TerminalLine(pStrDeviceNum,pStrType,nLineState);
	return TRUE;
}

void CBusinessManage_ConToPub::SendData_Broadcast_GisClient_TerminalLine(const char *strDrivceNum,const char *strDrivceType,int lineState)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] ={0};
	sprintf(sendBuf,"%s,%s,%d",strDrivceNum,strDrivceType,lineState);
	gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast(sendBuf,strlen(sendBuf),C_TYPE_PRIVATENET_LINE,0,CBusinessManage_GISTerminal::GisTer_All);
	gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [GIS][SendData_Broadcast_GisClient_TerminalLine] sendBuf:%s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);

// 	Sleep(1000);
// 	TERMINAL_INFO temp_TermainalInfo;
// 	if (m_HashmapDataInfo.FindItem(strDrivceNum,temp_TermainalInfo))
// 	{
// 		gPGisMainCtrlProc.m_BusinessManage_GISTerminal.SendData_Broadcast_char(sendBuf,strlen(sendBuf),C_TYPE_PRIVATENET_LINE,DWORD(&temp_TermainalInfo),"",CBusinessManage_GISTerminal::GISTer_Subscription);	//广播所有GIS用户
// 		gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [GIS][SendData_Broadcast_GisClient_TerminalLine] sendBuf:%s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);
// 	}
}

BOOL CBusinessManage_ConToPub::Pub_All(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext)
{	
	CBusinessManage_ConToPub* pInstance = (CBusinessManage_ConToPub*)dwInstance;
	DWORD dwtemp = GHndDat_GetOwner(dwClientContext);
	
	BOOL bRessult;
	if (GHndDat_GetOwner(dwClientContext) == dwSrcClientContext)
	{
		bRessult = TRUE;
	}else
	{
		bRessult = FALSE;
	}
	return bRessult;
}

void CBusinessManage_ConToPub::Broadcast_SendData(const char* pBuf,const DWORD dwBytes,const int nCmdType,const DWORD dwClientContext,PFN_ON_GSOCK_TRAVERSAL pfnTravesal)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s%.4X%.2X%s#","*HITOM",dwBytes,nCmdType,pBuf);

#if(_TIME)
	LARGE_INTEGER m_nFreq;  
	LARGE_INTEGER m_nBeginTime;  
	LARGE_INTEGER nEndTime;  
	if (!QueryPerformanceFrequency(&m_nFreq))	// 获取时钟周期 
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] 获取时钟周期 失败 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"));			
	}

	if (!QueryPerformanceCounter(&m_nBeginTime))	// 获取时钟计数 
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] 获取时钟计数 失败 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"));			
	}
#endif

	GTcpSvr_PostBroadcastBuf(sendBuf,strlen(sendBuf),(DWORD)this,dwClientContext,pfnTravesal);
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][GTcpSvr_PostBroadcastBuf] sendBuf:%s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),sendBuf);

#if(_TIME)
	if (!QueryPerformanceCounter(&nEndTime))	// 获取时钟计数 
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] 获取时钟计数 失败 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"));			
	}
	gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] [Broadcast_SendData] 时间精准差 %f ms 成功 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),(double)(nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart);
#endif
}


#include "StdAfx.h"
#include "BusManage_GISTerminal.h"

#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;

CBusinessManage_GISTerminal::CBusinessManage_GISTerminal(void)
{
	m_IsSuceeseStart = FALSE;
}

CBusinessManage_GISTerminal::~CBusinessManage_GISTerminal(void)
{
	m_HashmapDataGISTerminal.ClearAll();
	m_HashmapDataGISLogin.ClearAll();
}

BOOL CBusinessManage_GISTerminal::Start()
{	
	if (!gPGisMainCtrlProc.m_iocpsocket.m_IsIOCPActive || !LoadConfig())
	{
		return FALSE;
	}

	if (gPGisMainCtrlProc.m_iocpsocket.GTcpSvr_OpenListen(m_dwGisServerContext,m_szLocalIP,m_nLocalPort,0,(DWORD)this,
		&CBusinessManage_GISTerminal::OnConnectSvr,
		&CBusinessManage_GISTerminal::OnReceiveSvr,
		&CBusinessManage_GISTerminal::OnSendedSvr,
		&CBusinessManage_GISTerminal::OnDisconnectSvr,
		&CBusinessManage_GISTerminal::OnSendErrorSvr,
		&CBusinessManage_GISTerminal::OnConnectionOverflow,
		&CBusinessManage_GISTerminal::OnIdleOvertime,
		&CBusinessManage_GISTerminal::OnCreateClientSvr,
		&CBusinessManage_GISTerminal::OnDestroyClientSvr))
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

DWORD CBusinessManage_GISTerminal::GetContext()
{
	return m_dwGisServerContext;
}

BOOL CBusinessManage_GISTerminal::LoadConfig()
{
	Crwini rwIniTemp;

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}	

	if(NULL == rwIniTemp.ReadString(_T("GISServer"),_T("LocalIP"),gPGisMainCtrlProc.g_strConfigFile,m_szLocalIP)) 
	{
		strncpy(m_szLocalIP,"",sizeof(m_szLocalIP));
	}
	if(NULL == (m_nLocalPort = rwIniTemp.GetInt(_T("GISServer"),_T("LocalPort"),gPGisMainCtrlProc.g_strConfigFile)))
	{
		m_nLocalPort = 9000;
	}
	if(NULL == rwIniTemp.ReadString(_T("GISServer"),_T("StartInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szStartInfo)) 
	{
		strncpy(m_szStartInfo,_T("服务启动成功!"),sizeof(m_szStartInfo));
	}
	if(NULL == rwIniTemp.ReadString(_T("GISServer"),_T("ErrorInfo"),gPGisMainCtrlProc.g_strConfigFile,m_szErrorInfo)) 
	{
		strncpy(m_szErrorInfo,_T("服务启动失败!"),sizeof(m_szErrorInfo));
	} 

	return TRUE;
}

/****************************************************************************************************************************/
/*													CALLBACK BUSINESS														*/
/****************************************************************************************************************************/

void CBusinessManage_GISTerminal::OnConnectSvr(const DWORD dwClientContext, const char* pBuf, const DWORD dwBytes, const DWORD dwInstaned)
{
	OnReceiveSvr(dwClientContext,pBuf,dwBytes,dwInstaned);
// 	CBusinessManage_GISTerminal* temp_this = (CBusinessManage_GISTerminal*)dwInstaned;
// 	temp_this->GisClientLoginSuccessProcess(dwClientContext);
}

void CBusinessManage_GISTerminal::OnDisconnectSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] [GISTerminal] OnDisconnectSvr \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_GISTerminal::OnReceiveSvr(const DWORD dwClientContext, const char* pRecvData, const DWORD nRecvLen, const DWORD dwInstaned)
{
	TRACE(_T("[GISTerminal][THREAD:%d] 整体包长为 %s %d \n"),GetCurrentThreadId(),pRecvData,nRecvLen);
	CBusinessManage_GISTerminal* temp_this = (CBusinessManage_GISTerminal*)dwInstaned;

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
		TRACE(_T("[GISTerminal][THREAD:%d][ID:%d] ERROR 处理数据包超长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);		
		return /*FALSE*/;
	}

	memcpy(temprecvbufferinfo.RecvBuf+temprecvbufferinfo.nBufLenCount,pRecvData,nRecvLen);
	temprecvbufferinfo.nBufLenCount = temprecvbufferinfo.nBufLenCount + nRecvLen;
	int nSpaceLen = temprecvbufferinfo.nBufLenCount;
	char *pCurPoint = temprecvbufferinfo.RecvBuf;
	TRACE(_T("[GISTerminal][THREAD:%d][ID:%d] 需要处理的数据包长为 %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);

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
			TRACE(_T("[GISTerminal][THREAD:%d][ID:%d] 包头错:len:%d,recv:%s \n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [GISTerminal] 整包%s 截取后的包头错 %s,没有找到%X \n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,temppHeadCurPoint,PACKAGE_HEAD_DATA_STX);
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
				TRACE(_T("[GISTerminal][THREAD:%d][ID:%d] 完整包长为%s %d \n"),GetCurrentThreadId(),dwClientContext,pCurPoint+2,nPackageLen-2);
			}
			else
			{
				TRACE(_T("[GISTerminal][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s \n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[GISTerminal][THREAD:%d][ID:%d]包长度超过允许的最大值:len:%d,recv:%s \n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pRecvData);
			}
			else
			{
				TRACE(_T("[GISTerminal][THREAD:%d][ID:%d]没有找到包尾:len:%d,recv:%s \n"),GetCurrentThreadId(),dwClientContext,nSpaceLen,pCurPoint);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [GISTerminal] 整包%s 截取后的包尾错 %s,没有找到%X \n",CTime::GetCurrentTime().Format("%H:%M:%S"),pRecvData,ptempTailCurPoint,PACKAGE_TAIL_DATA_ETX);
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
			TRACE(_T("[GISTerminal][THREAD:%d][ID:%d] 没有处理的数据包长为%d %s %d \n"),GetCurrentThreadId(),dwClientContext,temprecvbufferinfo.RecvBuf,temprecvbufferinfo.nBufLenCount);
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

void CBusinessManage_GISTerminal::OnReceiveErrorSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] [GISTerminal] OnReceiveErrorSvr \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_GISTerminal::OnSendedSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_DEBUG,"%s [SYS] [ID:%d] [GISTerminal] OnSendedSvr:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_GISTerminal::OnSendErrorSvr(const DWORD dwClientContext, const char* pBuf, const DWORD nBytes, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] [GISTerminal] OnSendErrorSvr:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,pBuf);
}

void CBusinessManage_GISTerminal::OnIdleOvertime(const DWORD dwClientContext, const DWORD dwInstaned)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] [GISTerminal] OnIdleOvertime \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_GISTerminal::OnConnectionOverflow(const DWORD dwClientContext, const DWORD dwInstaned)
{
	CBusinessManage_GISTerminal* temp_this = (CBusinessManage_GISTerminal*)dwInstaned;
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"1,123");
	temp_this->SendData(dwClientContext,sendBuf,strlen(sendBuf),C_TYPE_LOGIN_ACK);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] [GISTerminal] OnConnectionOverflow \n",CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);	
}

void CBusinessManage_GISTerminal::OnCreateClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

void CBusinessManage_GISTerminal::OnDestroyClientSvr(const DWORD dwClientContext, const DWORD dwInstaned)
{

}

//--------------------------------BUSINESS-----------------------------------------------------------------------------------------
BOOL CBusinessManage_GISTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD nRecvLen)
{
	char tempBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(tempBuf,RecvData,nRecvLen);

	if(tempBuf == NULL)
	{
		return FALSE;
	}

// 	if(nRecvLen < MAIN_DATA_PACKAGE_LEN)
// 	{
// 		return FALSE;
// 	}

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(tempBuf, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype

	if((nTempLen -2) != nRecvLen)
	{
		return FALSE;
	}

	ProessAnalyseData(dwClientContext,tempBuf+6,nRecvLen-6,nCmdType);

	return TRUE;
}

BOOL CBusinessManage_GISTerminal::ProessAnalyseData(const DWORD dwClientContext, const char* RecvData, const DWORD dwRecvLen, int nCmdType)
{
	if(RecvData == NULL)
	{
		return FALSE;
	}

	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	BOOL bTranstion = TRUE;
	switch(nCmdType)
	{
		case C_TYPE_LOGIN:
			if(CheckGisClientLoginProcess(dwClientContext, RecvData))
			{
				EventLogin_DispatchAckToAnswer(dwClientContext);
			}
			GisClientLoginSuccessProcess(dwClientContext);
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] 接收GIS客户端登录数据:%s\n"), CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,RecvData);
			bTranstion = FALSE;
			break;
		case C_TYPE_TICK :
			EventHeartBeat_DispatchAckToAnswer(dwClientContext);
			bTranstion = FALSE;
			break;
		case C_TYPE_SYNC_REPORT: //同步
			GisClientSystemSnycProcess(dwClientContext,RecvData,dwRecvLen);
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] 接收GIS客户端同步数据:%s\n"), CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,RecvData);
			break;
		case C_TYPE_APPCMD_TERMINAL: //下发应用命令给终端协议
			nCmdType = A_TYPE_APPCMD_TERMINAL;

// 			if(atoi(pData[1]) == c_APPCMD_SendMessage) //发送文字信息
// 			{
// 				sprintf(sendBuf,"%s,%.2X,%s,%s",pData[0],c_APPCMD_SendMessage,pData[2],pData[3]);   //change by kevin 2009-12-22 将文字信息ID加在协议后发给了主控，主控来控制发送失败重新发机制 
// 				if(gPGisMainCtrlProc.m_BusinessManage_ConToPub.SendData(dwClientContext,sendBuf,dwRecvLen,nCmdType))
// 				{
// 					ReportWebSendMessage(atoi(pData[3]),pData[0]);									//通知WebService将GIS客户端发送的文字信息写入数据库
// 					sendBuf[nLen - 1] = NULL;
// 					gWriteLogApp.WriteLog(FOR_TRACE_1,_T("%s [PubNet<-]%s,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),_T("向公网服务器发送应用命令"),sendBuf+1);
// 					return TRUE;
// 				}    
// 			}
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] 接收GIS客户端应用命令数据:%s\n"), CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,RecvData);
			break;
		case C_TYPE_CTRLCMD_TERMINAL://下发控制命令协议
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] 接收GIS客户端发控制命令数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,RecvData);
			break;
		case C_TYPE_TERMINAL_ALERT_ACK: //下发应答和解除终端报警协议
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] 接收GIS客户端发应答和解除终端报警数据:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,RecvData);
			break;
		case C_TYPE_SUBSCRIPTION_ON:
			gPGisMainCtrlProc.m_SubscriptionManager.AddSubscription(RecvData,dwRecvLen);
			break;
		case C_TYPE_SUBSCRIPTION_OFF:
			gPGisMainCtrlProc.m_SubscriptionManager.DelSubscription(RecvData,dwRecvLen);
			break;
		case C_TYPE_QUERY_MY_LOCATION:
			gPGisMainCtrlProc.m_BusinessManage_ConnectToLocationServer.QueryLocationProcess(dwClientContext,RecvData,dwRecvLen);
			break;
		default:
			break;
	}
	
	if (bTranstion)
	{
		gPGisMainCtrlProc.m_BusinessManage_ConToPub.Broadcast_SendData(RecvData,dwRecvLen,nCmdType,gPGisMainCtrlProc.g_dwConToPubContext,CBusinessManage_ConToPub::Pub_All);
	}

	return TRUE;
}

BOOL CBusinessManage_GISTerminal::SendData(DWORD dwClientContext, const char* pszSendData, const DWORD nSendLen, int nCmdType)
{
	if ((nSendLen + 8)>SEND_DATA_PACKAGE_LEN)
	{
		return FALSE;
	}
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"%.4X%.2X%s",nSendLen+8,nCmdType,pszSendData);
	sendBuf[nSendLen + 7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));
}

BOOL CBusinessManage_GISTerminal::DispatchAckToAnswer(DWORD dwClientContext, int nCmdType)
{
	char sendBuf[512] = {0};
	sprintf(sendBuf,"%s,%s,NULL","00000000","00");
	return SendData(dwClientContext,sendBuf,strlen(sendBuf),nCmdType);
}

BOOL CBusinessManage_GISTerminal::EventHeartBeat_DispatchAckToAnswer(DWORD dwClientContext)
{
	char sendBuf[16] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"000801");
	sendBuf[7] = PACKAGE_TAIL_DATA_ETX;
	return GCommProt_PostSendBuf(dwClientContext,sendBuf,strlen(sendBuf));	
}

BOOL CBusinessManage_GISTerminal::CheckGisClientLoginProcess(const DWORD dwClientContext, const char *data)
{
	char TempBuf[64] = {0};
	memcpy(TempBuf,data,strlen(data));
	strtok(TempBuf,",");
	char *pszType = TempBuf;
	char *pszLoginNum = strtok(NULL,",");
	char *pszDeptNum = strtok(NULL,",");
	char *pszMac = strtok(NULL,",");

	if( 0 != strcmp(pszType,"C"))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [GIS] 登录协议格式错误,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),data);
		return FALSE;
	}

	if(pszType == NULL || pszLoginNum == NULL || pszDeptNum == NULL)
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [GIS] 登录协议格式错误\n"),CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}

	if (!CheckGISTerminal(pszLoginNum))
	{
		AddGISTerminal(dwClientContext,pszLoginNum,pszDeptNum,pszMac);
		gPGisMainCtrlProc.m_SubscriptionManager.Gis_Subscription_DeptNum(pszLoginNum,pszDeptNum);	//邮件订阅
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [GIS] 登录用户重复\n"),CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}

	return TRUE;
}

BOOL CBusinessManage_GISTerminal::CheckGISTerminal(const char *pszLoginNum)
{
	if (!m_HashmapDataGISLogin.IsEmpty())
	{
		return m_HashmapDataGISLogin.Find(pszLoginNum);
	}

	return FALSE;
}

void CBusinessManage_GISTerminal::AddGISTerminal(const DWORD dwClientContext,const char *pszLoginNum,const char *pszDeptNum,char *pszMac)
{
	DATA_GISTERMINAL temp_DATA_GISTERMINAL;
	strcpy(temp_DATA_GISTERMINAL.szLoginName,pszLoginNum);
	strcpy(temp_DATA_GISTERMINAL.szDeptNum,pszDeptNum);
	strcpy(temp_DATA_GISTERMINAL.szLoginMac,pszMac);
	m_HashmapDataGISTerminal.AddItem(dwClientContext,temp_DATA_GISTERMINAL);
}

void CBusinessManage_GISTerminal::EventLogin_DispatchAckToAnswer(const DWORD dwClientContext)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(sendBuf,"0,0%d0%d0%d0%d",gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_bConnPubServer,gPGisMainCtrlProc.m_BusinessManage_ConnectToLocationServer.m_bConnLocationServer,
		gPGisMainCtrlProc.m_DBManage_WebServer.m_bWebConnOK,gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_bConnGpsCheckServer);
	SendData(dwClientContext,sendBuf,strlen(sendBuf),C_TYPE_LOGIN_ACK);
	gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [GIS][ID:%d] GIS客户端登陆主控成功 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext);
}

void CBusinessManage_GISTerminal::GisClientLoginSuccessProcess(const DWORD dwClientContext)
{
	gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.TraversalItem((DWORD)(this),dwClientContext,CBusinessManage_GISTerminal::GisClientLoginSuccessProcess);
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS][ID:%d] GIS客户端登录成功,向GIS客户端发送在线终端总数量,count:%d\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.GetSize());
}

BOOL CBusinessManage_GISTerminal::GisClientLoginSuccessProcess(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	CBusinessManage_GISTerminal* temp_this = (CBusinessManage_GISTerminal*)dwInstaned;
	CBusinessManage_ConToPub::PTERMINAL_INFO ptemp_datainfo = (CBusinessManage_ConToPub::PTERMINAL_INFO)dwDataContext;
	char sendBuf[MAX_DATA_PACKAGE_LEN] ={0};

	DATA_GISTERMINAL temp_gisdatainfo;
	if (!temp_this->m_HashmapDataGISTerminal.FindItem(dwClientContext,temp_gisdatainfo))
	{
		return FALSE;
	}

// 	if (0 != strcmp(temp_gisdatainfo.szDeptNum,ptemp_datainfo->szDeptNum))
// 	{
// 		return FALSE;
// 	}
	
	BOOL bResult = FALSE;
	string tmp_strkey = ptemp_datainfo->szDeptNum;
	for (size_t ncount = 0;ncount < (tmp_strkey.size()/3);++ncount)
	{
		string tmp_str = tmp_strkey.substr(0,(ncount+1)*3);
		if (tmp_str == temp_gisdatainfo.szDeptNum)
		{
			bResult = TRUE;
		}
	}

	if (!bResult)
	{
		return FALSE;
	}

	sprintf(sendBuf,"%s,%s,%d",ptemp_datainfo->szDeviceNum,ptemp_datainfo->szDeviceType,LINE_ON);
	gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [GIS][ID:%d] 向GIS客户端发送在线终端信息 sendBuf:%s %s %s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),dwClientContext,sendBuf,temp_gisdatainfo.szDeptNum,ptemp_datainfo->szDeptNum);
	return temp_this->SendData(dwClientContext,sendBuf,strlen(sendBuf),C_TYPE_PRIVATENET_LINE);
}

//GIS客户端数据同步处理过程
BOOL CBusinessManage_GISTerminal::GisClientSystemSnycProcess(const DWORD dwClientContext, const char *SnycData, int nDataLen)
{
	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	memcpy(sendBuf,SnycData+2,nDataLen-2);

	char *pszDeptNum = NULL; //部门号
	char *pszDeviceNum = NULL; 
	char *pszChangeFlag = NULL;

	CString strDeptNum ;

	int nSnycType = SnycData[0] - '0';
	BOOL bisAddDervice = FALSE;
	char strTempDevice[MAX_LEN_DEVICECODE] = {0};

	switch(nSnycType)
	{
	case C_SNYC_TERMINLBIND: //绑定关系修改	巡防绑定
		strtok(sendBuf,",");
		pszDeptNum = sendBuf;				//部门号
		pszDeviceNum= strtok(NULL,",");
		pszChangeFlag = strtok(NULL,",");

		if(NULL == pszDeviceNum || NULL == pszDeptNum || NULL == pszChangeFlag)
		{
			return FALSE;
		}

		CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
		ZeroMemory(&temp_TermainalInfo,sizeof(CBusinessManage_ConToPub::TERMINAL_INFO));
		if (gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(pszDeviceNum,temp_TermainalInfo))
		{
			strDeptNum.Format("%s",pszDeviceNum);
			gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_PatrolDevListData.AddItem(strDeptNum);		//巡防数据同步
			gPGisMainCtrlProc.m_BusinessManage_ConToPub.AddPubTerminal(												//登陆用户信息同步
				temp_TermainalInfo.szDeviceNum,
				temp_TermainalInfo.szDeviceType,
				temp_TermainalInfo.szDeptNum,
				temp_TermainalInfo.szUserCode,
				temp_TermainalInfo.nUserType,
				temp_TermainalInfo.nLineState,
				atoi(pszChangeFlag));
			gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS] 绑定关系修改,修改在线列表中对应终端的参数，DeviceCode:%s ChangeFlag:%s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),pszDeviceNum,pszChangeFlag);
		}

		gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.SendSnycDataToGPSCheckServer(SnycData,nDataLen);
		break;
	case C_SNYC_POLICEINFO:
		break;
	case C_SNYC_CARSINFO:
		break;
	case C_SNYC_DECICEINFO://设备信息修改
		strtok(sendBuf,",");
		pszDeptNum = sendBuf;				//部门号
		pszDeviceNum= strtok(NULL,","); 
		pszChangeFlag= strtok(NULL,","); 
		
		if(pszDeviceNum == NULL || pszDeptNum == NULL || pszChangeFlag == NULL)
		{
			return FALSE;
		}

		/******************************************************/		//业务有问题
		if(atoi(pszChangeFlag) == 1) //增加设备
		{
			if(ResetLoginTerminalBindInfo(pszDeviceNum))				//设备无需处理
			{
//				ReportWebServiceDeviceOnLine(pszDeviceNum);				//通知WebService更新数据库终端状态  GIS连接 和数据库同步
				strcpy(strTempDevice,pszDeviceNum);
				bisAddDervice = TRUE;
			}
		}
		else if(atoi(pszChangeFlag) == 0) //删除设备
		{
			ResetLoginTerminalBindInfo(pszDeviceNum);
		}
		/******************************************************/

		break;
	case C_SNYC_AREARECT:	
		gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.SendSnycDataToGPSCheckServer(SnycData,nDataLen);
		break;
	case C_SNYC_AREALOAD:
		gPGisMainCtrlProc.m_DBManage_WebServer.WebServer_LoadDB_PatrolDeviceInfo(TRUE);
		strtok(sendBuf,",");
		pszDeptNum = sendBuf; 
		break;
	case C_SNYC_SCHEDUGROUPID:
		break;
	case C_SNYC_DEPTINFO:
		break;
	case C_SNYC_ROLEINFO:
		break;
	case C_SNYC_USEINFO:
		break;
	case C_SNYC_NOMERSNYC:
		break;
	default:
		break;
	}

	strDeptNum.Format("%s",pszDeptNum);
	SendData_Broadcast(sendBuf,strlen(sendBuf),C_TYPE_SYNC_NOTIFY,dwClientContext,CBusinessManage_GISTerminal::GisTer_Context_DeptNum);
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [GIS]向与同部门及上级部门GIS客户端广播同步数据:deptNum:%s,data:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),strDeptNum,sendBuf);

	return TRUE;
}

//复位在线终端绑定信息,终端信息修改时调用
BOOL CBusinessManage_GISTerminal::ResetLoginTerminalBindInfo(const char *strDeviceCode)
{
// 	POSITION pos = NULL;
// 	m_csLoginTerminalList.Lock();
// 	if( m_LoginTerminalList.IsEmpty() )
// 	{
// 		m_csLoginTerminalList.Unlock();
// 		return FALSE;
// 	}
// 	CString strTempNum = _T("");
// 	strTempNum.Format("%s",strDeviceCode);
// 
// 	pos = m_LoginTerminalList.GetHeadPosition();
// 	while(pos)
// 	{ 
// 		//GetNext()返回的是pos指向的当前节点，而不是返回下一个节点 ,在返回当前节点的同时，
// 		//pos被GetNext()修改成指向下一个节点 
// 		TERMINALInfo &tempTermainalInfo = m_LoginTerminalList.GetNext(pos);  
// 
// 		if( strTempNum.Compare(tempTermainalInfo.strNumber) == 0 )
// 		{
// 			tempTermainalInfo.bQueryInfoOk = FALSE;
// 			tempTermainalInfo.nQueryFailCount = 0;
// 			tempTermainalInfo.nUserType = 0;
// 			tempTermainalInfo.strDeptNum = _T("");
// 			tempTermainalInfo.strUserCode = _T("");
// 
// 			m_csLoginTerminalList.Unlock();
// 
// 			return TRUE;
// 		}
// 	}
// 	m_csLoginTerminalList.Unlock();

	return FALSE;
}

BOOL CBusinessManage_GISTerminal::GisTer_Context_DeptNum(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext)
{	
	CBusinessManage_GISTerminal* pInstance = (CBusinessManage_GISTerminal*)dwInstance;

	if (GHndDat_GetOwner(dwClientContext) != gPGisMainCtrlProc.g_dwGISTerminalContext || dwSrcClientContext == dwClientContext)
	{
		return FALSE;
	}
	
	DATA_GISTERMINAL temp_DATA_GISTERMINAL_SRC;
	pInstance->m_HashmapDataGISTerminal.FindItem(dwSrcClientContext,temp_DATA_GISTERMINAL_SRC);

	DATA_GISTERMINAL temp_DATA_GISTERMINAL_CRL;
	pInstance->m_HashmapDataGISTerminal.FindItem(dwClientContext,temp_DATA_GISTERMINAL_CRL);

	if (0 == strcmp(temp_DATA_GISTERMINAL_SRC.szDeptNum,temp_DATA_GISTERMINAL_CRL.szDeptNum) || 0 == strcmp("0000",temp_DATA_GISTERMINAL_CRL.szDeptNum))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBusinessManage_GISTerminal::GisTer_All(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext)
{	
	CBusinessManage_GISTerminal* pInstance = (CBusinessManage_GISTerminal*)dwInstance;

	if (GHndDat_GetOwner(dwClientContext) != gPGisMainCtrlProc.g_dwGISTerminalContext || dwSrcClientContext == dwClientContext)
	{
		return FALSE;
	}

	return TRUE;
}

void CBusinessManage_GISTerminal::SendData_Broadcast(char* pszSendData, const DWORD dwSendLen,const int nCmdType,const DWORD dwClientContext,PFN_ON_GSOCK_TRAVERSAL pfntravrsal)
{
	if ((dwSendLen + 8) > SEND_DATA_PACKAGE_LEN)
	{
		return;
	}
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"%.4X%.2X%s",dwSendLen+8,nCmdType,pszSendData);
	sendBuf[dwSendLen + 7] = PACKAGE_TAIL_DATA_ETX;

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

	GTcpSvr_PostBroadcastBuf(sendBuf,strlen(sendBuf),(DWORD)this,dwClientContext,pfntravrsal);

#if(_TIME)
	if (!QueryPerformanceCounter(&nEndTime))	// 获取时钟计数 
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] 获取时钟计数 失败 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"));			
	}

	gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] [SendData_Broadcast] 时间精准差 %f ms 成功 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),(double)(nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart);
#endif
}

BOOL CBusinessManage_GISTerminal::GISTer_Subscription(const DWORD dwInstance, const DWORD dwDataContext, const DWORD dwClientContext, const char *szchar)		//add_Subscription 
{	
	CBusinessManage_GISTerminal* pInstance = (CBusinessManage_GISTerminal*)dwInstance;	
	std::string temp_str = szchar;
	BOOL bResult = gPGisMainCtrlProc.m_SubscriptionManager.GetSubscriptionInfo_GIS(temp_str,dwClientContext,dwDataContext);
	gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [SubscriptionManager] [GISTer_Subscription] 查询成功：%d \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),bResult);
	return bResult;
}


BOOL CBusinessManage_GISTerminal::GISTer_DeptNum_Char(const DWORD dwInstance, const DWORD dwSrcClientContext, const DWORD dwClientContext, const char *szchar)
{	
	CBusinessManage_GISTerminal* pInstance = (CBusinessManage_GISTerminal*)dwInstance;

	if (GHndDat_GetOwner(dwClientContext) != gPGisMainCtrlProc.g_dwGISTerminalContext || dwSrcClientContext == dwClientContext)
	{
		return FALSE;
	}

	DATA_GISTERMINAL temp_DATA_GISTERMINAL_CRL;
	pInstance->m_HashmapDataGISTerminal.FindItem(dwClientContext,temp_DATA_GISTERMINAL_CRL);

	if (0 == strcmp(szchar,temp_DATA_GISTERMINAL_CRL.szDeptNum) || 0 == strcmp("0000",temp_DATA_GISTERMINAL_CRL.szDeptNum))
	{
		return TRUE;
	}

	return FALSE;
}

void CBusinessManage_GISTerminal::SendData_Broadcast_char(const char* pszSendData, const DWORD dwSendLen, const int nCmdType, const DWORD dwClientContext, char *szchar,PFN_ON_GSOCK_FINDDATA_CHAR pfntravrsal)
{
	if ((dwSendLen + 8) > SEND_DATA_PACKAGE_LEN)
	{
		return;
	}
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	sendBuf[0] = PACKAGE_HEAD_DATA_STX;
	sprintf(sendBuf+1,"%.4X%.2X%s",dwSendLen+8,nCmdType,pszSendData);
	sendBuf[dwSendLen + 7] = PACKAGE_TAIL_DATA_ETX;

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

	GTcpSvr_PostBroadcastBuf_Char(sendBuf,strlen(sendBuf),(DWORD)this,dwClientContext,szchar,pfntravrsal);

#if(_TIME)
	if (!QueryPerformanceCounter(&nEndTime))	// 获取时钟计数 
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] 获取时钟计数 失败 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"));			
	}

	gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [Time] [SendData_Broadcast_char] 时间精准差 %f ms 成功 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),(double)(nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart);
#endif
}

void CBusinessManage_GISTerminal::ShowLoginGisNum(void)
{
	gWriteLogApp.WriteLog(FOR_INFO,_T("%s [SYS] 在线Gis信息 数据连接数:%d SOCKET连接数：%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_HashmapDataGISTerminal.GetSize(),GTcpSvr_GetListenerConnectCount(m_dwGisServerContext));
}

void CBusinessManage_GISTerminal::ShowLoginGisClientInfo(void)
{
	m_HashmapDataGISTerminal.TraversalItem((DWORD)(this),NULL,GetOnLineGisClientInfo);
	ShowLoginGisNum();
}

BOOL CBusinessManage_GISTerminal::GetOnLineGisClientInfo(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	CBusinessManage_GISTerminal* temp_this = (CBusinessManage_GISTerminal*)dwInstaned;
	PDATA_GISTERMINAL ptemp_datainfo = (PDATA_GISTERMINAL)dwDataContext;
	gWriteLogApp.WriteLog(FOR_INFO,_T("%s [SYS] 在线Gis信息 LoginName:%s DeptNum:%s LoginMac:%s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
		ptemp_datainfo->szLoginName,ptemp_datainfo->szDeptNum,ptemp_datainfo->szLoginMac);
	return FALSE;
}
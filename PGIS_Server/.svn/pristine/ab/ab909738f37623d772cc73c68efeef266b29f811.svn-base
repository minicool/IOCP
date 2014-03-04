#include "StdAfx.h"
#include "MultiProtocolProcess.h"
#include "PGisMainCtrlServer.h"

extern CWriteLogThread gWriteLogApp;
extern UINT g_dwWorkThreadID;

MultiProtocolProcess::MultiProtocolProcess(void):g_nRecvConntClientCount(0),m_dwInstance(0),m_dwThreadID(0)
{

}

MultiProtocolProcess::~MultiProtocolProcess(void)
{
}

/*******************************************************************************
* 函数名称 : MultiProtocolProcess::Init
* 功能描述 : 用于初始化协议集所需信息
* 参　　数 : DWORD dwInstance	用于处理DATACALLBACK 回调函数类 指针
* 参　　数 : DWORD dwThread		用于线程投递信息处理
* 参　　数 : UINT nAppID		用于命令集信息
* 参　　数 : UINT Protocol		用于传输协议信息
* 返 回 值 : BOOL				返回初始化正确信息 读取配置文件出错返回FALSE
* 作　　者 : WANGZH
* 设计日期 : 2013年4月25日星期四
* 修改日期     修改人    修改内容
*******************************************************************************/
BOOL MultiProtocolProcess::Init(DWORD dwInstance,DWORD dwThread,UINT nAppID,UINT Protocol)
{
	m_nAppID = nAppID;
	m_dwInstance = dwInstance;
	m_dwThreadID = dwThread;
	
	if (Protocol == NULL_PROTOCOL)
	{
		CString tempstr =_T("pGisMainCtrlCfg.ini");

		if(!rwIniTemp.CheckIniFile(tempstr))
		{
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 系统启动失败,找不到配置文件\n"),CTime::GetCurrentTime().Format("%H:%M:%S"));
			return FALSE;
		}

		m_nProtocol = rwIniTemp.GetInt(_T("ProtocolSet"),_T("Set"),tempstr);
		if(m_nProtocol == 0)
		{
			m_nProtocol = UDP_SERVER;
		}
	}
	else
	{
		m_nProtocol = Protocol;
	}

	switch (m_nProtocol)
	{
	case TCP_SERVER:
		m_completionRoutineServer.InitRoutineServer(MAX_COUNT);
		break;
	case TCP_CLIENT:
		break;
	case UDP_SERVER:
		break;
	case UDP_CLIENT:
		break;
	case FTP_PROTOCOL:
		break;
	default:
		return FALSE;
		break;
	}
		
	return Loadconfig();
}

BOOL MultiProtocolProcess::Loadconfig()
{
	CString tempstr =_T("pGisMainCtrlCfg.ini");

	if(!rwIniTemp.CheckIniFile(tempstr))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 系统启动失败,找不到配置文件\n"),
			CTime::GetCurrentTime().Format("%H:%M:%S"));

		return FALSE;
	}

	switch (m_nProtocol)
	{
	case TCP_SERVER:
	case TCP_CLIENT:
	case UDP_CLIENT:
	case UDP_SERVER:
		m_nLocalPort = rwIniTemp.GetInt(_T("ProtocolSet"),_T("Local_Port"),tempstr);
		if(m_nLocalPort == 0)
		{
			m_nLocalPort = 9003;
		}

		m_nRemotePort = rwIniTemp.GetInt(_T("ProtocolSet"),_T("Remote_Port"),tempstr);
		if(m_nRemotePort == 0)
		{
			m_nRemotePort = 9004;
		}

		if(rwIniTemp.ReadString(_T("ProtocolSet"),_T("Remote_IP"),tempstr,m_szRemoteIP) == 0) 
		{
			strncpy(m_szRemoteIP,"192.168.1.1",sizeof(m_szRemoteIP));
		}
		break;
	case FTP_PROTOCOL:
		if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPDownLoadPatch"),tempstr,m_strFTPDownLoadPatch) == 0) 
		{
			sprintf(m_strFTPDownLoadPatch,"%sFTPConvey",rwIniTemp.GetCurrPath());
		}
		else
		{
			if(strlen(m_strFTPDownLoadPatch) == 0)
			{
				sprintf(m_strFTPDownLoadPatch,"%sFTPConvey",rwIniTemp.GetCurrPath());
			}
		}

		m_FtpFileReadTimeOut = rwIniTemp.GetInt(_T("FTPSet"),_T("FtpFileReadTimeOut"),tempstr);
		if(m_FtpFileReadTimeOut <= 0)
		{
			m_FtpFileReadTimeOut = 10;
		}
		break;
	default:
		break;
	}
	
	return TRUE;
}

BOOL MultiProtocolProcess::Start()
{

	int nRes;

	switch (m_nProtocol)
	{
	case UDP_SERVER:
		if (!m_iosocketudp.Start(NULL,m_nLocalPort,DataCallBack,DWORD(this)))
		{
			m_iosocketudp.Stop();
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 启动 UDP Server 监听失败\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
			return FALSE;
		}

		gWriteLogApp.WriteLog(FOR_ALARM, TEXT("%s [SYS] 启动 UDP Server 监听成功,端口:%d\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_nLocalPort);
		break;
	case UDP_CLIENT:
		if (!m_iosocketudp.Start(NULL,m_nLocalPort,DataCallBack,DWORD(this)))
		{
			m_iosocketudp.Stop();
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 启动 UDP Client 连接失败\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
			return FALSE;
		}

		gWriteLogApp.WriteLog(FOR_ALARM, TEXT("%s [SYS] 启动 UDP Client 连接成功,端口:%d\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_nLocalPort);
		break;
	case TCP_CLIENT:
		nRes = m_TcpClient.Connect(m_szRemoteIP,m_nRemotePort,DataCallBack,DWORD(this));
		if (nRes != TCPCLIENT_CONNECTSUCCESS)
		{
			m_TcpClient.Close();
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 启动 TCP Client 连接失败\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
			return FALSE;
		}

		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 启动 TCP Client 连接成功,IP:%s,端口:%d\n",CTime::GetCurrentTime().Format("%H:%M:%S"),m_szRemoteIP,m_nRemotePort);
		break;
	case TCP_SERVER:
		nRes = m_completionRoutineServer.Start(m_nLocalPort,NULL,DataCallBack,DWORD(this),m_nAppID);

		if(nRes != TCPSERVER_STARTSUCCESS)
		{
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 启动 TCP Server 监听失败,res:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),nRes);
			return FALSE;
		}

		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [SYS] 启动 TCP Server 监听成功,端口:%d \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),m_nLocalPort);
		break;
	case FTP_PROTOCOL:
		if(!m_ftpconvery.StartFTPConvey(NULL))
		{
			m_ftpconvery.StopFTPConvey();
			return FALSE;
		}
		break;
	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL MultiProtocolProcess::Close()
{
	switch (m_nProtocol)
	{
	case TCP_SERVER:
		for(int i = 0; i< MAX_COUNT; i++)
		{
			if(m_PubServerTcp[i].GetConnectState() != TCP_DISCONNECT)
			{
 				m_completionRoutineServer.CloseClient(i);
			}
		}
		m_completionRoutineServer.ReleaseRoutineServer();
		break;
	case TCP_CLIENT:
		m_TcpClient.Close();
		break;
	case UDP_SERVER:
		m_iosocketudp.Stop();
		break;
	case UDP_CLIENT:
		m_iosocketudp.Stop();
		break;
	case FTP_PROTOCOL:
		if (!m_ftpconvery.StopFTPConvey())
		{
			return FALSE;
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL MultiProtocolProcess::SendData(int nType,const char* szData,int ndatalen,int nSocketID)
{
	char senddata[MAX_DATA_PACKAGE_LEN] = {0};
	sprintf(senddata,"%.4X%.2X%s",ndatalen,nType,szData);

	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
	strncpy(sendBuf,Cmdtypelist[m_nAppID].HeadData,strlen(Cmdtypelist[m_nAppID].HeadData));
	strncpy(sendBuf+strlen(Cmdtypelist[m_nAppID].HeadData),senddata,strlen(senddata));
	strncpy(sendBuf+strlen(Cmdtypelist[m_nAppID].HeadData)+ndatalen+6,Cmdtypelist[m_nAppID].TailData,strlen(Cmdtypelist[m_nAppID].TailData));

	TRACE("[DATA_SEND] SendData:%s Datalen:%d SocketID:%d \n",sendBuf,ndatalen,nSocketID);

	return ProceSendData(sendBuf,strlen(sendBuf),nSocketID);
}

BOOL MultiProtocolProcess::ProceSendData(const char* szData,int ndatalen,int nSocketID)
{
	if (szData == NULL)
	{
		return FALSE;
	}
	
	switch (m_nProtocol)
	{
	case TCP_SERVER:
		if (-1 == nSocketID)
		{
			for(int SocketID = 0;SocketID < MAX_COUNT;++SocketID)
			{
				if( m_completionRoutineServer.Send(SocketID,szData,ndatalen) == TCPSERVER_ERROR_SEND )
				{
					m_completionRoutineServer.CloseClient(SocketID);
					TRACE("[ERROR] TCP_SERVER Send Data is Wrong! SocketID:%d",SocketID);
				}
			}
		}
		else
		{
			if( m_completionRoutineServer.Send(nSocketID,szData,ndatalen) == TCPSERVER_ERROR_SEND )
			{
				m_completionRoutineServer.CloseClient(nSocketID);
				return FALSE;
			}
		}
		break;
	case TCP_CLIENT:
		if(m_TcpClient.Send(szData,ndatalen) == TCPCLIENT_ERROR_SEND)
		{
			m_TcpClient.Close();
			return FALSE;
		}
		break;
	case UDP_SERVER:
		if (m_iosocketudp.UdpSendTo((PBYTE)szData,ndatalen,m_szRemoteIP,m_nRemotePort) != ndatalen)
		{
			return FALSE;
		}
		break;
	case UDP_CLIENT:
		if (m_iosocketudp.UdpSendTo((PBYTE)szData,ndatalen,m_szRemoteIP,m_nRemotePort) != ndatalen)
		{
			return FALSE;
		}
		break;
	case FTP_PROTOCOL:
		m_ftpconvery.WriteData(0,szData);
		break;
	default:
		break;
	}	

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
void MultiProtocolProcess::TcpClientConnected(int typeID,int nSocketID,const char *strIP,int nPort)
{
	if( nSocketID < MAX_COUNT )
	{
		m_PubServerTcp[nSocketID].SetSocketConn(nSocketID,strIP);
		m_PubServerTcp[nSocketID].StartTcpTickTimer(TCP_LOGIN_WAIT,/*m_nLoginTimerVal*/40);
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] TCP Client 接入成功,IP地址:%s,端口:%d\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,strIP,nPort);
	}
	else
	{
		m_completionRoutineServer.CloseClient(nSocketID);
	}

}

void MultiProtocolProcess::TcpClientDisconnect(int typeID,int nSocketID,const char *strIP,int nPort)
{
	if(m_PubServerTcp[nSocketID].GetConnectState() != TCP_DISCONNECT)
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] TCP Client 连接断开,IP地址:%s,端口:%d\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,strIP,nPort);
	}

	m_PubServerTcp[nSocketID].Reset();
}

void MultiProtocolProcess::TcpServerDisconnect(int typeID,int nSocketID,const char *strIP,int nPort)
{
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] TCP Server 连接断开 \n",CTime::GetCurrentTime().Format("%H:%M:%S"));
	m_TcpClient.Close();
}

BOOL MultiProtocolProcess::ProcessData(int nAppID,int nSocketID,const char *recvData ,int nRecvLen)
{
	switch(nAppID)
	{
	case SOCKET_PUBSERVER_TYPEID:
		return ProcessData(nSocketID,recvData,nRecvLen);
		break;
	default:
		return FALSE;
		break;
	}
}

/*******************************************************************************
* 函数名称 : MultiProtocolProcess::DataCallBack
* 功能描述 : 
* 参　　数 : UINT uMsg
* 参　　数 : DWORD dwParam
* 参　　数 : DWORD dwInstance
* 返 回 值 : DWORDWINAPI 
* 作　　者 : WANGZH
* 设计日期 : 2013年4月25日星期四
* 修改日期     修改人    修改内容
*******************************************************************************/
DWORD WINAPI MultiProtocolProcess::DataCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance)
{
	MultiProtocolProcess* temp_mpp = (MultiProtocolProcess*)dwInstance;
	TcpServerClient* temp_socketinfo = (TcpServerClient*)dwParam;

	switch(uMsg)
	{
	case TCPSERVER_EVENT_CLIENTJOIN:
		temp_mpp->TcpClientConnected(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->szIP,temp_socketinfo->nPort);
		break;
	case TCPSERVER_EVENT_CLIENTEXIT:
		temp_mpp->TcpClientDisconnect(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->szIP,temp_socketinfo->nPort);
		break;
	case TCPSERVER_EVENT_RECVDATA:
		temp_mpp->ProcessData(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->pszRecvData,temp_socketinfo->nRecvDataLen);
		break;
	case UDP_DATA:
		temp_mpp->ProcessData(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->pszRecvData,temp_socketinfo->nRecvDataLen);
		break;
	case TCPCLIENT_EVENT_RECVDATA:
		temp_mpp->ProcessData(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->pszRecvData,temp_socketinfo->nRecvDataLen);
		break;
	case TCPCLIENT_EVENT_DISCONNECT:
		temp_mpp->TcpServerDisconnect(temp_socketinfo->nAppID,temp_socketinfo->nID,temp_socketinfo->pszRecvData,temp_socketinfo->nRecvDataLen);
		break;
	default:
		break;
	}

	return 0;
}


/*******************************************************************************
* 函数名称 : MultiProtocolProcess::ProcessData
* 功能描述 : 对数据包进行脱包处理，去除包头及包尾
* 参　　数 : int nAppID				用于对应包头及包尾模式
* 参　　数 : int nSocketID			接收数据所获得的SOCKETID[对应SERVER中的SOCKET]信息
* 参　　数 : const char *recvData	获取的数据信息
* 参　　数 : int nRecvLen			数据的长度信息
* 返 回 值 : BOOL					判断处理包的是否正常
* 作　　者 : WANGZH
* 设计日期 : 2013年4月24日星期三
* 修改日期     修改人    修改内容
*******************************************************************************/
BOOL MultiProtocolProcess::ProcessData(int nSocketID,const char *recvData ,int nRecvLen)
{
	if(m_nBufLenCount + nRecvLen > MAX_RECVICE_BUFFER)
	{
		memset(m_RecvBuf,0,MAX_RECVICE_BUFFER);
		return FALSE;
	}

	memcpy(m_RecvBuf+m_nBufLenCount,recvData,nRecvLen);
	m_nBufLenCount = m_nBufLenCount + nRecvLen;
	int nSpaceLen = m_nBufLenCount;
	char *pCurPoint = m_RecvBuf;
	int nPackageLen = 0;

	char *m_strHead =  Cmdtypelist[m_nAppID].HeadData;
	unsigned int temp_nHeadDataLen = strlen(m_strHead);
	char *m_strTail =  Cmdtypelist[m_nAppID].TailData;
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
				nHeadPoint = ++i;
				break;
			}
		}

		if (-1 == nHeadPoint)
		{
			TRACE(_T("[THREAD:%d][ID:%d] 包头错:len:%d,recv:%s\n"),GetCurrentThreadId(),nSocketID,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][MultiProtocolProcess] 整包%s 截取后的包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,recvData,temppHeadCurPoint,m_strHead);
			nSpaceLen = 0;
			break;	
		}

		char *ptempTailCurPoint = pCurPoint;
		int nTailPoint = 0;
		for(int i = 0 ; i < nSpaceLen; i++ )
		{
			if(strncmp(ptempTailCurPoint,m_strTail,temp_nTailDataLen) != 0)
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
				AnalyseData(nSocketID,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
				TRACE(_T("[ID:%d] 完整包为%s\n"),nSocketID,pCurPoint+temp_nHeadDataLen);
//				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] 处理整包:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,recvData);
			}
			else
			{
				TRACE(_T("[ID:%d] 包长度超过允许的最大值:len:%d,recv:%s\n"),nSocketID,nSpaceLen,recvData);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][MultiProtocolProcess] 包长度超过允许的最大值:len:%d,recv:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,nSpaceLen,recvData);
			}
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			TRACE(_T("[ID:%d] 没有找到包尾:len:%d,recv:%s\n"),nSocketID,nSpaceLen,pCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d][MultiProtocolProcess] 整包%s 截取后的包尾错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,recvData,ptempTailCurPoint,m_strTail);
			nSpaceLen = 0;
			break;
		}

	}

	if( nSpaceLen > 0 )
	{
		m_nBufLenCount = nSpaceLen;
		if( pCurPoint != m_RecvBuf )
		{
			memcpy(m_RecvBuf,pCurPoint,m_nBufLenCount);
			memset(m_RecvBuf+m_nBufLenCount,0,MAX_RECVICE_BUFFER-m_nBufLenCount);
		}
	}
	else
	{
		m_nBufLenCount = 0;
		memset(m_RecvBuf,0,MAX_RECVICE_BUFFER);
	}

	return TRUE;
}


/*******************************************************************************
* 函数名称 : MultiProtocolProcess::AnalyseData
* 功能描述 : 分析经去包标志数据，进行分析数据是否丢包 解析CMD命令
* 参　　数 : int nSocketID
* 参　　数 : const char *recvData
* 参　　数 : int nRecvLen
* 返 回 值 : BOOL
* 作　　者 : WANGZH
* 设计日期 : 2013年4月24日星期三
* 修改日期     修改人    修改内容
*******************************************************************************/
BOOL MultiProtocolProcess::AnalyseData(int nSocketID,const char *recvData ,int nRecvLen)
{
	if(nRecvLen < PACKAGE_LEN)
	{
		return FALSE;
	}

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(recvData, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype
	nRecvLen = nRecvLen - PACKAGE_LEN;
	recvData = recvData + PACKAGE_LEN;

	if(nTempLen != nRecvLen)
	{
		TRACE("AnalyseData 数据长度比较错误!  SOCKETID:%d \n",nSocketID);
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] Data Len Error %s RecvLen:%d TempLen:%d \n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,recvData,nRecvLen,nTempLen);
		return FALSE;
	}

	RECV_CLIENTDATA* tempRecvMessage = new RECV_CLIENTDATA;
	memset(tempRecvMessage->MessagePacket,0,MAX_DATA_PACKAGE_LEN);

	tempRecvMessage->nAppID = m_nAppID;
	tempRecvMessage->nSockID = nSocketID;
	tempRecvMessage->nCmdType = nCmdType;
	tempRecvMessage->length = nRecvLen; 
	memcpy(tempRecvMessage->MessagePacket,recvData,nRecvLen);

	if (!PostThreadMessage(g_dwWorkThreadID,WM_RECVICE_MESSAGE,WPARAM(tempRecvMessage),0))
	{
		delete tempRecvMessage;
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [ID:%d] PostThreadMessage Error %s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,recvData);
	}
	
	return TRUE;
}
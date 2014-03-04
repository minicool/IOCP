// TcpClient.cpp: implementation of the Crwini class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Tcp_MainServer.h"
//#include "MemoryPool.h"

extern CWriteLogThread gWriteLogApp;

extern ::CCriticalSection g_csRecvServerList;
extern UINT g_dwWorkThreadID;

extern int g_nRecvLocationGPSCount;
extern int g_nPatrAlertCount;

extern UINT g_dwWebThreadID;
extern int g_nRecvConntGisClientCount;
//////////////////////////////////////////////////////////////////////
//
CTcpMainServer::CTcpMainServer()
{
	memset(m_RecvBuf,0,MAX_RECVICE_BUFFER);
	m_nRecvBufLen = 0;

	m_nCheckTimeValue = 0;
	m_nTickCount = 0;
	m_bStartOk = FALSE;
	memset(m_strServerIP,0,MAX_IPADDRESS_LEN);

	InitializeCriticalSection(&m_SendCriSec);
}

//
CTcpMainServer::~CTcpMainServer()
{
	DeleteCriticalSection( &m_SendCriSec );  
}

//静态函数
DWORD WINAPI CTcpMainServer::TcpClientCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance)
{
	int nSpaceLen = 0;
    char *pCurPoint = NULL;
    int nPackageLen = 0;

    CTcpMainServer *pGisMainCtrl = (CTcpMainServer *)dwInstance;
    TcpConnect *pTcpConnectInfo = (TcpConnect *)dwParam;

    switch(uMsg) 
	{
	case TCPCLIENT_EVENT_DISCONNECT:
		pGisMainCtrl->TcpDisConnect();
        break;
    case TCPCLIENT_EVENT_RECVDATA:
		pGisMainCtrl->AnalyseRecvTcpServerData(pTcpConnectInfo->pszRecvData,
            pTcpConnectInfo->nRecvDataLen);
		break;
	default:
		break;
	}

	return 0;
}

void CTcpMainServer::TcpDisConnect()
{
	CString strTemp;
	if(!m_bStartOk)
	{
		return; //add by kevin 2010-3-2
	}
       
	m_ConnSockServer.Close(); //先关之前的连接 2010－6－17
		return;
	}
    

	m_bStartOk = FALSE; //add by kevin 2010-1-20

	gWriteLogApp.WriteLog(FOR_ALARM,strTemp);
    
    Sleep(10);
    m_ConnSockServer.Close(); //先关之前的连接 2010－6－17
}

//启动
BOOL CTcpMainServer::Start(const char *strIPAddress,int nPort,int AppID)
{
	CString strTemp = "";
	strcpy(m_strServerIP,strIPAddress);
	m_nGpsServerPort = nPort;
	m_nAppID = AppID;

	m_nCheckTimeValue = 0; 
	m_nTickCount = 0;

	int nRes ;
    
    nRes = m_ConnSockServer.Connect(strIPAddress,
            nPort,
            TcpClientCallBack,
            (DWORD)this);

    if( nRes != TCPCLIENT_CONNECTSUCCESS )
    {
		switch(AppID)
		{
		default:
			break;
		}
        gWriteLogApp.WriteLog(FOR_ALARM,strTemp);

		return FALSE;
    }

    gWriteLogApp.WriteLog(FOR_ALARM,strTemp);

	m_bStartOk = TRUE;

	return TRUE;
}

//停止
void CTcpMainServer::Stop()
{
	if(m_bStartOk)
	{
		m_ConnSockServer.Close();

		CString strTemp;
		gWriteLogApp.WriteLog(FOR_ALARM,strTemp);
	}
	
	m_bStartOk = FALSE;
	m_nTickCount = 0;

	m_nCheckTimeValue = 0;
}  

//发送数据
int CTcpMainServer::SendData(const char *sendBuf,int nSendLen)
{
	int nLen = 0;
	if(m_bStartOk)
	{
		EnterCriticalSection(&m_SendCriSec);
		nLen = m_ConnSockServer.Send(sendBuf,nSendLen);
		LeaveCriticalSection(&m_SendCriSec);

		if(nLen == TCPCLIENT_ERROR_SEND )
		{
			m_ConnSockServer.Close();
			m_bStartOk = FALSE;
			m_nTickCount = 0;

			m_nCheckTimeValue = 0;

			CString strTemp;

			gWriteLogApp.WriteLog(FOR_ALARM,strTemp);
		}
	}

	return nLen;
	
}

void CTcpMainServer::CheckTimerOver()
{
	if(m_bStartOk)
	{
		if(m_nCheckTimeValue++ >= 10)
		{
			m_nCheckTimeValue = 0;
			
			if(m_nTickCount >= 3)
			{
				m_ConnSockServer.Close();
				m_bStartOk = FALSE;
				m_nTickCount = 0;

                   PostThreadMessage(g_dwWorkThreadID,WM_SERV_CONNERROR_MESSAGE,CASE_GPSCHECK_CONNERROR,0);
				}
				else
				{
					gWriteLogApp.WriteLog(FOR_ALARM,"%s <LOCAL>地理信息服务器TCP连接心跳超时\n",
						CTime::GetCurrentTime().Format("%H:%M:%S"));

                    PostThreadMessage(g_dwWorkThreadID,WM_SERV_CONNERROR_MESSAGE,CASE_LOCATION_CONNERROR,0);
				}
			}
			else
			{
				//发送心跳
				char sendBuf[16] = {0};
				sendBuf[0] = PACKAGE_HEAD_DATA_STX;
				sprintf(sendBuf+1,"000801");
				sendBuf[7] = PACKAGE_TAIL_DATA_ETX;

				if(SendData(sendBuf,8) < 0)
				{
					m_ConnSockServer.Close();
					m_bStartOk = FALSE;
					m_nTickCount = 0;
				}
				else
				{
					m_nTickCount++;
				}
			}
		}
	}
	else
	{
		if(m_nCheckTimeValue++ >= 30)
		{
			m_nCheckTimeValue = 0;
			Start(m_strServerIP,m_nGpsServerPort,m_nAppID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
//
BOOL CTcpMainServer::AnalyseRecvTcpServerData(const char *recvData ,int nRecvLen)
{
	int nSpaceLen = 0;
    char *pCurPoint = NULL;
    int nPackageLen = 0;

	int nPoint;
    int i = 0;

	if( m_nRecvBufLen + nRecvLen > MAX_RECVICE_BUFFER )
    {
        gWriteLogApp.WriteLog( FOR_ALARM,"%s <SER>:超长的数据包,接收缓冲区已满.nRecvLen:%d\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),m_nRecvBufLen );

        m_nRecvBufLen = 0;

        return FALSE;
    }

    memcpy(m_RecvBuf+m_nRecvBufLen,recvData,nRecvLen);
    m_nRecvBufLen = m_nRecvBufLen + nRecvLen;
    //分包
    nSpaceLen = m_nRecvBufLen;
    pCurPoint = m_RecvBuf;
    
    while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
    {
        if( pCurPoint[0] != PACKAGE_HEAD_DATA_STX ) //检查包头
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s <SER>:包头错:len:%d\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),nSpaceLen);

            nSpaceLen = 0;
            break;
        }

        //查找包尾
        nPoint = 0;
        for( i = 0 ; i < nSpaceLen; i++ )
        {
            if( pCurPoint[i] == PACKAGE_TAIL_DATA_ETX)
            {
                nPoint = i;
                break;
            }
        }
        if( nPoint > 0 )
        {
            //一个完整的包
            nPackageLen = nPoint + 1;
            //
            if( nPackageLen < MAX_DATA_PACKAGE_LEN )
            {
				pCurPoint[nPackageLen -1] = NULL;

                TcpServerDataProcess(pCurPoint+1,nPackageLen - 2);
            }
            else
            {
                gWriteLogApp.WriteLog( FOR_ALARM,"%s <SER>:包长度超过允许的最大值,len:%d.\n",
                    CTime::GetCurrentTime().Format("%H:%M:%S"),nPackageLen );
            }

            //剩下的数据继续分包
            pCurPoint = pCurPoint + nPackageLen;
            nSpaceLen = nSpaceLen - nPackageLen;
        }
        else
        {
            if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
            {
                gWriteLogApp.WriteLog( FOR_ALARM,"%s <SER>:超长的数据包,len:%d.\n",
                        CTime::GetCurrentTime().Format("%H:%M:%S"),nSpaceLen );
                nSpaceLen = 0;
            }
            break;
        }
        
    }//endwhile

    //是否有没有处理的数据
    if( nSpaceLen > 0 )
    {
        m_nRecvBufLen = nSpaceLen;
        if( pCurPoint != m_RecvBuf )
        {
            memcpy(m_RecvBuf,pCurPoint,m_nRecvBufLen);
            memset(m_RecvBuf + m_nRecvBufLen,0,MAX_RECVICE_BUFFER - m_nRecvBufLen);
        }
    }
    else
    {
        m_nRecvBufLen = 0;
        memset(m_RecvBuf,0,MAX_RECVICE_BUFFER);
    }

	return TRUE;
}

//recvData为去掉了包头包尾的数据
void CTcpMainServer::TcpServerDataProcess(const char *recvData, int nRecvLen)
{
	if(nRecvLen < 6)
	{
		return;
	}

	char recvBuf[MAX_DATA_PACKAGE_LEN] ={0};
	
	int nTempLen;
	int nCmdType;
	sscanf(recvData, "%4x%2x",&nTempLen,&nCmdType);
	nTempLen = nTempLen - 2;
	if(nTempLen != nRecvLen)
	{
		TRACE("数据长度不一致,data:%s,len:%d\n",recvData,nRecvLen);
		return;
	}

	RECV_SERVERDATA tempServerData;
	memset(tempServerData.Data,0,MAX_DATA_PACKAGE_LEN);
	switch(m_nAppID)
	{
	case SOCKET_SERVER_GPSCHECK_TYPEID:
	{
		if(nCmdType == 1) //握手回应
		{
			m_nTickCount = 0;
			//TRACE("%s 巡防管理服务器握手回应数据\n",
			//	CTime::GetCurrentTime().Format("%H:%M:%S"));
		}
		else if(nCmdType == 5) //报警提示
		{
			//去掉recvData的长度字段和命令类型字段，后面的数据就是设备号码+报警标志
			memcpy(recvBuf,recvData+6,nRecvLen-6);

			strtok(recvBuf,",");
			tempServerData.length = strlen(recvBuf); //终端号码
			tempServerData.nAppID = SOCKET_SERVER_GPSCHECK_TYPEID;
			memcpy(tempServerData.Data,recvBuf,tempServerData.length);

			g_csRecvServerList.Lock();
			g_RecvServerList.AddTail(tempServerData);
			g_csRecvServerList.Unlock();

			PostThreadMessage(g_dwWorkThreadID,WM_RECVICE_SERVER_MESSAGE,0,0);

			gWriteLogApp.WriteLog(FOR_TRACE_2,"%s <PATR>巡防管理服务器报警提示数据,DeviceCode:%s\n",
				CTime::GetCurrentTime().Format("%H:%M:%S"),recvBuf);

			g_nPatrAlertCount++;
		}
		else
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s <PATR>无法识别的巡防管理服务器数据,data:%s\n",
				CTime::GetCurrentTime().Format("%H:%M:%S"),recvData);
		}
		break;
	}
	case SOCKET_SERVER_LOCATION_TYPEID:
	{
		if(nCmdType == 1) //握手回应
		{
			m_nTickCount = 0;
			//TRACE("%s 地理位置服务器握手回应数据\n",
			//	CTime::GetCurrentTime().Format("%H:%M:%S"));
		}
		else if(nCmdType == 5) //位置回应
		{
			//去掉recvData的长度字段和命令类型字段，后面的数据就是index+设备编号+地理位置
			memcpy(recvBuf,recvData+6,nRecvLen-6);

			strtok(recvBuf,",");
			char *pStrQueryIndex = recvBuf; //编号
			char *pStrDeviceCode = strtok(NULL,",");
			char *pStrLocation= strtok(NULL,",");

			if(pStrQueryIndex == NULL || pStrDeviceCode == NULL || pStrLocation == NULL)
			{
				gWriteLogApp.WriteLog(FOR_ALARM,"%s <LOCAL>无法识别的地理位置服务器数据\n",
					CTime::GetCurrentTime().Format("%H:%M:%S"));

				break;
			}
			
			g_nRecvLocationGPSCount++;

			int nQIndex = atoi(pStrQueryIndex);

			gWriteLogApp.WriteLog(FOR_TRACE_2,"%s <LOCAL>地理位置服务器回应位置,deviceCode:%s,%d,%s\n",
					CTime::GetCurrentTime().Format("%H:%M:%S"),pStrDeviceCode,nQIndex,pStrLocation);

			//nQIndex < -1表示为历史GPS =-1表示是查询我的位置GPS add by kevin 2010-3-22
			if(g_nRecvConntGisClientCount > 0 && nQIndex > 0)
			{
				tempServerData.length = sizeof(ReptLocationData);
				tempServerData.nAppID = SOCKET_SERVER_LOCATION_TYPEID;
				
				ReptLocationData *pTempLocationData = NULL;
				pTempLocationData = (ReptLocationData *)tempServerData.Data;

				pTempLocationData->nQueryIndex = nQIndex;
		
				_snprintf(pTempLocationData->DeviceCode,MAX_LEN_DEVICECODE -1,"%s",pStrDeviceCode);
				_snprintf(pTempLocationData->Location,MAX_LEN_LOCATION_INFO - 1,"%s",pStrLocation);

				g_csRecvServerList.Lock();
				g_RecvServerList.AddTail(tempServerData);
				g_csRecvServerList.Unlock();
				PostThreadMessage(g_dwWorkThreadID,WM_RECVICE_SERVER_MESSAGE,0,0);
			}
#ifndef __SERVICE_MODE_ 
			else
			{
				printf("主控没有GIS客户端连接[收地理位置]\n");
			}
#endif		
			if(nQIndex != -1)
			{
				ReportWebServerUpDataLocation(nQIndex,pStrDeviceCode,pStrLocation);
			}
		}
		else
		{
			gWriteLogApp.WriteLog(FOR_ALARM,"%s <LOCAL>无法识别的地理位置服务器数据\n",
				CTime::GetCurrentTime().Format("%H:%M:%S"));
		}
		break;
	}
	default:
		return;
	}
}
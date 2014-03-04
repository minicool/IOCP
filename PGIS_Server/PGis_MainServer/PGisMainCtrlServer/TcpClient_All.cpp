// TcpClient.cpp: implementation of the Crwini class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TcpClient.h"
#include "MemoryPool.h"

#ifdef WANGZH_ALLP //2013-4-23
#include "PGisMainCtrlServer.h"
extern CPgisMainCtrlProc gPGisMainCtrlProc;
#endif

extern CWriteLogThread gWriteLogApp;
extern ::CCriticalSection g_csRecvClientList;
extern CList<RECV_CLIENTDATA, RECV_CLIENTDATA&> g_RecvClientList;//?
extern UINT g_dwWorkThreadID;

extern UINT g_dwWebThreadID;
extern CMemoryPool gWebServiceList;
extern int gRecvGpsCount;
//////////////////////////////////////////////////////////////////////
//
CTcpClient_All::CTcpClient_All()
{
    Reset();
}

//
CTcpClient_All::~CTcpClient_All()
{
    
}

//////////////////////////////////////////////////////////////////////
//
BOOL CTcpClient_All::AnalyseRecvData(const char *recvData ,int nRecvLen,int nAppID)
{
    if(m_nBufLenCount + nRecvLen > MAX_RECVICE_BUFFER)
    {
        memset(m_RecvBuf,0,MAX_RECVICE_BUFFER);

        return FALSE;
    }
    
    memcpy(m_RecvBuf+m_nBufLenCount,recvData,nRecvLen);
    m_nBufLenCount = m_nBufLenCount + nRecvLen;
    
    //分包
    int nSpaceLen = m_nBufLenCount;
    char *pCurPoint = m_RecvBuf;
    int i;
    int nPackageLen = 0;
    while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
    {
        if( pCurPoint[0] != PACKAGE_HEAD_DATA_STX ) //检查包头
        {
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [CTcpClient][ID:%d,APP:%d]包头错:len:%d,recv:%s\n"),
                    CTime::GetCurrentTime().Format(_T("%H:%M:%S")),m_nSocketID,nAppID,nSpaceLen,recvData);

            nSpaceLen = 0;

            break;
        }
        //查找包尾
        int nPoint = 0;
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
				TcpClientDataProcess(pCurPoint+1,nPackageLen-2,nAppID);
            }
            else
            {
                gWriteLogApp.WriteLog( FOR_ALARM,_T("%s :包长度超过允许的最大值,len:%d.\n"),
                    CTime::GetCurrentTime().Format("%H:%M:%S"),
					nPackageLen);
            }
            //剩下的数据继续分包
            pCurPoint = pCurPoint + nPackageLen;
            nSpaceLen = nSpaceLen - nPackageLen;
        }
        else
        {
            if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
            {
                gWriteLogApp.WriteLog( FOR_ALARM,_T("%s 超长的数据包,len:%d.\n"),
                        CTime::GetCurrentTime().Format("%H:%M:%S"),nSpaceLen );
                
                nSpaceLen = 0;
            }
            break;
        }
        
    }//endwhile
    
    //是否有没有处理的数据
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

//GIS客户端 数据长度验证 
BOOL CTcpClient_All::TcpClientDataProcess(const char *recvData, int nRecvLen,int nAppID)
{
	if(nRecvLen < PACKAGE_LEN)
    {
        return FALSE;
    }

	int nTempLen = 0;
	int nCmdType;
    sscanf(recvData, "%4x%2x",&nTempLen,&nCmdType);
	nTempLen = nTempLen - 2;	//去包头包尾
	if(nTempLen != nRecvLen)
	{
		gWriteLogApp.WriteLog( FOR_ALARM,_T("%s 数据长度不一致,data:%s\n"),
			CTime::GetCurrentTime().Format("%H:%M:%S"),recvData );

		return FALSE;
	}

	nRecvLen = nRecvLen - PACKAGE_LEN;	//长度 命令头
	CString strTrace;
	CString strCurrTime;
	strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");

    if(nAppID == SOCKET_GISCLIENT_TYPEID)
    {
        DispatchGisClientRecvData(nCmdType,recvData+PACKAGE_LEN,nRecvLen); 
    }
// #ifdef WANGZH_ALLP //2013-4-23
// 	else if (nAppID == SOCKET_PUBSERVER_TYPEID)
// 	{
// 		gPGisMainCtrlProc.DispatchPubNetClientRecvData(nCmdType,recvData+PACKAGE_LEN,nRecvLen); 	
// 	}
// #endif
	else
    {
        return FALSE;
    }

	return TRUE;
}

//GIS客户端发送过来的数据  
void CTcpClient_All::DispatchGisClientRecvData(int CmdType,const char *RecvData, int nLen)
{
	CString strCurrTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	CString strTrace;
	switch(CmdType)
	{
	case C_TYPE_TICK :
		//strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端心跳包:%s，IP:%s\n"),strCurrTime,
		//	m_nSocketID,
		//	RecvData,
		//	GetConnectIP());
		//gWriteLogApp.WriteLog(FOR_ALARM,strTrace);
		break;
	case C_TYPE_LOGIN:
		strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端登录数据:%s\n"),strCurrTime,
			m_nSocketID,
			RecvData);
		gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		break;
	case C_TYPE_SYNC_REPORT: //同步
		strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端同步数据:%s\n"),strCurrTime,
			m_nSocketID,
			RecvData);
		gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		break;
	case C_TYPE_APPCMD_TERMINAL: //下发应用命令给终端协议
		strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端应用命令数据:%s\n"),strCurrTime,
			m_nSocketID,
			RecvData);
		gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		break;
	case C_TYPE_CTRLCMD_TERMINAL://下发控制命令协议
		strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端发控制命令数据:%s\n"),strCurrTime,
			m_nSocketID,
			RecvData);
		gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		break;
	case C_TYPE_TERMINAL_ALERT_ACK: //下发应答和解除终端报警协议
		strTrace.Format(_T("%s [GIS][ID:%d]接收GIS客户端发应答和解除终端报警数据:%s\n"),strCurrTime,
			m_nSocketID,
			RecvData);
		gWriteLogApp.WriteLog(FOR_TRACE_1,strTrace);
		break;

// #ifdef WANGZH_H6 //2013-5-2
// 	case C_H6_TYPE_TERMINAL_ALERT_ACK:
// 		break;
// 	case C_H6_TYPE_TERMINAL_STATE_ACK:
// 		break;
// 	case C_H6_TYPE_TERMINAL_WAKEUP:
// 		break;
// #endif
// 	default:
// 		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s [GIS][ID:%d]接收数据,无法识别的数据,type:%d,data:%s\n"),
// 			CTime::GetCurrentTime().Format("%H:%M:%S"),
// 			m_nSocketID,
// 			CmdType,
// 			RecvData);

		return ;
	}

	if(g_dwWorkThreadID > 0)
	{
		RECV_CLIENTDATA tempRecvMessage;
		memset(tempRecvMessage.MessagePacket,0,MAX_DATA_PACKAGE_LEN);

		tempRecvMessage.nAppID = SOCKET_GISCLIENT_TYPEID;
		tempRecvMessage.nSockID = m_nSocketID;
		tempRecvMessage.nCmdType = CmdType;

		memcpy(tempRecvMessage.MessagePacket,RecvData,nLen);
		tempRecvMessage.length = nLen; 
		g_csRecvClientList.Lock();
		g_RecvClientList.AddHead(tempRecvMessage);
		//将数据交给工作线程来处理
		if(!PostThreadMessage(g_dwWorkThreadID,WM_RECVICE_CLIENT_MESSAGE,0,0))
		{
			Sleep(1);
			PostThreadMessage(g_dwWorkThreadID,WM_RECVICE_CLIENT_MESSAGE,0,0);
		}
		g_csRecvClientList.Unlock();

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CTcpClient_All::StartTcpTickTimer(int nConnState ,int nTimerVal)
{
    m_bStartTimerFlag = FALSE;
    m_nNowTimerVal = 0;
    m_nConnState = nConnState;
    if(nTimerVal > 0)
    {
        m_nTimerVal = nTimerVal;
        m_bStartTimerFlag = TRUE;
    }
}

void CTcpClient_All::SetSocketConn(int nSocketID,const char *strIp)
{
	m_nSocketID = nSocketID;
	if(strIp != NULL)
	{
		_snprintf(strIpAddress, MAX_IPADDRESS_LEN-1, "%s",strIp);
	}
}

void CTcpClient_All::Reset()
{
    m_bStartTimerFlag = FALSE;
    m_nSocketID = -1;
    m_nNowTimerVal = 0;
    m_nConnState = TCP_DISCONNECT;
	m_nGISClientType = 0;

	memset(strIpAddress,0,MAX_IPADDRESS_LEN);

}
void CTcpClient_All::StopTcpTickTimer()
{
    m_bStartTimerFlag = FALSE;
}

int CTcpClient_All::CheckTimerOver()
{
    if(!m_bStartTimerFlag)
    {
        return 0;
    }

    switch(m_nConnState)
    {
    case TCP_LOGIN_WAIT: //客户端TCP连接成功后等待登录包
        if( ++m_nNowTimerVal >= m_nTimerVal )
        {
            m_bStartTimerFlag = FALSE;
            m_nNowTimerVal = 0;

            return 1;
        }
        break;
    case TCP_LOGIN_SUCCESS: //对心跳只需要应答
        if( ++m_nNowTimerVal > m_nTimerVal )
        {
            m_bStartTimerFlag = FALSE;
            m_nNowTimerVal = 0;

            return 2;
        }
        break;
    default:
        break;
    }

    return 0;
}
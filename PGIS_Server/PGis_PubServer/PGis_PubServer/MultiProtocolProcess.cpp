#include "StdAfx.h"
#include "MultiProtocolProcess.h"
#include "GPSProc.h"

extern CWriteLogThread gWriteLogApp;
extern UINT g_dwDataAnalyseThreadID;

MultiProtocolProcess::MultiProtocolProcess(void)
{
		m_dwThreadID = 0;
}

MultiProtocolProcess::~MultiProtocolProcess(void)
{

}

BOOL MultiProtocolProcess::Start(DWORD dwThread,UINT Protocol,const char* szRemoteIP,int nRemotePort,const char* szLocalIP,int nLocalPort)
{
	m_dwThreadID = dwThread;
	m_nProtocol = Protocol;

	m_nRemotePort = nRemotePort;
	strncpy(m_szRemoteIP,szRemoteIP,strlen(szRemoteIP));

	m_nLocalPort = nLocalPort;
	strncpy(m_szLocalIP,szLocalIP,strlen(szLocalIP));

	switch (m_nProtocol)
	{
	case UDP_PROTOCOL:
		{
			if (!m_iosocketudp.Start(NULL,m_nLocalPort,DataCallBack,(DWORD)this))
			{
				m_iosocketudp.Stop();
				return FALSE;
			}
			break;
		}
	case FTP_PROTOCOL:
		{
			if(!m_ftpconvery.StartFTPConvey(g_dwDataAnalyseThreadID))
			{
				m_ftpconvery.StopFTPConvey();
				return FALSE;
			}
			break;
		}
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
	case UDP_PROTOCOL:
		m_iosocketudp.Stop();
		break;
	case FTP_PROTOCOL:
		{
			if (!m_ftpconvery.StopFTPConvey())
			{
				return FALSE;
			}
			break;
		}
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
	strncpy(sendBuf,Cmdtypelist[0].HeadData,strlen(Cmdtypelist[0].HeadData));
	strncpy(sendBuf+strlen(Cmdtypelist[0].HeadData),senddata,strlen(senddata));
	strncpy(sendBuf+strlen(Cmdtypelist[0].HeadData)+ndatalen+6,Cmdtypelist[0].TailData,strlen(Cmdtypelist[0].TailData));

	TRACE("[DATA_SEND] SendData:%s Datalen:%d SocketID:%d \n",sendBuf,ndatalen,nSocketID);
	gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] [ID:%d] SendData:%s \n",CTime::GetCurrentTime().Format("%H:%M:%S"),nSocketID,sendBuf);
	return ProceSendData(sendBuf,ndatalen+strlen(Cmdtypelist[0].HeadData) + strlen(Cmdtypelist[0].TailData) + 6,nSocketID);
}

BOOL MultiProtocolProcess::ProceSendData(const char* szData,int ndatalen,int nSocketID)
{
	if (szData == NULL)
	{
		return FALSE;
	}

	switch (m_nProtocol)
	{
	case UDP_PROTOCOL:
		{
			if (m_iosocketudp.UdpSendTo((PBYTE)szData,ndatalen,m_szRemoteIP,m_nRemotePort) != ndatalen)
			{
				return FALSE;
			}
			break;
		}
	case FTP_PROTOCOL:
		{
			m_ftpconvery.WriteData(0,szData);
			break;
		}
	default:
		break;
	}	

	return TRUE;
}

DWORD WINAPI MultiProtocolProcess::DataCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance)
{
	MultiProtocolProcess* temp_mpp = (MultiProtocolProcess*)dwInstance;
	switch(uMsg)
	{
	case UDP_DATA:
		{
			TcpServerClient* temp_socketinfo = (TcpServerClient*)dwParam;
			temp_mpp->DataProcessData(temp_socketinfo->nID,temp_socketinfo->pszRecvData,temp_socketinfo->nRecvDataLen);
			break;
		}
	default:
		break;
	}

	return 0;
}

BOOL MultiProtocolProcess::DataProcessData(int nSocketID,const char *recvData ,int nRecvLen)
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
			TRACE(_T("[ID:%d]包头错:len:%d,recv:%s\n"),nSocketID,nSpaceLen,temppHeadCurPoint);
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [MultiProtocolProcess::ProcessData]%s 包头错 %s,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),recvData,temppHeadCurPoint,m_strHead);
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
				AnalyseData(nSocketID,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen,DWORD(this));
				TRACE("[ID:%d]AnalyseData:%s \n",nSocketID,pCurPoint);
			}
			else
			{
				TRACE(_T("[ID:%d]包长度超过允许的最大值:len:%d,recv:%s\n"),nSocketID,nSpaceLen,recvData);
				gWriteLogApp.WriteLog(FOR_ALARM,"%s [MultiProtocolProcess::ProcessData]%s包头错,没有找到%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),pCurPoint,m_strHead);
			}
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen - nHeadPoint;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("[ID:%d]超长的数据包:len:%d,recv:%s\n"),nSocketID,nSpaceLen,recvData);
			}
			else
			{
				TRACE(_T("[ID:%d]没有找到包尾:len:%d,recv:%s\n"),nSocketID,nSpaceLen,pCurPoint);

			}
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

#define MAIN_PUBNET_PACKAGE_LEN         4
#define MAX_PUBNET_PACKAGE_LEN          256
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
BOOL MultiProtocolProcess::AnalyseData(int nSocketID,const char *recvData ,int nRecvLen,DWORD dwInstance)
{
	MultiProtocolProcess* temp_Instance = (MultiProtocolProcess*)dwInstance;

	if(nRecvLen < PACKAGE_PUB_LEN)
	{
		return FALSE;
	}

	int nTempLen = 0;
	int nCmdType = 0;
	sscanf(recvData, "%4x%2x",&nTempLen,&nCmdType);	//|-4-|datalen |-2-|cmdtype

	if(nTempLen != nRecvLen -6)
	{
		TRACE(_T("[ID:%d]数据包长度匹配错误:len:%d,recv:%s\n"),nSocketID,nRecvLen,recvData);
		return FALSE;
	}

	RECV_CLIENTDATA* tempRecvMessage = new RECV_CLIENTDATA;
	memset(tempRecvMessage->MessagePacket,0,MAX_DATA_PACKAGE_LEN); 
	tempRecvMessage->nAppID = 0;
	tempRecvMessage->nSockID = nSocketID;
	tempRecvMessage->nCmdType = nCmdType;
	tempRecvMessage->length = nRecvLen-6; 
	memcpy(tempRecvMessage->MessagePacket,recvData+6,nRecvLen-6);

	if (!PostThreadMessage(temp_Instance->m_dwThreadID,WM_RECVICE_MESSAGE,WPARAM(tempRecvMessage),0))
	{
		delete tempRecvMessage;
	}

	return TRUE;
}


//---------------------------------------------------FTP-------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////
extern const char *g_pcFtpDownFileName; //需要从FTP下载下来文件
typedef enum enumGisMainAppCmd
{
	aAPP_CMD_SetFixGpsDt = 0, //设置固定上传GPS信息时间间隔
	aAPP_CMD_GpsTrace = 1, //即时跟踪命令
	aAPP_CMD_AreaGpsCall = 2,//区域点名
	aAPP_CMD_SendMessage = 3,
	app_MAX = 4,
}GISMAIN_APP_CMD;
static int g_AppCmdTB[app_MAX] = {SET_GPS_TINMEROUT,SET_GPS_MONITOR,SET_GPS_BOUNDCALL,APP_SEND_MESSAGE};
//FTP上传下载
//读从FTP下载下来的文件，与主控的交互信息 DataAnalyseThreadProc
BOOL MultiProtocolProcess::OnReadMainFtpConveyFile()
{
	static int nReadTick = 0;
	if(m_FtpFileReadTimeOut <= 0)
	{
		return FALSE;
	}

	if(nReadTick++ < m_FtpFileReadTimeOut)
	{
		return FALSE;  
	}
	else
	{
		nReadTick = 0;
	}

	char tempFileName[MAX_PATH] = {0};
	char FtpFileName[MAX_PATH] = {0};
	sprintf(FtpFileName,"%s\\%s",m_strFTPDownLoadPatch,g_pcFtpDownFileName);
	CFileFind IsValidFile;
	if(!IsValidFile.FindFile(FtpFileName)) 
	{
		//文件不存在
		nReadTick =  m_FtpFileReadTimeOut - 2;//2秒后再查
		return FALSE;
	}

	sprintf(tempFileName,"%s\\TempMainCtrl.txt",m_strFTPDownLoadPatch);
	BOOL bFlag = TRUE;
	if(IsValidFile.FindFile(tempFileName)) 
	{
		TRY
		{
			CFile::Remove(tempFileName);//删除文件
		}
		CATCH( CFileException, e )
		{
			TRACE("删除文件失败\n");
			bFlag = FALSE;
		}
		END_CATCH

	}

	if(!bFlag)
	{
		return FALSE;
	}
	//重新命名文件
	int nRes;
	nRes = rename(FtpFileName,tempFileName);
	if(nRes != 0)
	{
		return FALSE;
	}
	Sleep(5);

	FILE *fptr;
	if((fptr = fopen(tempFileName, "rb")) == NULL)
	{    
		//打开文件失败
		return FALSE;
	}

	unsigned char rdDataBuf[MAX_GPS_MESSAGE_LEN*2] = {0};
	int nReadLen = 0;
	BOOL bLoop = TRUE;
	int nSeekCount = 0;
	int nOkCount = 0;
	while(bLoop)
	{
		if(fseek(fptr,nSeekCount, SEEK_SET) == 0)
		{
			nReadLen = fread(rdDataBuf,1,MAX_GPS_MESSAGE_LEN, fptr);
			//对读取的数据进行分析
			if(nReadLen > 0)
			{
				int nCurrPoint = 0;
				char strGetNumber[MAX_GPS_MESSAGE_LEN] = {0}; 
				int nNumLen;
				CString strAddNum;

				for(int i = 0; i < nReadLen; i++)
				{
					if(rdDataBuf[i] == '\r' || rdDataBuf[i] == '\n') 
					{
						memset(strGetNumber,0,MAX_GPS_MESSAGE_LEN);
						nNumLen = i - nCurrPoint;
						memcpy(strGetNumber,rdDataBuf+nCurrPoint,nNumLen);
						nCurrPoint = i+1;
						strAddNum.Format(_T("%s"),strGetNumber);
						if(strAddNum.GetLength() > 4)
						{
							if(FTP_MainDataAnalyse(strGetNumber))
							{
								nOkCount++;
							}
						}
					}
				}
				nSeekCount = nSeekCount + nCurrPoint;
				if(nSeekCount == 0) //读了MAX_GPS_MESSAGE_LEN没有找到一行结束
				{
					nSeekCount = MAX_GPS_MESSAGE_LEN;
				}
			}

			if( nReadLen < MAX_GPS_MESSAGE_LEN)
			{
				//文件读完退出
				fclose(fptr);  
				fptr = NULL;
				bLoop = FALSE;
			}
		}
	}

	if(fptr)
	{
		fclose(fptr); 
	}

	gWriteLogApp.WriteLog(FOR_ALARM,_T("%s <FTP>处理FTP主控交换文件,共%d条正确的数据\n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),nOkCount);

	if(IsValidFile.FindFile(tempFileName)) 
	{
		TRY
		{
			CFile::Remove(tempFileName);//删除文件
		}
		CATCH( CFileException, e )
		{
			TRACE("删除文件失败\n");
		}
		END_CATCH
	}


	return TRUE;
}

//分析读文件读到的数据包
BOOL MultiProtocolProcess::FTP_MainDataAnalyse(const char *pstrMainDataPacket)
{
	if(pstrMainDataPacket == NULL)
	{
		return FALSE;
	}

	char tempBuf[MAX_GPS_MESSAGE_LEN] = {0};
	char *pData[20];
	int i;
	for(i = 0; i < 20; i++)
	{
		pData[i] = NULL;
	}

	int nDataLen = strlen(pstrMainDataPacket);
	memcpy(tempBuf,pstrMainDataPacket,nDataLen);
	char *pBuf = tempBuf;
	i = 0;
	int nCount = 0;
	while((pData[i]=strtok(pBuf,",")) != NULL)
	{
		i++;
		nCount++;
		pBuf = NULL; 
	}
	if(nCount < 1)
	{
		return FALSE;
	}

	int nCmdType = 0;
	int nPackageLen;
	if(strlen(pData[0]) < 6)
	{
		return FALSE;
	}
	sscanf(pData[0], "%4x%2x",&nPackageLen,&nCmdType);

	if(nPackageLen != nDataLen - 6)
	{
		//协议格式错误
		return FALSE;
	}

//	int nClientID = GetClientID(pData[0]+6);
	int nMoveLen = 6;

	RECV_CLIENTDATA* tempRecvMessage = new RECV_CLIENTDATA;
	memset(tempRecvMessage->MessagePacket,0,MAX_DATA_PACKAGE_LEN);
	tempRecvMessage->nAppID = 0;
//	tempRecvMessage->nSockID = nClientID;
	tempRecvMessage->nCmdType = nCmdType;
	tempRecvMessage->length = nPackageLen; 
	memcpy(tempRecvMessage->MessagePacket,pstrMainDataPacket+nMoveLen,tempRecvMessage->length);

	if (!PostThreadMessage(m_dwThreadID,WM_RECVICE_MESSAGE,WPARAM(tempRecvMessage),0))
	{
		delete tempRecvMessage;
	}

//	TRACE("[ID:%d]Cmdtype:%d Recvlen:%d Recvdata:%s\n",nClientID,nCmdType,nPackageLen,pstrMainDataPacket+nMoveLen);

	return TRUE;	

}
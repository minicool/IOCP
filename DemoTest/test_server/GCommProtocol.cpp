
#include "stdafx.h"

#include <winsock2.h>
#include <mswsock.h>

#include "GLog.h"
#include "GWorkerThread.h"
#include "GPerIoData.h"
#include "GPerHandleData.h"
#include "GSocketInside.h"
#include "GSocket.h"

#include "Interface.h"

char m_RecvBuf[MAX_RECVICE_BUFFER];
int m_nBufLenCount;	//用于数据包处理
unsigned int m_nAppID = 0;
const int MAX_DATA_PACKAGE_LEN = 256;
const int MAIN_DATA_PACKAGE_LEN =  8;
BOOL GCommProt_ProcessReceive(PGHND_DATA pHndData, char* recvData, DWORD nRecvLen, PFN_ON_GHND_DATA_EVENT pfnOnProc)
{
	//在这里使用GCommProtocol处理粘包
	//......

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
	char *m_strHead =  "*HITOM"/*m_Cmdtypelist[m_nAppID].HeadData*/;
	unsigned int temp_nHeadDataLen = 6/*strlen(m_strHead)*/;
	char *m_strTail =  "#"/*m_Cmdtypelist[m_nAppID].TailData*/;
	unsigned int temp_nTailDataLen = 1/*strlen(m_strTail)*/;
	char *ptempCurPoint = pCurPoint;
	while(nSpaceLen >= MAIN_DATA_PACKAGE_LEN)
	{
		if(strncmp(pCurPoint,m_strHead,temp_nHeadDataLen) != 0) //检查包头
		{
			TRACE(_T("包头错:len:%d,recv:%s\n"),nSpaceLen,recvData);
			nSpaceLen = 0;

			break;
		}
		//查找包尾
		int nPoint = 0;
		for(int i = 0 ; i < nSpaceLen; i++ )
		{
			if(strncmp(ptempCurPoint,m_strTail,temp_nTailDataLen) != 0) //检查包头
			{
				++ptempCurPoint;
			}
			else
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
				pCurPoint[nPackageLen-1] = NULL;
				if (NULL != pfnOnProc)
				{
					pfnOnProc((DWORD)pHndData,pCurPoint+temp_nHeadDataLen,nPackageLen-temp_nHeadDataLen-temp_nTailDataLen);
				}
			}
			else
			{
				TRACE(_T("包长度超过允许的最大值:len:%d,recv:%s\n"),nSpaceLen,recvData);
			}
			//剩下的数据继续分包
			pCurPoint = pCurPoint + nPackageLen;
			nSpaceLen = nSpaceLen - nPackageLen;
		}
		else
		{
			if( nSpaceLen > MAX_DATA_PACKAGE_LEN )
			{
				TRACE(_T("超长的数据包:len:%d,recv:%s\n"),nSpaceLen,recvData);
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

	//pfnOnProc((DWORD)pHndData, pBuf, dwBytes);
	//return(FALSE);
}

BOOL GCommProt_PostSend(PGHND_DATA pHndData, PGIO_DATA pIoData)
{
	DWORD dwBytes;
	pIoData->OperType = GIO_WRITE_COMPLETED;
	if((SOCKET_ERROR == WSASend(pHndData->Socket, &(pIoData->WSABuf), 1, &dwBytes, 0, LPWSAOVERLAPPED(pIoData), NULL)) &&
		(WSAGetLastError() != ERROR_IO_PENDING))
	{
		PostQueuedCompletionStatus(hGWkrThrdCompletionPort, 0, (DWORD)pHndData, LPWSAOVERLAPPED(pIoData));
		return(FALSE);
	}else
		return(TRUE);
}

BOOL GCommProt_PostSendGBuf(DWORD dwClientContext, PGIO_BUF pGBuf, DWORD dwBytes)
{
	pGBuf = (PGIO_BUF)((char *)pGBuf - dwGBufOffset);
	((PGIO_DATA)pGBuf)->WSABuf.len = dwBytes;
	return(GCommProt_PostSend((PGHND_DATA)dwClientContext, (PGIO_DATA)pGBuf));
}

BOOL GCommProt_PostSendBuf(DWORD dwClientContext, char* pBuf, DWORD dwBytes)
{
	//在这里使用GCommProtocol处理发送
	//if(dwBytes > dwGBufSize)
	//......

// 	char sendBuf[MAX_DATA_PACKAGE_LEN] = {0};
// 	CopyMemory(sendBuf,m_Cmdtypelist[m_nAppID].HeadData,strlen(m_Cmdtypelist[m_nAppID].HeadData));
// 	CopyMemory(sendBuf+strlen(m_Cmdtypelist[m_nAppID].HeadData),pBuf,dwBytes);
// 	CopyMemory(sendBuf+strlen(m_Cmdtypelist[m_nAppID].HeadData)+dwBytes,m_Cmdtypelist[m_nAppID].TailData,strlen(m_Cmdtypelist[m_nAppID].TailData));

	PGIO_DATA pIoData;

	pIoData = GIoDat_Alloc();
	if(!pIoData)
	{
		GLog_Write("GSock_PostSendBuf：分配IoData失败，无法投递发送请求");
		return(FALSE);
	}
// 	CopyMemory(&(pIoData->cData), sendBuf, dwBytes + sizeof(m_Cmdtypelist[m_nAppID].HeadData) + sizeof(m_Cmdtypelist[m_nAppID].TailData));
// 	pIoData->WSABuf.len = dwBytes + sizeof(m_Cmdtypelist[m_nAppID].HeadData) + sizeof(m_Cmdtypelist[m_nAppID].TailData);
	CopyMemory(&(pIoData->cData), pBuf, dwBytes);
	pIoData->WSABuf.len = dwBytes;
	return(GCommProt_PostSend((PGHND_DATA)dwClientContext, pIoData));
}

void GCommProt_Init(void)
{
	GIoDat_SetPackHeadSize(0);
	GIoDat_SetPackTailSize(0);
}




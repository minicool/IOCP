#include "StdAfx.h"
#include "DBManage_WebServer.h"
#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;

extern UINT g_dwWorkThreadID;
extern UINT g_dwWebThreadID;
extern int gRecvGpsCount;
extern int g_nRecvConntGisClientCount;
extern int gWriteGpsConnt;

extern int g_nSendLocationGPSCount;
extern int g_nRecvLocationGPSCount;

CDBManage_WebServer::CDBManage_WebServer(void)
{
	m_bThreadStart = FALSE;
	m_bWebConnOK = FALSE;

}

CDBManage_WebServer::~CDBManage_WebServer(void)
{

}


//连接WebService
BOOL CDBManage_WebServer::StartWebService()
{
	//初时化COM
	::CoInitialize(NULL);

	//初始化队列缓冲区
	if(!gWebServiceList.InitBuffer(MAX_WEBSERVICE_LISTBUF_LEN))
	{
		return FALSE;
	}
	gWebServiceList.ClearBuf();

	m_pWebService = new CLocationData; // 代理对象 
	if(m_pWebService == NULL)
	{
		return FALSE;
	}
	
	LoadConfig();

	m_pWebService->SetUrl(m_strWebServiceURL);

	Sleep(50);

	m_bWebServiceConn = TRUE;
	
	WebServer_LoadDB_PatrolDeviceInfo(FALSE);

	return TRUE;
}

//释放WebService对象
void CDBManage_WebServer::FreeWebService()
{
	if(m_pWebService)
	{
		delete m_pWebService;
		m_pWebService = NULL;
	}

	m_bWebServiceConn = FALSE;
	gWebServiceList.FreeBuf();		//释放队列缓冲区
}

BOOL CDBManage_WebServer::LoadConfig()
{
	Crwini rwIniTemp;

	if(!rwIniTemp.CheckIniFile(gPGisMainCtrlProc.g_strConfigFile))
	{
		gWriteLogApp.WriteLog(FOR_ALARM,"%s [SYS] 系统启动失败,找不到配置文件\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
		return FALSE;
	}

	if(NULL != rwIniTemp.ReadString(_T("DBServer"),_T("WebServiceURL"),gPGisMainCtrlProc.g_strConfigFile,m_strWebServiceURL.GetBuffer(256)))
	{
		m_strWebServiceURL.ReleaseBuffer();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDBManage_WebServer::StartThread()
{
	if( m_bThreadStart )
	{
		TRACE(_T("System thread has run.\n"));
		return TRUE;
	}

	if( m_hWebThreadEndEvt == 0 )
	{
		m_hWebThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);
	}

	m_hWebThread = (HANDLE)_beginthreadex( NULL,0,&WebServiceThreadProc,this, 0,&m_dwWebThreadID );
	if( !m_hWebThread )
	{
		TRACE( _T("Strat WebService thread fail.\n"));
		return FALSE;
	}
	else
	{
		::SetThreadPriority(m_hWebThread,/*THREAD_PRIORITY_ABOVE_NORMAL*/THREAD_PRIORITY_HIGHEST);
	}

	g_dwWebThreadID = m_dwWebThreadID;
	m_bThreadStart = TRUE;
	return TRUE;
}

void CDBManage_WebServer::StopThread()
{
	if(m_bThreadStart)
	{
		PostThreadMessage(m_dwWebThreadID, WM_THREAD_STOP, 0L, 0L);
		if(WaitForSingleObject(m_hWebThreadEndEvt,INFINITE) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hWebThread,0);
			TRACE(_T("WebService线程强制终止 \n"));
		}
		CloseHandle(m_hWebThreadEndEvt);
		m_hWebThreadEndEvt = NULL;
		m_hWebThread =NULL;
		m_bThreadStart = FALSE;
	}

}

UINT WINAPI CDBManage_WebServer::WebServiceThreadProc(void *lpParameter)
{
	CDBManage_WebServer *pDBManage_WebServer = (CDBManage_WebServer *)lpParameter;
	TRACE( _T("Into WebService thread%X\n"), pDBManage_WebServer->m_hWebThread );
    pDBManage_WebServer->StartWebService();

	ResetEvent(pDBManage_WebServer->m_hWebThreadEndEvt);
	MSG msg;
	BOOL bcontinue = TRUE;
	while(bcontinue)
	{
		if( GetMessage(&msg,NULL,0,0) )
		{
			switch(msg.message)
			{
			case WM_THREAD_STOP:
				bcontinue = FALSE;
				break;
			case WM_WEB_SERVICE_MESSAGE:
				pDBManage_WebServer->OnWebServiceCmd(); 
				break;
//			case WM_WEB_LOADPATROL_MESSAGE: //巡防路线修改,重新LOAD寻访终端信息
//				pDBManage_WebServer->WebServerLoadPatrolDevice(TRUE);
//				break;
			default:
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
			}
		}
	}

	SetEvent(pDBManage_WebServer->m_hWebThreadEndEvt);

	pDBManage_WebServer->FreeWebService();
	TRACE( _T("Quit WebService thread\n") );
	_endthreadex(0);

	return 0;
}

BOOL CDBManage_WebServer::OnWebServiceCmd()
{
	POSITION pos = NULL;
	//WEB_MESSAGE tmpWebMsg;
	static int nFailCount = 0;
	char tempbuf[MAX_DATA_PACKAGE_LEN] = {0};
	int nTempBufLen =  gWebServiceList.GetFromBuffer(tempbuf);
	if(nTempBufLen == 0)
	{
		return FALSE;
	}

	int nCmdType = tempbuf[0];
	CString strTempNum;
	bool bReturnFlag = false;
	switch(nCmdType)
	{
	case WEB_QUERY_INFO:
		{
			CString strUserCode;
			int nUserType = 0;
			CString strDeptNum;

			strTempNum.Format(_T("%s"),tempbuf+2);
			if(WebService_QueryDB_PubTerminalInfo(strTempNum,nUserType,strUserCode,strDeptNum))
			{
				bReturnFlag = true;
//				WriteTerminalUserInfoToList(strTempNum,nUserType,strUserCode,strDeptNum);
				CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
				ZeroMemory(&temp_TermainalInfo,sizeof(CBusinessManage_ConToPub::TERMINAL_INFO));
				if (gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(strTempNum.GetBuffer(),temp_TermainalInfo))
				{
					gPGisMainCtrlProc.m_BusinessManage_ConToPub.AddPubTerminal(temp_TermainalInfo.szDeviceNum,temp_TermainalInfo.szDeviceType,strDeptNum.GetBuffer(),strUserCode.GetBuffer(),nUserType,temp_TermainalInfo.nLineState,temp_TermainalInfo.bPatrolStart);
					gWriteLogApp.WriteLog(FOR_TRACE,_T("%s WEBService查询设备信息,DeviceCode:%s,UserType:%d,UserCode:%s,DeptNum:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),strTempNum,nUserType,strUserCode,strDeptNum);
				}
				nFailCount = 0;
			}
			else
			{
				if(tempbuf[1] == WEB_FAIL_AFRESH_NO)
				{
					bReturnFlag = true;
					gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WEBService查询终端信息失败,DeviceCode:%s\n"),CTime::GetCurrentTime().Format("%H:%M:%S"),tempbuf+2);
				}
			}

			if(!bReturnFlag)
			{
				tempbuf[1] = WEB_FAIL_AFRESH_NO;
			}
			else
			{
				SetEvent(m_hWaitResultEvt); //有事件
			}
			break;
		}
	case WEB_SET_STATE:
		{
			strTempNum.Format(_T("%s"),tempbuf+3);
			CString strState;
			strState.Format(_T("%d"),tempbuf[2]);

			if(WebService_UpdataDB_PubTerminalInfo_DeviceOnLineStatus(strTempNum,strState))
			{
				bReturnFlag = true;

				if(tempbuf[2] > 0)
				{
					gWriteLogApp.WriteLog(FOR_TRACE,_T("%s WEBService设置设备状态为在线,DeviceCode:%s\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),
						strTempNum);
				}
				else
				{
					gWriteLogApp.WriteLog(FOR_TRACE,_T("%s WEBService设置设备状态为离线,DeviceCode:%s\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),
						strTempNum);
				}

				nFailCount = 0;
			}
			else
			{
				if(tempbuf[1] == WEB_FAIL_AFRESH_NO) //不需要再查一次
				{
					bReturnFlag = true;

					gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WEBService设置设备状态失败,DeviceCode:%s,State:%s\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),
						strTempNum,
						strState);
				}	
			}

			if(!bReturnFlag)
			{
				tempbuf[1] = WEB_FAIL_AFRESH_NO;
			}
			else
			{
				SetEvent(m_hWaitResultEvt); //有事件
			}

			break;
		}
	case WEB_WRITE_GPSDDATA:
		{
			DWORD dwCmdType = 0;
			sscanf(tempbuf+2, "%8x",&dwCmdType);
			if(WebService_WriteDB_GpsData(dwCmdType,tempbuf+10,nTempBufLen-10))
			{
				bReturnFlag = true;

				nFailCount = 0;
			}
			else
			{
				if(tempbuf[1] == WEB_FAIL_AFRESH_NO) //不需要再写一次数据库
				{
					bReturnFlag = true;

					gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WEBService将GPS数据写入数据库失败,data:%s\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),tempbuf+2);

					nFailCount++;
				}
			}

			if(!bReturnFlag)
			{
				tempbuf[1] = WEB_FAIL_AFRESH_NO;
			}
			else
			{
				SetEvent(m_hWaitResultEvt); //有事件
			}
			break;
		}
	case WEB_REPT_SENDMSG:
		{
			WebReportSendMsgOK *pWebReptAlarmMsg = NULL;

			pWebReptAlarmMsg = (WebReportSendMsgOK *)(tempbuf+2);
			if(WebService_WriteDB_AlarmMessage(pWebReptAlarmMsg->nMsgID,pWebReptAlarmMsg->TerminalNum))
			{
				bReturnFlag = true;
			}
			else
			{
				if(tempbuf[1] == WEB_FAIL_AFRESH_NO) //不需要再写一次数据库
				{
					bReturnFlag = true;

					gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WEBService记录下发文字到数据库失败,MsgID:%d\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),pWebReptAlarmMsg->nMsgID);
				}
			}

			if(!bReturnFlag)
			{
				tempbuf[1] = WEB_FAIL_AFRESH_NO;
			}
			break;
		}
	case WEB_UPDATA_LOCATION: //更新地理位置
		{
			ReptLocationDataHead *pTempLocationHead = NULL;
			pTempLocationHead = (ReptLocationDataHead *)(tempbuf+2);
			int nTempLen = sizeof(ReptLocationDataHead) + 2;
			if(nTempBufLen > nTempLen)
			{
				if(WebService_UpDataDB_Location(pTempLocationHead->nQueryIndex,pTempLocationHead->DeviceCode,tempbuf+nTempLen))
				{
					bReturnFlag = true;
				}
				else
				{
					bReturnFlag = true;
					gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WEBService更新数据库地理位置失败,DeviceCode:%s\n"),
						CTime::GetCurrentTime().Format("%H:%M:%S"),
						pTempLocationHead->DeviceCode);
				}
			}
			else
			{
				bReturnFlag = true;
			}

			if(!bReturnFlag)
			{
				tempbuf[1] = WEB_FAIL_AFRESH_NO;
			}

			break;
		}
	default:
		bReturnFlag = true;
		break;
	}

	//
	if(!bReturnFlag)
	{
		gWebServiceList.SendToBuffer(tempbuf,nTempBufLen);
		::PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0);
	}

	if(nFailCount > 5)
	{
		//连续三次GPS写库失败认为WebService连接断开,向GIS客户断报告WEBService连接端
		PostThreadMessage(g_dwWorkThreadID,WM_SERV_CONNERROR_MESSAGE,CASE_WEBSER_CONNERROR,0);
		nFailCount = 0;
	}
	if(!bReturnFlag)
	{
		return FALSE;
	}

	return TRUE;

}

void CDBManage_WebServer::ReportWebServiceDeviceOnLine(const char *strDrivceID)
{
	if(m_bWebServiceConn)
	{
		int nLen;
		int nPackageLen;
		char tempPacket[MAX_DATA_PACKAGE_LEN] = {0};
		tempPacket[0] = WEB_SET_STATE; //设置状态
		tempPacket[1] = WEB_FAIL_AFRESH_YES; //WEBService写库失败是否重写
		tempPacket[2] = LINE_ON;

		nLen = strlen(strDrivceID);
		memcpy(tempPacket+3,strDrivceID,nLen);
		nPackageLen = nLen + 3;

		if(!gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
		{
			//TRACE("WebService接收队列缓冲区已满\n");
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WebService接收队列缓冲区已满\n"),
				CTime::GetCurrentTime().Format("%H:%M:%S"));

			return ;
		}
		PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0);
	}
}

//GIS客户端发送文字消息，通知WebService发送文字信息成功，WebService更新数据库
void CDBManage_WebServer::ReportWebSendMessage(int MsgID,const char * strDeviceCode)
{
	if(m_bWebServiceConn)
	{
		int nPackageLen ;
		char tempPacket[MAX_DATA_PACKAGE_LEN] = {0};
		WebReportSendMsgOK *pWebReptSendMsg = NULL;

		tempPacket[0] = WEB_REPT_SENDMSG;
		tempPacket[1] = WEB_FAIL_AFRESH_YES;
		pWebReptSendMsg = (WebReportSendMsgOK *)(tempPacket+2);

		pWebReptSendMsg->nMsgID = MsgID;
		strcpy(pWebReptSendMsg->TerminalNum,strDeviceCode);

		nPackageLen =sizeof(WebReportSendMsgOK) + 2;

		if(gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
		{
			PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0);
		}
		else
		{
			//TRACE("WebService接收队列缓冲区已满\n");
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WebService接收队列缓冲区已满\n"),
				CTime::GetCurrentTime().Format("%H:%M:%S"));
		}
	}
}

//通知WebService终端上下线
void CDBManage_WebServer::ReportWebServiceTerminalLine(const char *strDrivceID,int nState)
{
	if(m_bWebServiceConn)
	{
		int nLen;
		int nPackageLen;
		char tempPacket[MAX_DATA_PACKAGE_LEN] = {0};
		tempPacket[0] = WEB_SET_STATE; //设置状态
		tempPacket[1] = WEB_FAIL_AFRESH_YES; //WEBService写库失败是否重写
		tempPacket[2] = nState;

		nLen = strlen(strDrivceID);
		memcpy(tempPacket+3,strDrivceID,nLen);
		nPackageLen = nLen + 3;

		if(!gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
		{
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WebService接收队列缓冲区已满\n"),
				CTime::GetCurrentTime().Format("%H:%M:%S"));

			return ;
		}
		if(PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0))
		{
			//等待,与WEBService处理线程保持同步
//			StartWaitResult();
		}

		if(nState == LINE_ON)
		{
			tempPacket[0]  = WEB_QUERY_INFO; //查询设备信息
			tempPacket[1] = WEB_FAIL_AFRESH_YES;
			memcpy(tempPacket+2,strDrivceID,nLen);
			nPackageLen = nLen + 2;

			if(gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
			{
				if(PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0))
				{
					//等待,与WEBService处理线程保持同步
//					StartWaitResult();
				}
			}
		}
	}
}

//通知WebService更新地理位置信息
void CDBManage_WebServer::ReportWebServer_UpDataLocation(int nQueryIndex,char *strDeviceCode ,char *strLocation)
{
	if(g_dwWebThreadID > 0)
	{
		int nPackageLen = 0;
		char tempPacket[MAX_DATA_PACKAGE_LEN] = {0};

		ReptLocationDataHead *pDataHead = NULL;

		tempPacket[0] = WEB_UPDATA_LOCATION;
		tempPacket[1] = WEB_FAIL_AFRESH_NO;
		pDataHead = (ReptLocationDataHead *)(tempPacket + 2);
		pDataHead->nQueryIndex = nQueryIndex;
		strcpy(pDataHead->DeviceCode,strDeviceCode);

		nPackageLen = 2 + sizeof(ReptLocationDataHead);
		int nTempLen =  strlen(strLocation);
		memcpy(tempPacket+nPackageLen,(char *)strLocation,nTempLen);

		nPackageLen = nPackageLen + nTempLen + 1;
		if(gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
		{
			PostThreadMessage(g_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0);
		}
		else
		{
			gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WebService接收队列缓冲区已满[收地理位置]\n"),
				CTime::GetCurrentTime().Format("%H:%M:%S"));
		}
	}
}

void CDBManage_WebServer::ReportWebServiceRecvGps(const DWORD dwClientContext,const char *pRecvData,int nLen)
{
	int nPackageLen ;
	char tempPacket[MAX_DATA_PACKAGE_LEN] = {0};
	tempPacket[0] = WEB_WRITE_GPSDDATA;
	tempPacket[1] = WEB_FAIL_AFRESH_NO; //

	sprintf(tempPacket+2,"%.8X",dwClientContext);
	memcpy(tempPacket+10,pRecvData,nLen);
	nPackageLen = nLen + 10;
	
	if(gWebServiceList.SendToBuffer(tempPacket,nPackageLen))
	{
		if(PostThreadMessage(m_dwWebThreadID,WM_WEB_SERVICE_MESSAGE,0,0))
		{
			//等待,与WEBService处理线程保持同步
			StartWaitResult();
			gRecvGpsCount++;
		}
	}
	else
	{
		gWriteLogApp.WriteLog(FOR_ALARM,_T("%s WebService接收队列缓冲区已满\n"),
			CTime::GetCurrentTime().Format("%H:%M:%S"));
	}
}

void CDBManage_WebServer::StartWaitResult()
{
	switch(WaitForSingleObject(m_hWaitResultEvt, 3000))
	{
	case WAIT_OBJECT_0:
		ResetEvent(m_hWaitResultEvt);
		break;
	case WAIT_TIMEOUT:
		break;
	default:
		break;
	}
}


//通过WebService查询在巡防的终端信息，当终端发来GPS数据时，用来判断该终端是否在巡防，
//如果是还要将该GPS数据发给巡防管理服务
BOOL CDBManage_WebServer::WebServer_LoadDB_PatrolDeviceInfo(BOOL isUpdateOnLineDeviceList)
{
	BOOL bRet = FALSE;
	
	if (!gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_PatrolDevListData.IsEmpty())
	{
		gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_PatrolDevListData.ClearAll();
	}
	
	if(m_bWebServiceConn)
	{
		MSXML2::IXMLDOMDocumentPtr pDoc;
		HRESULT	hr;
		hr=pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		if(FAILED(hr))
		{  
			gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [SYS] 无法创建DOMDocument对象，请检查是否安装了MS XML Parser 运行库\n"),CTime::GetCurrentTime().Format("%H:%M:%S"));
			return FALSE;
		}

		CComBSTR hiResult;
		m_pWebService->HYTPGIS_PatroAreaUser_QueryAllDevice(&hiResult);
		char *pStr = NULL;
		pStr = _com_util::ConvertBSTRToString(hiResult);
		CString strXML;
		strXML.Format("<?xml version=\"1.0\" encoding=\"utf-8\" ?><DataSet xmlns=\"http://tempuri.org/\"> %s</DataSet>",pStr);

		VARIANT_BOOL bRet = pDoc->loadXML((_bstr_t)strXML);
		if(bRet == -1)
		{
			MSXML2::IXMLDOMNodeListPtr	 pListNode;
			pListNode = pDoc->selectNodes("//Table");
			if(pListNode != NULL)
			{
				int i;
				long tLen;
				pListNode->get_length(&tLen);
				CString strTemp;

				for(i = 0 ; i < tLen; i++)
				{
					int j = 0;
					MSXML2::IXMLDOMNode	 *pTempNode;
					pListNode->get_item(i,&pTempNode);

					MSXML2::IXMLDOMNodeList *TempChildList;
					pTempNode->get_childNodes(&TempChildList);

					long listCount = 0;
					TempChildList->get_length(&listCount);

					MSXML2::IXMLDOMNode *tempListItem = NULL;
					_variant_t	tempValue;
					for(j = 0 ; j < listCount; j++)
					{
						TempChildList->get_item(j,&tempListItem);

						tempListItem->get_nodeTypedValue(&tempValue);
						strTemp.Format("%s",(char *)(_bstr_t)tempValue);
						strTemp = strTemp.TrimLeft();
						strTemp = strTemp.TrimRight();
						gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_PatrolDevListData.AddItem(strTemp);
						if(isUpdateOnLineDeviceList) //当巡防路线修改时为TRUE,此时要更新在线终端的巡防状态字段
						{
							gPGisMainCtrlProc.m_BusinessManage_ConToPub.Update_OnlineDeviceList_PatrolState(strTemp);	//
						}
						TRACE("添加巡防用户信息 %s \n",strTemp);
					}
				} //end for(i = 0

				bRet = TRUE;
                gWriteLogApp.WriteLog( FOR_ALARM,_T("%s [SYS] 加载巡防终端信息 %d 成功 \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),tLen);
				m_bWebConnOK = TRUE;
			}
		}

		if(pStr)
		{
			delete []pStr;
			pStr = NULL;
		}
	}

	return bRet;
}

//WebServicer查询终端信息
BOOL CDBManage_WebServer::WebService_QueryDB_PubTerminalInfo(CString strDeviceCode,int &nUserType,CString &strUserCode,CString &strDeptNum)
{
	bool bReturnFlag = false;

	DeviceInfoClass TestDeviceClass;	
	static int nFailCount = 0;

	BSTR szDeviceCode = strDeviceCode.AllocSysString();

	m_pWebService->HYTPGIS_DeviceTable_Query(szDeviceCode,&bReturnFlag,&TestDeviceClass);
	::SysFreeString(szDeviceCode);

	if(bReturnFlag)
	{
		char *pStrUserCode = NULL;
		pStrUserCode = _com_util::ConvertBSTRToString(TestDeviceClass.USER_CODE);
		if(pStrUserCode)
		{
			strUserCode.Format(_T("%s"),pStrUserCode);
			delete[] pStrUserCode;
			pStrUserCode = NULL;
		}
		char *pStrUserType = NULL;
		pStrUserType = _com_util::ConvertBSTRToString(TestDeviceClass.USER_TYPE);
		if(pStrUserType)
		{
			nUserType = atoi(pStrUserType);
			delete[] pStrUserType;
			pStrUserType = NULL;
		}

		char *pStrDeptNum = NULL;
		pStrDeptNum = _com_util::ConvertBSTRToString(TestDeviceClass.DEPT_NO);
		if(pStrDeptNum)
		{
			strDeptNum.Format(_T("%s"),pStrDeptNum);
			delete[] pStrDeptNum;
			pStrDeptNum = NULL;
		}
	}
	else
	{
		//WEBService查询失败
		return FALSE;
	}

	return TRUE;
}

//更新数据库中终端在线状态
BOOL CDBManage_WebServer::WebService_UpdataDB_PubTerminalInfo_DeviceOnLineStatus(CString strDeviceCode,CString strState)
{
	bool bReturnFlag = false;

	BSTR DEVICE_CODE = strDeviceCode.AllocSysString();
	BSTR OnLineStatus = strState.AllocSysString();

	m_pWebService->HYTPGIS_DeviceModify_OnLineStatus(DEVICE_CODE,OnLineStatus,&bReturnFlag);
	::SysFreeString(DEVICE_CODE);
	::SysFreeString(OnLineStatus);

	if(!bReturnFlag)
	{
		return FALSE;
	}

	return TRUE;
}

//WebServicer将巡防告警信息写入数据库
BOOL CDBManage_WebServer::WebService_WriteDB_AlarmMessage(const int nMsgID,const char * pszDeviceID)
{
	bool bReturnFlag = false;
	
	CString strDeviceID;
	strDeviceID.Format("%s",pszDeviceID);
	BSTR szDeviceCode = strDeviceID.AllocSysString();

	CString strChnlType = _T("GPRS");
	BSTR szChannel = strChnlType.AllocSysString();

	m_pWebService->HYTPGIS_AlarmDevice_Add(nMsgID,szDeviceCode,szChannel,&bReturnFlag);

	::SysFreeString(szDeviceCode);
	::SysFreeString(szChannel);

	if(!bReturnFlag)
	{
		return FALSE;
	}

	return TRUE;
}

//WebServicer更新数据库中地理位置信息
BOOL CDBManage_WebServer::WebService_UpDataDB_Location(int nIndex,const char *strDevice,const char *strLocation)
{
	bool bReturnFlag = false;

	CString strTempDeviceCode;
	strTempDeviceCode.Format("%s",strDevice);
	BSTR szDeviceCode = strTempDeviceCode.AllocSysString();

	CString strTempLocation;
	strTempLocation.Format("%s",strLocation);
	BSTR szLocation = strTempLocation.AllocSysString();

	m_pWebService->HYTPGIS_LocationData_ModifyByLocation(nIndex,szDeviceCode,szLocation,&bReturnFlag);
	::SysFreeString(szDeviceCode);
	::SysFreeString(szLocation);

	if(!bReturnFlag)
	{
		return FALSE;
	}

	return TRUE;
}

//WebService处理GPS数据，GPS数据写库
BOOL CDBManage_WebServer::WebService_WriteDB_GpsData(CString strDeviceID,const int nDeviceType,const char *pGpsData,int nLen)
{
	return FALSE;
}

//WebService处理GPS数据，GPS数据写库
BOOL CDBManage_WebServer::WebService_WriteDB_GpsData(const DWORD dwClientContext,const char *pRecvData,int nLen)
{
	char *pData[16];
	for(int i = 0; i < 16; i++)
	{
		pData[i] = NULL;
	}

	char temp[MAX_DATA_PACKAGE_LEN] ={0};
	memcpy(temp,pRecvData,nLen);

	char *pBuf = temp;
	int i = 0;
	while((pData[i]=strtok(pBuf,",")) != NULL)
	{
		i++;
		pBuf = NULL;
	}
	if(i != 12)
	{
		return FALSE ;
	}

	//查询设备信息
	CString strUserCode = _T(" ");
	CString strDeptNum = _T(" ");

	CString strDeviceCode;
	strDeviceCode.Format("%s",pData[0]);
	int nGpsDataType = atoi(pData[1]);

	CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
	ZeroMemory(&temp_TermainalInfo,sizeof(CBusinessManage_ConToPub::TERMINAL_INFO));
	gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(strDeviceCode.GetBuffer(),temp_TermainalInfo);

	int nUserType = temp_TermainalInfo.nUserType;
	strUserCode.Format("%s",temp_TermainalInfo.szUserCode);
	strDeptNum.Format("%s",temp_TermainalInfo.szDeptNum);

	//时间 HHMMSS
	int nHH = 0;
	int nMM = 0;
	int nSS = 0;
	sscanf(pData[2],"%2d%2d%d",&nHH,&nMM,&nSS);	//格林威治时间，取GPS时间，与北京时间有 8 小时时差。时分秒各占两位，不足前面补“0”，如：120104表示“12时1分4秒”。

	//日期 DDMMYY
	int nDD = 0;
	int nM = 0;
	int nYY = 0;
	sscanf(pData[10],"%2d%2d%d",&nDD,&nM,&nYY);	//日月年各占两位，不足前面补“0”，如：220809表示2009年8月22日

	//格林威治时间->调整北京时间
	nHH = nHH + 8;
	if((nHH/24) >=1 )
	{
		nDD++;
	}
	nHH = nHH%24;

	CString strTempTime;
	strTempTime.Format(_T("20%.2d-%.2d-%.2d %.2d:%.2d:%.2d"),nYY,nM,nDD,nHH,nMM,nSS);

	//GPS位置
	float fTemp1 = 0;
	float fTemp2 = 0;
	float fTemp3 = 0;
	float y = 0;
	float x = 0;

	//Latitude纬度
	sscanf(pData[4],"%2f%7f",&fTemp1,&fTemp2);
	fTemp3 = fTemp2 / 60;
	y = fTemp1 + fTemp3;
	//Longitude 经度
	sscanf(pData[6],"%3f%7f",&fTemp1,&fTemp2);
	fTemp3 = fTemp2 /60;
	x = fTemp1 + fTemp3;

	//写数据库
	CString tempStr;
	LocationDataInfo testClass;

	tempStr.Format(_T("%s"),pData[0]); //SIM卡号
	testClass.DEVICE_CODE = tempStr.AllocSysString();

	testClass.DIRECTION = atoi(pData[9]);

	testClass.GPS_DATETIME = strTempTime.AllocSysString(); //时间

	tempStr.Format(_T("%s"),pData[7]);
	testClass.GPSAV = tempStr.AllocSysString(); //‘A’或‘V’

	tempStr.Format(_T("%s"),pData[3]);
	testClass.LAT_NS = tempStr.AllocSysString(); 

	testClass.LATITUDE = y;//纬度

	tempStr.Format(_T("%s"),pData[5]);
	testClass.LONG_WE = tempStr.AllocSysString();

	testClass.LONGITUDE = x; //经度

	double dSpeed = atof(pData[8]);
	testClass.SPEED = (int)dSpeed;

	tempStr = _T(" ");
	if(strlen(pData[11]) == 8)
	{
		int nState1;
		int nState2;
		int nState3;
		int nState4;
		sscanf(pData[11], "%2x%2x%2x%2x",&nState1,&nState2,&nState3,&nState4);

		if(nState2 & 0x80)
		{
			tempStr = _T("锁定;");
		}
		else
		{
			tempStr = _T("未锁定;");
		}

		if(nState2 & 0x40)
		{
			tempStr = tempStr + _T("GPS间断打开;");
		}
		else
		{
			tempStr = tempStr + _T("GPS常开;");
		}

		if(nState3 & 0x80)
		{
			tempStr = tempStr + _T("电池电量低;");
		}
	}
	else
	{
		tempStr = _T(" ");
	}
	testClass.STATE = tempStr.AllocSysString(); //状态

	tempStr = _T(" ");
	testClass.LOCATION = tempStr.AllocSysString(); //位置 NULL ?
	
	testClass.USER_CODE = strDeviceCode.AllocSysString(); //
	testClass.USER_TYPE = nUserType;;
	
	int nReturnIndex = -1;
	m_pWebService->HYTPGIS_LocateDataAdd(testClass,&nReturnIndex);

	::SysFreeString(testClass.DEVICE_CODE);
	::SysFreeString(testClass.GPS_DATETIME);
	::SysFreeString(testClass.GPSAV);
	::SysFreeString(testClass.LAT_NS);
	::SysFreeString(testClass.LONG_WE);
	::SysFreeString(testClass.STATE);
	::SysFreeString(testClass.LOCATION);
	::SysFreeString(testClass.USER_CODE);

	if(nReturnIndex < 0) //写库不成功
	{
		return FALSE;
	}

	//写库成功
	gWriteGpsConnt++;

	if(nGpsDataType != GPS_DATA_HISTORY)
	{
		gPGisMainCtrlProc.m_BusinessManage_ConToPub.SendData_Broadcast_GisClient_TerminalGpsData(pRecvData,nReturnIndex);
	}

	if(gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.m_bConnGpsCheckServer && nGpsDataType != GPS_DATA_HISTORY)
	{
		if (gPGisMainCtrlProc.m_BusinessManage_ConnectToGPSCheckServer.SendUpGPSDataToGPSCheckServer(pData[0],pData[6],pData[4]))
		{
			++g_nRecvLocationGPSCount;
		}	
	}

	if(gPGisMainCtrlProc.m_BusinessManage_ConnectToLocationServer.m_bConnLocationServer && nGpsDataType != GPS_DATA_HISTORY) 
	{
		if (gPGisMainCtrlProc.m_BusinessManage_ConnectToLocationServer.QueryLocationProcess(nReturnIndex,pData[0],pData[6],pData[4]))	//返回信息
		{
			++g_nSendLocationGPSCount;
		}
	}

	for (size_t nCount = 0;nCount< gPGisMainCtrlProc.m_BusinessManage_ConnectToShanhaiServer_vec.size();++nCount)
	{
		CBusinessManage_ConToPub::TERMINAL_INFO temp_TermainalInfo;
		gPGisMainCtrlProc.m_BusinessManage_ConToPub.m_HashmapDataInfo.FindItem(pData[0],temp_TermainalInfo);
		if(gPGisMainCtrlProc.m_BusinessManage_ConnectToShanhaiServer_vec[nCount]->m_bConnShanhaiServer && gPGisMainCtrlProc.m_SubscriptionManager.GetSubscriptionInfo_ShanHai(gPGisMainCtrlProc.m_BusinessManage_ConnectToShanhaiServer_vec[nCount]->m_szSubscriptionID,(DWORD)&temp_TermainalInfo))
		{
			SH_Gps temp_GpsData;
			ZeroMemory(&temp_GpsData,sizeof(SH_Gps));
			memcpy(temp_GpsData.GpsId,pData[0],strlen(pData[0]));
			temp_GpsData.speed = (unsigned short)atoi(pData[8]);
			temp_GpsData.dir = htons((unsigned short)atoi(pData[9]));
			temp_GpsData.height = htons(0);
			COleDateTime tempDTime;
			nYY = nYY + 2000;
			tempDTime.SetDateTime(nYY,nM,nDD,nHH,nMM,nSS);
			temp_GpsData.lat = y; //纬度
			temp_GpsData.lon = x;//经度
			
			if (NULL == strncmp("A",pData[7],strlen(pData[7])))
			{
				temp_GpsData.nMsgtype =	htonl(0x0000);
			}else if (NULL == strncmp("V",pData[7],strlen(pData[7])))
			{
				temp_GpsData.nMsgtype =	htonl(0xFFFF);
			}

			temp_GpsData.nYear = tempDTime.GetYear();
			temp_GpsData.nMonth = tempDTime.GetMonth();
			temp_GpsData.nDay = tempDTime.GetDay();
			temp_GpsData.nHour = tempDTime.GetHour();
			temp_GpsData.nMintue = tempDTime.GetMinute();
			temp_GpsData.nSecond = tempDTime.GetSecond();
			
			char tmp[100];
			memcpy(tmp,(char*)&temp_GpsData,sizeof(SH_Gps));

			gPGisMainCtrlProc.m_BusinessManage_ConnectToShanhaiServer_vec[nCount]->SendGPSData((const char*)&temp_GpsData,sizeof(SH_Gps));
			gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [ShanHai][%s] GpsId:%s speed:%d dir:%d height:%d lat:%f lon:%f %d %d %d %d %d %d\n"), CTime::GetCurrentTime().Format("%H:%M:%S"),
				gPGisMainCtrlProc.m_BusinessManage_ConnectToShanhaiServer_vec[nCount]->m_szSubscriptionID,temp_GpsData.GpsId,temp_GpsData.speed,temp_GpsData.dir,temp_GpsData.height,temp_GpsData.lat,temp_GpsData.lon
				,temp_GpsData.nYear,temp_GpsData.nMonth,temp_GpsData.nDay,temp_GpsData.nHour,temp_GpsData.nMintue,temp_GpsData.nSecond);
		}
	}

	return TRUE;
}
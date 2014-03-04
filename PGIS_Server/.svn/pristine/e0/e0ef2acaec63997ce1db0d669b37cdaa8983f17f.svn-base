// HttpInterface.cpp: implementation of the CHttpInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpInterface.h"
#include "afxinet.h"
#include "XMLParse.h"
#include "XMLMakeUp.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//获取网卡地址
// 把UTF-8转换成Unicode
void UTF_8ToUnicode(WCHAR* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

// Unicode 转换成UTF-8 
void UnicodeToUTF_8(char* pOut,WCHAR* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

// 把Unicode 转换成 GB2312 
void UnicodeToGB2312(char* pOut,unsigned short uData)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
	return;
}     

// GB2312 转换成　Unicode
void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return;
}

CString	BinToTxt(unsigned char* lpBin,int len,BOOL addH)
{
	CString str;
	char szTemp[100];
	for(int i = 0; i < len; i++)
	{
		unsigned char cc = lpBin[i];
		sprintf(szTemp,"%02x",cc);
		if(addH)
		{
			str += "%";
		}
		str += szTemp;
	}
	str.MakeUpper();
	return str;
}

//GB2312 转为 UTF-8
void GB2312ToUTF_8(char* pOut,char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2000];

	memset(buf,0,4);
	memset(rst,0,pLen + (pLen >> 2) + 2000);

	int i = 0;
	int j = 0;
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			WCHAR pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			unsigned short int tmp = 0;
			if(1)
			{
				CString strText = BinToTxt((unsigned char*)buf,3,true);
				strcat(rst+j,(LPCSTR)strText);				
				j += strText.GetLength();
				i += 2;
			}else
			{
				tmp = rst[j] = buf[0];
				tmp = rst[j+1] = buf[1];
				tmp = rst[j+2] = buf[2];


				j += 3;
				i += 2;
			}			
		}
	}
	rst[j] = 0;

	//返回结果
	//pOut = rst;   
	strcpy(pOut,rst);
	delete rst;   

	return;
}

//UTF-8 转为 GB2312
void UTF_8ToGB2312(char* pOut, char *pText, int pLen)
{
	char * newBuf = new char[pLen + 100];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Ctemp,Wtemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = 0;

	//pOut = newBuf;
	strcpy(pOut,newBuf);
	delete newBuf;

	return; 
}
//--------------------------------------------------------------------------------



CHttpInterface::CHttpInterface()
{	
}

CHttpInterface::~CHttpInterface()
{

}

CString CHttpInterface::GetResultXML(CString strValue)
{
	CString strXMLData = strValue;
	strXMLData.Replace("^lt;","<");
	strXMLData.Replace("^gt;",">");
	strXMLData.Replace("^amp;","&");
	strXMLData.Replace("^apos;","\'");
	strXMLData.Replace("^quot;","\"");
	return strXMLData;
}

BOOL CHttpInterface::GetWebTxtExt(CString strURL, char* lpBuffer, int nBufLen)
{
	CString strServer;
	DWORD dwType;
	CString strFilePath;
	INTERNET_PORT dwPort;
	DWORD m_dwHttpRequestFlags;
	CString strWebText = _T("");
	
	//m_dwHttpRequestFlags=HSR_DOWNLOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT; 	
	m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	BOOL b = AfxParseURL(strURL,dwType,strServer,strFilePath,dwPort);
	if(!b || dwType!=AFX_INET_SERVICE_HTTP)
	{
		return FALSE;
	}
	
	//CInternetSession cis;
	CInternetSession cis(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE/*|INTERNET_OPTION_RESET_URLCACHE_SESSION*/);
	CHttpConnection *pHttp;
	b = cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,50000);
	if(!b)
	{
		return FALSE;
	}
	
	//pHttp=cis.GetHttpConnection(strServer,dwPort);
	pHttp=cis.GetHttpConnection(strServer,INTERNET_FLAG_RELOAD,dwPort);
	if(!pHttp)
		return FALSE;
	
	CHttpFile *pFile;
	pFile = NULL;
	try
	{
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_GET, strFilePath, NULL, 1, NULL, NULL, m_dwHttpRequestFlags); 
	}
	catch(CInternetException*  pException)
	{
		pException->Delete();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	if(!pFile)
		return FALSE;
	
	try
	{
		if(!pFile->SendRequest())
		{
			ASSERT(FALSE);
			pFile->Close();
			pHttp->Close();
			cis.Close();
			return FALSE;
		}
	}catch(CInternetException*  pException1)
	{
		pException1->Delete();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	strWebText.Empty();
	char buf[1024*200];
	memset(buf,0,1024*200);	
	int n = 0;
	int nTotalSize = 0;
	char tempbuf[1024*100];

	try
	{
		while(1)
		{
			memset(tempbuf,0,1024*100);
			n = pFile->Read(tempbuf,1024*90);
			if(n <=0) break;
			
			if((nTotalSize + n) > nBufLen)
			{
				break;
			}
			
			strcat(lpBuffer,tempbuf);		
			nTotalSize+= n;
			//strWebText += tempbuf;
		}
	}
	catch(CInternetException*  pException12)
	{
		pException12->Delete();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	pFile->Close();
	pHttp->Close();
	cis.Close();	
	return TRUE;
}

CString CHttpInterface::GetWebTextHttps(CString strURL)
{
	CString strServer;
	DWORD dwType;
	CString strFilePath;
	INTERNET_PORT dwPort;
	DWORD m_dwHttpRequestFlags;
	CString strWebText = _T("");
	
	m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	BOOL b = AfxParseURL(strURL,dwType,strServer,strFilePath,dwPort);
	if(!b || dwType != AFX_INET_SERVICE_HTTPS)
	{
		return strWebText;
	}
	
	//CInternetSession cis;
	CInternetSession cis(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE);
	CHttpConnection *pHttp;
	b = cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,50000);
	if(!b)
	{
		return strWebText;
	}
	
	//pHttp=cis.GetHttpConnection(strServer,dwPort);
	pHttp=cis.GetHttpConnection(strServer,INTERNET_FLAG_RELOAD,dwPort);
	if(!pHttp)
		return strWebText;
	
	CHttpFile *pFile;
	pFile = NULL;
	try
	{
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_GET, strFilePath, NULL, 1, NULL, NULL, m_dwHttpRequestFlags); 
	}
	catch(CInternetException*  pException)
	{
		pException->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	
	if(!pFile)
		return strWebText;
	
	try
	{
		if(!pFile->SendRequest())
		{
			ASSERT(FALSE);
			pFile->Close();
			pHttp->Close();
			cis.Close();
			return strWebText;
		}
	}catch(CInternetException*  pException1)
	{
		pException1->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	
#if 0
	CString strErrorrrrr;
	pFile->QueryInfo(HTTP_QUERY_STATUS_CODE,strErrorrrrr);
	if(strErrorrrrr=="404")
	{
		pFile->Close();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
#endif
	
	strWebText.Empty();
	char buf[1024*200];
	memset(buf,0,1024*200);	
	int n = 0;
	int nTotalSize = 0;
	//char tempbuf[1024*100];
	try
	{
		while(1)
		{
			memset(buf,0,1024*200);
			n = pFile->Read(buf,1024*199);
			if(n <=0) break;
			
			nTotalSize+= n;
			strWebText += buf;
			//if(nTotalSize >= (1024*190))
			//	break;
			//strcat(buf,tempbuf);
		}
	}catch(CInternetException*  pException12)
	{
		pException12->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	//strWebText = buf;
	
	pFile->Close();
	pHttp->Close();
	cis.Close();	
	return strWebText;
}

CString CHttpInterface::GetWebText(CString strURL)
{
	CString strServer;
	DWORD dwType;
	CString strFilePath;
	INTERNET_PORT dwPort;
	DWORD m_dwHttpRequestFlags;
	CString strWebText = _T("");
	
	//m_dwHttpRequestFlags=HSR_DOWNLOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT; 	
	m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	BOOL b = AfxParseURL(strURL,dwType,strServer,strFilePath,dwPort);
	if(!b || dwType!=AFX_INET_SERVICE_HTTP)
	{
		return strWebText;
	}
	
	//CInternetSession cis;
	CInternetSession cis(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE/*|INTERNET_OPTION_RESET_URLCACHE_SESSION*/);
	CHttpConnection *pHttp;
	b = cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,50000);
	if(!b)
	{
		return strWebText;
	}
	
	//pHttp=cis.GetHttpConnection(strServer,dwPort);
	pHttp=cis.GetHttpConnection(strServer,INTERNET_FLAG_RELOAD,dwPort);
	if(!pHttp)
		return strWebText;
	
	CHttpFile *pFile;
	pFile = NULL;
	try
	{
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_GET, strFilePath, NULL, 1, NULL, NULL, m_dwHttpRequestFlags); 
	}
	catch(CInternetException*  pException)
	{
		pException->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	
	if(!pFile)
		return strWebText;
	
	try
	{
		if(!pFile->SendRequest())
		{
			ASSERT(FALSE);
			pFile->Close();
			pHttp->Close();
			cis.Close();
			return strWebText;
		}
	}catch(CInternetException*  pException1)
	{
		pException1->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	
#if 0
	CString strErrorrrrr;
	pFile->QueryInfo(HTTP_QUERY_STATUS_CODE,strErrorrrrr);
	if(strErrorrrrr=="404")
	{
		pFile->Close();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
#endif
	
	strWebText.Empty();
	char buf[1024*200];
	memset(buf,0,1024*200);	
	int n = 0;
	int nTotalSize = 0;
	//char tempbuf[1024*100];
	try
	{
		while(1)
		{
			memset(buf,0,1024*200);
			n = pFile->Read(buf,1024*199);
			if(n <=0) break;
			
			nTotalSize+= n;
			strWebText += buf;
			//if(nTotalSize >= (1024*190))
			//	break;
			//strcat(buf,tempbuf);
		}
	}catch(CInternetException*  pException12)
	{
		pException12->Delete();
		pHttp->Close();
		cis.Close();
		return strWebText;
	}
	//strWebText = buf;
	
	pFile->Close();
	pHttp->Close();
	cis.Close();	
	return strWebText;
}


BOOL CHttpInterface::TestWebConnect(CString strURL)
{
	CString strServer;
	DWORD dwType;
	CString strFilePath;
	INTERNET_PORT dwPort;
	DWORD m_dwHttpRequestFlags;
	
	//m_dwHttpRequestFlags=HSR_DOWNLOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT; 	
	m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	BOOL b = AfxParseURL(strURL,dwType,strServer,strFilePath,dwPort);
	if(!b || dwType!=AFX_INET_SERVICE_HTTP)
	{
		return FALSE;
	}
	
	//CInternetSession cis;
	CInternetSession cis(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE);
	CHttpConnection *pHttp;
	b = cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,50000);
	if(!b)
	{
		return FALSE;
	}
	pHttp=cis.GetHttpConnection(strServer,dwPort);
	if(!pHttp)
		return FALSE;
	
	CHttpFile *pFile;
	pFile = NULL;
	try
	{
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_GET, strFilePath, NULL, 1, NULL, NULL, m_dwHttpRequestFlags); 
	}
	catch(CInternetException*  pException)
	{
		pException->Delete();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	if(!pFile)
		return FALSE;
	
	try
	{
		if(!pFile->SendRequest())
		{
			ASSERT(FALSE);
			pFile->Close();
			pHttp->Close();
			cis.Close();
			return FALSE;
		}
	}catch(CInternetException*  pException1)
	{
		pException1->Delete();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	CString strErrorrrrr;
	pFile->QueryInfo(HTTP_QUERY_STATUS_CODE,strErrorrrrr);
	if(strErrorrrrr=="404")
	{
		pFile->Close();
		pHttp->Close();
		cis.Close();
		return FALSE;
	}
	
	pFile->Close();
	pHttp->Close();
	cis.Close();	
	return TRUE;
}


CString __MakeRequestHeaders(CString &strBoundary,CString m_strSeverName, INTERNET_PORT m_nPort)
{
    CString strFormat=_T("");
    CString strData =_T("");
    strFormat += _T("Content-Type: multipart/form-data; boundary=%s\r\n"); 
    strFormat +=_T("Host: %s:%d\r\n");
    strData.Format(strFormat, strBoundary,m_strSeverName, m_nPort); 	
    return strData;
}


CString  __MakePreFileData(CString &strBoundary, CString &strFileName,CString strID)
{
	//Content-Type:
	//JPG image/pjpeg
	//PNG image/x-png
	//BMP image/bmp
	//TIF image/tiff
	//GIF image/gif
	CString strFormat=_T("");
	CString strData=_T("");
    strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"file1\"; filename=\"%s\"");
	//strFormat += _T("Content-Disposition: form-data; name=\"file1\"; filename=\"%s\"; name=\"OUTSIDEID\";OUTSIDEIDname=\"%s\"");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: multipart/form-data");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Transfer-Encoding: binary");
	strFormat += _T("\r\n\r\n");
	
	//strData.Format(strFormat, strBoundary, strFileName,strID);
	strData.Format(strFormat, strBoundary, strFileName);
	
	return strData; 
}

CString __MakePostFileData(CString &strBoundary)
{
	CString strFormat;
	CString strData;
	
	strFormat = _T("\r\n");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("submit");
	strFormat += _T("\r\n");
	strFormat += _T("--%s--");
	strFormat += _T("\r\n");
	
	strData.Format(strFormat, strBoundary, strBoundary);
	
	return strData;
}

#if 1
BOOL CHttpInterface::PostData(CString strURL, char* lpData, int nLen, CString &strResult)
{
	m_bBadNet = FALSE;
	m_bInCheck = FALSE;
	char szTempHost[512];
	strncpy(szTempHost,strURL.GetBuffer(nLen),sizeof(szTempHost));

	DWORD m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	DWORD dwType = 0;
	CString strServer;
	CString strObject; 	
	INTERNET_PORT wPort = 80;	
	AfxParseURL(szTempHost, dwType, strServer, strObject, wPort); 
		
	//CInternetSession Session;
	CInternetSession Session(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE);
	Session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,90000);
	CHttpConnection *pHttpConnection = NULL;
	CHttpFile* pHTTP;
	CString strRequestHeader=_T("");
	BOOL bSuccess = FALSE;

	strResult.Empty();
	strRequestHeader = _T("Content-Type: application/x-www-form-urlencoded\r\n");	
	//strRequestHeader = _T("Content-Type: text/xml\r\n");	
	try
	{
		pHttpConnection = Session.GetHttpConnection(strServer,wPort);
		//pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject,NULL,1,NULL,NULL,m_dwHttpRequestFlags);
		pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
		BOOL result;

#if 0
		BOOL result = pHTTP->SendRequest(strRequestHeader, (LPVOID)lpData, nLen);
		if(!result)
		{
			goto POSTDATAERROR;
		}
#else
		DWORD dwTotalRequestLength = nLen;
		pHTTP->AddRequestHeaders(strRequestHeader);
		pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);
		pHTTP->Write((LPSTR)(LPCSTR)lpData, nLen);
		pHTTP->EndRequest(HSR_SYNC);
#endif
	
		DWORD dwRet;
		pHTTP->QueryInfoStatusCode(dwRet);	
		char szBuff[1024];
		UINT nRead;

		char *szNewBuf = new char[1024*1024];
		memset(szNewBuf, 0, 1024 * 100);
		while ((nRead = pHTTP->Read(szBuff,1023))>0)
		{
			strResult += CString(szBuff,nRead);
//			UTF_8ToGB2312(szNewBuf, szBuff, nRead);//转码UTF_8到GBK			
//			strResult += szNewBuf;
		}	

//		UTF_8ToGB2312(szNewBuf, CSTRINGTOTCHAR(strResult), strResult.GetLength());//转码UTF_8到GBK			
		strResult = szNewBuf;
		delete []szNewBuf;

		if (dwRet == HTTP_STATUS_OK)
		{
			bSuccess = TRUE;
		}
		else
		{
			if (100 == dwRet)
			{
				m_bInCheck = TRUE;
			}
		}
	}
	catch (CException* e)
	{
		//e->GetErrorMessage(szError, MAX_PATH);
		e->Delete();		
		bSuccess = FALSE;
		m_bBadNet = TRUE;
	}

//	delete lpUtf8;
	pHTTP->Close();
	delete pHTTP;
	return bSuccess;
}

#else

BOOL CHttpInterface::PostData(CString strURL, char* lpData, int nLen, CString &strResult)
{
	char szTempHost[512];
	MYCHAR_sprintf(szTempHost,"%s",CSTRINGTOTCHAR(strURL));
	//char* lpUtf8 = new char[nLen + 100];
	//memset(lpUtf8,0,nLen + 100);	
	//GB2312ToUTF_8(lpUtf8,lpData, nLen);
	
	DWORD m_dwHttpRequestFlags=INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE /*| INTERNET_OPTION_RESET_URLCACHE_SESSION*/;
	DWORD dwType = 0;
	CString strServer;
	CString strObject; 	
	INTERNET_PORT wPort = 80;	
	AfxParseURL(szTempHost, dwType, strServer, strObject, wPort); 
	
	//CInternetSession Session;
	CInternetSession Session(_T("session"),0,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_DONT_CACHE);
	Session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,90000);
	CHttpConnection *pHttpConnection = NULL;
	CHttpFile* pHTTP;
	CString strRequestHeader=_T("");
	BOOL bSuccess = FALSE;
	
	strResult.Empty();
	//strRequestHeader = _T("Content-Type: application/x-www-form-urlencoded\r\n");	
	strRequestHeader = _T("Content-Type: text/xml\r\n");		
	try
	{
		pHttpConnection = Session.GetHttpConnection(strServer,wPort);
		//pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject,NULL,1,NULL,NULL,m_dwHttpRequestFlags);
		pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
		BOOL result = pHTTP->SendRequest(strRequestHeader, (LPVOID)lpData, nLen);
		if(!result)
		{
			goto POSTDATAERROR;
		}
		
		DWORD dwRet;
		pHTTP->QueryInfoStatusCode(dwRet);	
		char szBuff[1024];
		UINT nRead;
		while ((nRead = pHTTP->Read(szBuff,1023))>0)
		{
			strResult += CString(szBuff,nRead);
		}
		
		if (dwRet == HTTP_STATUS_OK)
		{
			bSuccess = TRUE;
		}
	}
	catch (CException* e)
	{
		//e->GetErrorMessage(szError, MAX_PATH);
		e->Delete();		
		bSuccess = FALSE;
	}
	
	//	delete lpUtf8;
POSTDATAERROR:
	pHTTP->Close();
	delete pHTTP;
	return bSuccess;
}

#endif

char* g_lpszJXInterface[] = 
{
	"User_doLogin",//"UserAction", User_doLogin
	"UserOld_doLogins",
	"PlanManageAction",
	"TempManage_init",//TempManageAction
	"IndexExamineManageAction",
	"ScoreReleaseAction",
	"WorkLogAction",
	"ScoreShowAction",
	"OtherBusinessAction",
	"User_loginSFZH",
	"User_loginOut"
};

PTHttpInterfaceData CHttpInterface::CHI_New(ENJXSystemType enType)
{
	PTHttpInterfaceData pIData = new THttpInterfaceData;
	memset(pIData,0,sizeof(THttpInterfaceData));
	pIData->enType = enType;
	pIData->nResultCode = -1;
	return pIData;
}

BOOL CHttpInterface::CHI_AddFunName(PTHttpInterfaceData pIData, char* lpFunName)
{
	strcpy(pIData->szFunName,lpFunName);
	return TRUE;
}

BOOL CHttpInterface::CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, int nParam)
{
	char szTemp[MAX_PATH];
	sprintf(szTemp,"%d",nParam);
	return CHI_AddOneInItem(pIData, lpInName,szTemp );
}

BOOL CHttpInterface::CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, float fParam)
{
	char szTemp[MAX_PATH];
	sprintf(szTemp,"%f",fParam);
	return CHI_AddOneInItem(pIData, lpInName,szTemp );
}

BOOL CHttpInterface::CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, char* lpParam)
{
	if(pIData->nInItemCount >= MAX_PARAM_ITEM_COUNT)
		return FALSE;

	CString strInName = lpInName;
	strInName.MakeUpper();
	int nIdx = pIData->nInItemCount;
	strncpy(pIData->tInParam[nIdx].szParamName,strInName.GetBuffer(),sizeof(pIData->tInParam[nIdx].szParamName));	
	pIData->tInParam[nIdx].lpParam=lpParam;		
	pIData->nInItemCount++;
	return TRUE;
}

BOOL CHttpInterface::CHI_AddOneOutItem(PTHttpInterfaceData pIData,char* lpOutName)
{
	if(pIData->nOutItemCount >= MAX_PARAM_ITEM_COUNT)
		return FALSE;
	
	int nIdx = pIData->nOutItemCount;
	CString strName = lpOutName;
	strName.MakeUpper();
	strncpy(pIData->tOutParam[nIdx].szParamName,strName.GetBuffer(),sizeof(pIData->tOutParam[nIdx].szParamName));
	_strdup(pIData->tOutParam[nIdx].szParamName);
	pIData->nOutItemCount++;
	return TRUE;
}

BOOL CHttpInterface::CHI_ProcIterfaceEx(PTHttpInterfaceData pIData,CString& strReturnXml,CString strInputXML)
{
	strReturnXml.Empty();
//先组织XML文件
	CString strXML;
	strXML.Empty();
	//strXML = "parameter=";
	strXML += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	strXML += "<request>";
	strXML += "<function>";
	strXML += pIData->szFunName;
	strXML += "</function>";

	strXML += "<inputparameters>";
	for(int i = 0; i < pIData->nInItemCount; i++)
	{
		strXML += "<inputItem>";
		strXML += "<name>";
		strXML += pIData->tInParam[i].szParamName;
		strXML += "</name>";

		strXML += "<value>";
		strXML += pIData->tInParam[i].lpParam;
		strXML += "</value>";

		strXML += "</inputItem>";
	}

	strXML += strInputXML;
	strXML += "</inputparameters>";

// 	strXML += "<outItems>";
// 	for(int i = 0; i < pIData->nOutItemCount; i++)
// 	{
// 		strXML += "<name>";		
// 		strXML += pIData->tOutParam[i].szParamName;
// 		strXML += "</name>";
// 	}
// 	strXML += "</outItems>";
	strXML += "</request>";
	//TRACE("%s\n",strXML);

#ifdef _DEBUG
	{
		FILE* fp = fopen("D:\\input.xml","wt");
		if(fp)
		{
			fwrite(strXML.GetBuffer(),1,strXML.GetLength(),fp);
			fclose(fp);
		}
	}	
#endif

	//strURL += "hitomJXKP/";
	CString strURL = "";
	strURL += g_lpszJXInterface[pIData->enType];
	strURL += ".action";
	CString strResultXML;
	
	//CHttpInterface::ShowWait();
	BOOL bRet = PostData(strURL,strXML.GetBuffer(),strXML.GetLength(),strResultXML);
//	CHttpInterface::HideWait();	
	if(!bRet)
		return FALSE;	

	strReturnXml = strResultXML;
	char* lpXMLData = new char[strResultXML.GetLength() + 1];
	strcpy(lpXMLData,strResultXML.GetBuffer());
#ifdef _DEBUG
	{
		FILE* fp = fopen("D:\\output.xml","wt");
		if(fp)
		{
			fwrite(strResultXML.GetBuffer(),1,strResultXML.GetLength(),fp);
			fclose(fp);
		}
	}	
#endif

	//解析返回结果
	CString strXMLData;
	CMarkup  xml;
	xml.SetDoc(lpXMLData);
	BOOL bR = xml.FindElem("request");	
	ASSERT(bR);
		if(!bR)
	{
		delete lpXMLData;
		return FALSE;
	}
	
	bR = xml.IntoElem();
	ASSERT(bR);	
	if(!bR)
	{
		delete lpXMLData;
		return FALSE;
	}

	if(xml.FindElem("errorcode"))
	{
		
		strXMLData = xml.GetData();
		strncpy(pIData->szErrorCode,strXMLData.GetBuffer(),MAX_PATH-1);
	}else
	{
		ASSERT(FALSE);
		delete lpXMLData;
		return FALSE;	
	}

	if(xml.FindElem("resultcode"))
	{
		
		strXMLData = xml.GetData();
		pIData->nResultCode = atoi(strXMLData.GetBuffer());		
	}else
	{
		ASSERT(FALSE);
		delete lpXMLData;
		return FALSE;	
	}

	if(pIData->nResultCode != 0)
	{
		delete lpXMLData;
		return FALSE;
	}
	
	bR = xml.FindElem("outputparameters");
	//	ASSERT(bR);
	if(!bR)
	{
		delete lpXMLData;
		return (pIData->nResultCode == 0);
	}
	
	bR = xml.IntoElem();
	ASSERT(bR);
	if(!bR)
	{
		delete lpXMLData;
		return (pIData->nResultCode == 0);
	}
	
	BOOL BFirstFind = FALSE;
	while(xml.FindElem("outItem"))
	{
		BFirstFind = TRUE;
		TParamItem tItemData;
		
		memset(&tItemData,0,sizeof(TParamItem));
		bR = xml.IntoElem();
		if(!bR)
		{
			delete lpXMLData;
			return FALSE;
		}
		
		if(xml.FindElem( "name" ))
		{
			strXMLData = xml.GetData();
			strncpy(tItemData.szParamName,strXMLData.GetBuffer(),MAX_PATH-1);
		}else
		{
			ASSERT(FALSE);
			delete lpXMLData;
			return FALSE;
		}
		
		if(xml.FindElem( "value" ))
		{
			strXMLData = xml.GetData();
			
			strXMLData = GetResultXML(strXMLData);
			tItemData.lpParam = new char[strXMLData.GetLength() + 1];
			memset(tItemData.lpParam,0,strXMLData.GetLength() + 1);
			strcpy(tItemData.lpParam,strXMLData.GetBuffer());
		}else
		{
			ASSERT(FALSE);
			delete lpXMLData;
			return FALSE;
		}
		
		BOOL bFind = FALSE;
		for(int k = 0; k < pIData->nOutItemCount; k++)
		{
			if(0 == strcmp(pIData->tOutParam[k].szParamName,tItemData.szParamName))
			{
				bFind = TRUE;
				pIData->tOutParam[k].lpParam = tItemData.lpParam;
			}
		}
		
		if(!bFind)
		{
			ASSERT(FALSE);
			delete tItemData.lpParam;
		}
		
		bR = xml.OutOfElem();
		if(!bR)
		{
			delete lpXMLData;
			return FALSE;
		}
	}

	delete lpXMLData;
	return (pIData->nResultCode == 0);
}

BOOL CHttpInterface::CHI_ProcIterface(PTHttpInterfaceData pIData)
{
	CString strXML;
	strXML.Empty();
	strXML += "TXTCONTENT=<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	strXML += "<request>";
	strXML += "<function>";
	strXML += pIData->szFunName;
	strXML += "</function>";

	strXML += "<inputparameters>";
	for(int i = 0; i < pIData->nInItemCount; i++)
	{
		strXML += "<inputItem>";
		strXML += "<name>";
		strXML += pIData->tInParam[i].szParamName;
		strXML += "</name>";
		strXML += "<value>";
		strXML += pIData->tInParam[i].lpParam;
		strXML += "</value>";
		strXML += "</inputItem>";
	}
	strXML += "</inputparameters>";
	strXML += "</request>";


	char* lpXMLData1 = new char[strXML.GetLength() + 100];
	memset(lpXMLData1,0,strXML.GetLength() + 100);
	GB2312ToUTF_8(lpXMLData1,strXML.GetBuffer(),strXML.GetLength());
	strXML.Format("%s",lpXMLData1);
	delete lpXMLData1;

	CString strURLInfo = "http://192.168.1.119:8080/baseServer/ydjwService";
	CString strResultXML;
	BOOL bRet = PostData(strURLInfo,strXML.GetBuffer(),strXML.GetLength(),strResultXML);
	//BOOL bRet = PostData(strURLInfo,lpXMLData1,strlen(lpXMLData1),strResultXML);

	if(!bRet)
	{
		return FALSE;
	}

// 	char* lpXMLData = new char[strResultXML.GetLength() + 1];
// 	UTF_8ToGB2312(lpXMLData, (char*)(LPCSTR)strResultXML,strResultXML.GetLength());
// 	strcpy(lpXMLData,strResultXML.GetBuffer());
	//解析返回结果
// 	CString strXMLData;
// 	CMarkup  xml;
// 	xml.SetDoc(lpXMLData);
// 
// 	BOOL bR = xml.FindElem("request");	
// 	ASSERT(bR);
// 	if(!bR)
// 	{
// 		delete lpXMLData;
// 		return FALSE;
// 	}
// 	
// 	bR = xml.IntoElem();
// 	ASSERT(bR);	
// 	if(!bR)
// 	{
// 		delete lpXMLData;
// 		return FALSE;
// 	}
// 
// 	BOOL bRetErr = xml.FindElem("errorcode");
// 	if(bRetErr)
// 	{
// 		
// 		strXMLData = xml.GetData();
// 		strncpy(pIData->szErrorCode,strXMLData.GetBuffer(),MAX_PATH-1);
// 	}else
// 	{
// 		ASSERT(FALSE);
// 		delete lpXMLData;
// 		return FALSE;	
// 	}
// 
// 	if(xml.FindElem("resultcode"))
// 	{
// 		
// 		strXMLData = xml.GetData();
// 		pIData->nResultCode = atoi(strXMLData.GetBuffer());		
// 		if (strcmp(strXMLData, "can not found!") == 0)
// 		{
// 			delete lpXMLData;
// 			return FALSE;	
// 		}
// 		//ASSERT(pIData->nResultCode == 0);
// 	}else
// 	{
// 		ASSERT(FALSE);
// 		delete lpXMLData;
// 		return FALSE;	
// 	}
// 
// 	if(pIData->nResultCode != 0)
// 	{
// 		delete lpXMLData;
// 		return FALSE;
// 	}	
// 
// 	bR = xml.FindElem("outputparameters");
// 	//ASSERT(bR);
// 	if(!bR)
// 	{
// 		delete lpXMLData;
// 		return (pIData->nResultCode == 0);
// 	}
// 
// 	bR = xml.IntoElem();
// 	ASSERT(bR);
// 	if(!bR)
// 	{
// 		delete lpXMLData;
// 		return (pIData->nResultCode == 0);
// 	}
// 	
// 	BOOL BFirstFind = FALSE;
// 	while(xml.FindElem("outItem"))
// 	{
// 		BFirstFind = TRUE;
// 		TParamItem tItemData;
// 
// 		memset(&tItemData,0,sizeof(TParamItem));
// 		bR = xml.IntoElem();
// 		if(!bR)
// 		{
// 			delete lpXMLData;
// 			return FALSE;
// 		}
// 
// 		if(xml.FindElem( "name" ))
// 		{
// 			strXMLData = xml.GetData();
// 			strncpy(tItemData.szParamName,strXMLData.GetBuffer(),MAX_PATH-1);
// 		}else
// 		{
// 			ASSERT(FALSE);
// 			delete lpXMLData;
// 			return FALSE;
// 		}
// 
// 		if(xml.FindElem( "value" ))
// 		{
// 			strXMLData = xml.GetData();
// 
// 			strXMLData = GetResultXML(strXMLData);
// 			tItemData.lpParam = new char[strXMLData.GetLength() + 1];
// 			memset(tItemData.lpParam,0,strXMLData.GetLength() + 1);
// 			strcpy(tItemData.lpParam,strXMLData.GetBuffer());
// 		}else
// 		{
// 			ASSERT(FALSE);
// 			delete lpXMLData;
// 			return FALSE;
// 		}
// 
// 		BOOL bFind = FALSE;
// 		for(int k = 0; k < pIData->nOutItemCount; k++)
// 		{
// 			if(0 == strcmp(pIData->tOutParam[k].szParamName,tItemData.szParamName))
// 			{
// 				bFind = TRUE;
// 				pIData->tOutParam[k].lpParam = tItemData.lpParam;
// 			}
// 		}
// 
// 		if(!bFind)
// 		{
// 			//ASSERT(FALSE);
// 			delete tItemData.lpParam;
// 		}
// 
// 		bR = xml.OutOfElem();
// 		if(!bR)
// 		{
// 			delete lpXMLData;
// 			return (pIData->nResultCode == 0);
// 		}
// 	}
// 
// 	ASSERT(BFirstFind);
//	delete lpXMLData;
	return (pIData->nResultCode == 0);
}

#define NULL_STRING	""
char* CHttpInterface::CHI_GetOneOutItem(PTHttpInterfaceData pIData,char* lpOutName)
{	
	CString str;
	for(int k = 0; k < pIData->nOutItemCount; k++)
	{
		if(0 == stricmp(pIData->tOutParam[k].szParamName,lpOutName))
		{
			if(pIData->tOutParam[k].lpParam)
				return pIData->tOutParam[k].lpParam;
			else
				return NULL_STRING;
		}
	}

	ASSERT(FALSE);
	return NULL_STRING;
}

int	 CHttpInterface::CHI_GetOneOutItemInt(PTHttpInterfaceData pIData,char* lpOutName)
{
	int nResult = atoi(CHI_GetOneOutItem(pIData,lpOutName));
	return nResult;
}

float CHttpInterface::CHI_GetOneOutItemFloat(PTHttpInterfaceData pIData,char* lpOutName)
{
	float fResult = atof(CHI_GetOneOutItem(pIData,lpOutName));
	return fResult;
}

BOOL CHttpInterface::CHI_DeleteInterface(PTHttpInterfaceData pIData)
{
	delete pIData;
	pIData = 0;
	return TRUE;
}
BOOL CHttpInterface::m_bBadNet = FALSE;

BOOL CHttpInterface::m_bInCheck= FALSE;
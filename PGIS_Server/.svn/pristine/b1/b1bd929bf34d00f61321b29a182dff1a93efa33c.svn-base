//
#include "StdAfx.h"
#include "FTPConveyFile.h"
#include "rwini.h"
#include <wininet.h>

///////////////////////////////////////////////////////////////////////////////////////////
//
#define WM_WRITE_FTPFILE_MSG            WM_USER+SIMUL_USER_MESSAGE_BASE + 70 

#define WM_START_FTP_MESSAGE            WM_USER+SIMUL_USER_MESSAGE_BASE + 71

//FTP文件上传下载完成通知
#define WM_REPORT_FTP_COMPLETE_MSG      WM_USER+SIMUL_USER_MESSAGE_BASE + 72

//FTP工作状态定义
#define FTP_FREE    0
#define FTP_BUSY    1

extern CWriteLogThread gWriteLogApp;

const char *g_pcTempSaveFileName = "TempMainCtrl.txt"; //需要交换的数据写这个文件
const char *g_pcFtpUpFileName = "FtpMainCtrl.txt"; //需要上传到FTP上的文件名
const char *g_pcFtpDownFileName= "FtpPubNet.txt"; //FTP下载下来文件
///////////////////////////////////////////////////////////////////////////////////////////
//
CFTPConveyFile::CFTPConveyFile()
{
    m_bFTPStart = FALSE;
    m_dwAppThreadID = 0;

    m_dwWriteThreadID = 0;
    m_hWriteThread = NULL;
    m_hWriteThreadEndEvt = NULL;

    m_dwFtpThreadID = 0;
    m_hFTPThread = NULL;
    m_hFTPThreadEndEvt = NULL;

    m_nFTPWorkMode = 0;

    memset(m_strFTPURL,0,512);
    memset(m_strFTPLoginUser,0,256);
    memset(m_strFTPLoginPSWD,0,256);
    memset(m_strDownLoadSavePath,0,MAX_PATH);

    m_pInetSession = NULL;
}

CFTPConveyFile::~CFTPConveyFile()
{
    //
}

//创建保存FTP交互数据的目录
void CFTPConveyFile::CreateFtpConveyDir()
{
    TCHAR AppCurrPath[_MAX_PATH] ={0}; 
    
    GetModuleFileName(NULL,AppCurrPath,_MAX_PATH); 
	
    (_tcsrchr(AppCurrPath,'\\'))[1] = 0; 
	
    sprintf(m_strFtpFileDir,"%sFTPConvey",AppCurrPath);
	
    CFileFind IsValidFile;
    if(!IsValidFile.FindFile(m_strFtpFileDir))  //假如目录不存在
    {
        CreateDirectory(m_strFtpFileDir,NULL);
    }
}

//读配置
BOOL CFTPConveyFile::LoadFtpConfig()
{

	CString tempstr =_T("pGisMainCtrlCfg.ini");

    Crwini rwIniTemp;
    if(!rwIniTemp.CheckIniFile(tempstr))
    {        
        return FALSE;
    }

    m_nFTPWorkMode = rwIniTemp.GetInt(_T("FTPSet"),_T("FTPWorkMode"),tempstr);
    if(m_nFTPWorkMode > 0)
    {
        if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPServerURL"),tempstr,m_strFTPURL) == 0) 
        {
            return FALSE;
        }

        if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPServerUserName"),tempstr,m_strFTPLoginUser) == 0) 
        {
            return FALSE;
        }

        if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPServerPSWD"),tempstr,m_strFTPLoginPSWD) == 0) 
        {
            return FALSE;
        }

        //下载文件保存路径
        if(rwIniTemp.ReadString(_T("FTPSet"),_T("FTPDownLoadPatch"),tempstr,m_strDownLoadSavePath) == 0) 
        {
            sprintf(m_strDownLoadSavePath,"%s",m_strFtpFileDir);
        }
        else
        {
            if(strlen(m_strDownLoadSavePath) == 0)
            {
                sprintf(m_strDownLoadSavePath,"%s",m_strFtpFileDir);
            }
        }

		m_nFTPPort = rwIniTemp.GetInt(_T("FTPSet"),_T("FTPPort"),tempstr);
		if(m_nFTPPort <= 0)
		{
			m_nFTPPort = 21;
		}
    }

	m_nFTPTimerOut = rwIniTemp.GetInt(_T("FTPSet"),_T("FTPTimerOut"),tempstr);
	if(m_nFTPTimerOut <= 0)
	{
		m_nFTPTimerOut = 10;
	}

    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
BOOL CFTPConveyFile::StartFTPConvey(UINT nAppThreadID)
{
    if(m_bFTPStart)
    {
        return TRUE;
    }

    if(!LoadFtpConfig())
    {
        m_nFTPWorkMode = 0;
    }
    
	CreateFtpConveyDir();

    //文件处理线程
    if(m_hWriteThreadEndEvt == 0)
    {
        m_hWriteThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);
    }
    m_hWriteThread = (HANDLE)_beginthreadex( NULL,0,&WriteDataThreadProc,this, 0,&m_dwWriteThreadID );
    if(!m_hWriteThread)
    {
        TRACE(_T("<FTP>Strat WriteData thread fail.\n"));
        return FALSE;
    }

    if(m_nFTPWorkMode > 0)
    {
        //FTP处理线程
        if(m_hFTPThreadEndEvt == 0)
        {
            m_hFTPThreadEndEvt = CreateEvent(NULL,TRUE,TRUE,NULL);
        }
        m_hFTPThread = (HANDLE)_beginthreadex( NULL,0,&FTPDownUpThreadProc,this, 0,&m_dwFtpThreadID);
        if(!m_hFTPThread)
        {
            TRACE(_T("<FTP>Strat FTP thread fail.\n"));
            StopFTPConvey();
            return FALSE;
        }

        m_nftpWorkState = FTP_FREE;
    }

    m_dwAppThreadID = nAppThreadID;

    m_bFTPStart = TRUE;

    return TRUE;
}

BOOL CFTPConveyFile::StopFTPConvey()
{
    if(m_hWriteThread)
    {
        //终止线程
        PostThreadMessage(m_dwWriteThreadID, WM_THREAD_STOP, 0L, 0L);
        ::WaitForSingleObject(m_hWriteThreadEndEvt, INFINITE);

        CloseHandle(m_hWriteThreadEndEvt);
        m_hWriteThreadEndEvt = NULL;
        m_dwWriteThreadID = 0;

        m_hWriteThread = NULL;
    }

    if(m_hFTPThread)
    {
        //终止线程
        PostThreadMessage(m_dwFtpThreadID, WM_THREAD_STOP, 0L, 0L);
        ::WaitForSingleObject(m_hFTPThreadEndEvt, INFINITE);
        
        CloseHandle(m_hFTPThreadEndEvt);
        m_hFTPThreadEndEvt = NULL;
        m_dwFtpThreadID = 0;
        
        m_hFTPThread = NULL;
    }
    
    m_dwAppThreadID = 0;

    m_bFTPStart = FALSE;

    return TRUE;
}
 
// void CFTPConveyFile::WriteData(int nType,const char *format , ... ) 
// {
//     if(m_bFTPStart)
//     {
//         FTPConveryData tmpFtpData;
//         memset(&tmpFtpData,0,sizeof(FTPConveryData));
//         tmpFtpData.nID = nType;
//         if(format != NULL)
//         {
//             va_list argp;
//             va_start(argp, format);	
//             
//             int res = _vsnprintf(tmpFtpData.csData, 511, format, argp);
//             if( res > 0 )
//             {
//                 m_csbufferList.Lock();
//                 m_bufferList.AddTail(tmpFtpData);
//                 m_csbufferList.Unlock();
//                 
//                 if(m_dwWriteThreadID)
//                 {
//                     PostThreadMessage(m_dwWriteThreadID,WM_WRITE_FTPFILE_MSG,0,0); 
//                 }
//             }
//             va_end(argp);
//         }
//     }
// } 

void CFTPConveyFile::WriteData(int nType,const char *pcaData ) 
{
    if(m_bFTPStart && pcaData != NULL)
    {
        FTPConveryData tmpFtpData = {0};
        tmpFtpData.nID = nType;
        _snprintf(tmpFtpData.csData, 511, "%s", pcaData); //glf_20111104 增加\n
        
        m_csbufferList.Lock();
        m_bufferList.AddTail(tmpFtpData);
        m_csbufferList.Unlock();
        
        if(m_dwWriteThreadID)
        {
            PostThreadMessage(m_dwWriteThreadID,WM_WRITE_FTPFILE_MSG,0,0); 
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//写文件线程函数
UINT WINAPI CFTPConveyFile::WriteDataThreadProc(void *lpParameter)
{
    CFTPConveyFile *pMyFtpThread;
    pMyFtpThread = (CFTPConveyFile *)lpParameter;
    
    int timerid = SetTimer(NULL,NULL, 1000, 0L); //产生1s的基准定时
    TRACE(_T("Into WrateData thread%X\n"), pMyFtpThread->m_hWriteThread);
    ResetEvent(pMyFtpThread->m_hWriteThreadEndEvt);
    MSG msg;
    BOOL bcontinue = TRUE;
    while(bcontinue)
    {
        if(GetMessage(&msg,NULL,0,0))
        {
            switch(msg.message)
            {
            case WM_THREAD_STOP:
                bcontinue = FALSE;
                break;
            case WM_WRITE_FTPFILE_MSG: 
                //将数据写入文件
                pMyFtpThread->OnWriteFtpFile();
                break;
            case WM_REPORT_FTP_COMPLETE_MSG:  //FTP文件上传下载完成通知
                pMyFtpThread->OnFTPUpDownCompleteMsg((BOOL)msg.lParam); //
				break;
            case WM_TIMER:
                pMyFtpThread->OnTimerWriteCheck();
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }
    if(timerid != 0)
    {
        KillTimer(NULL, timerid);
    }	
    SetEvent(pMyFtpThread->m_hWriteThreadEndEvt);
    
    TRACE(_T("Quit WriteData thread\n"));
    _endthreadex(0);
    return 0;
}

BOOL CFTPConveyFile::OnWriteFtpFile()
{
    m_csbufferList.Lock();
    
    if(m_bufferList.IsEmpty())
    {
        m_bufferList.RemoveAll();
        m_csbufferList.Unlock();
        return FALSE;
    }
    
    FTPConveryData tmpFtpData = {0};
    tmpFtpData = m_bufferList.GetHead();
    m_bufferList.RemoveHead();
    m_csbufferList.Unlock();

    //写文件
    FILE *fhandle;
    char tempFileName[MAX_PATH] = {0};

    sprintf(tempFileName,"%s\\%s",m_strFtpFileDir,g_pcTempSaveFileName);

    m_lckFile.Lock();
    fhandle = fopen(tempFileName, "a+");
    if(fhandle == NULL)
    {
        return FALSE;
    }
    if(fseek(fhandle, 0, SEEK_END) != 0)
    {
        fclose(fhandle);
        m_lckFile.Unlock();
        return FALSE;
    }

    size_t uWritten = fwrite((LPCTSTR)tmpFtpData.csData, 1, strlen(tmpFtpData.csData), fhandle);
    fclose(fhandle);
    m_lckFile.Unlock();
    return TRUE;
}

void CFTPConveyFile::OnTimerWriteCheck()
{
    if(m_nFTPWorkMode > 0)
    {
        static int nTickCount = 0;
        if(m_nftpWorkState == FTP_FREE)
        {
            if(nTickCount > m_nFTPTimerOut)
            {
                //检测到有需要上传的temp文件,将文件改名后通知FTP线程准备上传文件,并将之前的文件删除之
                if(DispFTPUpDownState())
                {
                    nTickCount = 0;
                }
                else
                {
                    //没有要上传到FTP的文件则隔2秒后重新
                    nTickCount = nTickCount - 2;
                }
            }
        }
        
        if(nTickCount <= m_nFTPTimerOut)
        {
            nTickCount++;
        }
    }
}

//查找是否有文件需要上传,
BOOL CFTPConveyFile::DispFTPUpDownState()
{
    CFileFind IsValidFile;
    BOOL bNeedFTPUpFlag = FALSE;
    BOOL bNeedFTPDownFlag = FALSE;
    if(m_nFTPWorkMode == 1 || m_nFTPWorkMode == 3)
    {
        char tempFileName[MAX_PATH] = {0};
        sprintf(tempFileName,"%s\\%s",m_strFtpFileDir,g_pcTempSaveFileName);

        if(IsValidFile.FindFile(tempFileName)) 
        {
            bNeedFTPUpFlag = TRUE; //需要将FtpPubNet.txt上传到FTP
        }
    }

    if(m_nFTPWorkMode >= 2)
    {
        bNeedFTPDownFlag = TRUE;
    }
    if(bNeedFTPDownFlag || bNeedFTPUpFlag)
    {
		gWriteLogApp.WriteLog(FOR_TRACE_1,"%s 准备上传文件:%d\n",
			CTime::GetCurrentTime().Format("%H:%M:%S"),bNeedFTPUpFlag);
        if(PostThreadMessage(m_dwFtpThreadID,WM_START_FTP_MESSAGE,(WPARAM)bNeedFTPUpFlag,(LPARAM)bNeedFTPDownFlag))
        {
            //等待
            m_nftpWorkState = FTP_BUSY;
        }
    }
    else
    {
        if(m_nftpWorkState > 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

void CFTPConveyFile::OnFTPUpDownCompleteMsg(BOOL bSuccess)
{
	m_nftpWorkState = FTP_FREE;	
}


///////////////////////////////////////////////////////////////////////////////////////////
//
//FTP上传下载处理线程
UINT WINAPI CFTPConveyFile::FTPDownUpThreadProc(void *lpParameter)
{
    CFTPConveyFile *pMyFtpThread;
    pMyFtpThread = (CFTPConveyFile *)lpParameter;
    
    int timerid = SetTimer(NULL,NULL, 1000, 0L); //产生1s的基准定时
    TRACE(_T("Into FTP thread%X\n"), pMyFtpThread->m_hFTPThread);
    ResetEvent(pMyFtpThread->m_hFTPThreadEndEvt);
    MSG msg;
    BOOL bcontinue = TRUE;
    while(bcontinue)
    {
        if(GetMessage(&msg,NULL,0,0))
        {
            switch(msg.message)
            {
            case WM_THREAD_STOP:
                bcontinue = FALSE;
                break;
            case WM_START_FTP_MESSAGE: 
                pMyFtpThread->OnFtpStartMessage(msg.wParam,msg.lParam); 
				break;
            case WM_TIMER:
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }
    if(timerid != 0)
    {
        KillTimer(NULL, timerid);
    }	
    SetEvent(pMyFtpThread->m_hFTPThreadEndEvt);
    
    TRACE(_T("Quit FTP thread\n"));
    _endthreadex(0);
    return 0;
}

void CFTPConveyFile::OnFtpStartMessage(BOOL bUpFlag, BOOL bDownFlag)
{
    //登录FTP
	m_pInetSession =NULL;
	m_pInetSession=new CInternetSession;
    //m_pInetSession=new CInternetSession(AfxGetAppName(),1,PRE_CONFIG_INTERNET_ACCESS,NULL,NULL,0);
	if(!m_pInetSession )
	{
		return;
	}
    CFtpConnection* m_pFtpConnection;//The ftp_connection
    //m_myInetSession
    try 
    { 
        //新建连接对象
        m_pFtpConnection=m_pInetSession->GetFtpConnection(m_strFTPURL,m_strFTPLoginUser,
            m_strFTPLoginPSWD,m_nFTPPort); 
    } 
    catch(CInternetException *pEx) 
    {
        //获取错误
        TCHAR szError[1024]= {0};
        if(pEx->GetErrorMessage(szError,1024))
        {
			szError[256] = 0;
			gWriteLogApp.WriteLog(FOR_ALARM,"%s [FTP]主控连接FTP失败,原因:%s\n",CTime::GetCurrentTime().Format("%H:%M:%S"),szError);
        }
        else  
        {
            gWriteLogApp.WriteLog(FOR_ALARM,"%s [FTP]主控连接FTP失败!!\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
        }
        pEx->Delete();
		if(m_pInetSession != NULL)
		{
			delete m_pInetSession;
			m_pInetSession = NULL;
		}
        m_pFtpConnection=NULL;
		
		PostThreadMessage(m_dwWriteThreadID,WM_REPORT_FTP_COMPLETE_MSG,FALSE,0);
        
        return;
	}
    
    CFtpFileFind* m_pRemoteFinder;		//远程查找文件对象
    m_pRemoteFinder = new CFtpFileFind(m_pFtpConnection);

    //查询FTP目录中是否有与上传同名文件,有则删除，同时查询是否有要下载的文件
    CString strRemoteFile;
    BOOL res = TRUE;
    if(bUpFlag)
    {
        strRemoteFile.Format("%s",g_pcFtpUpFileName);
        //在FTP上找是否有次文件,有则删除之
        if(m_pRemoteFinder->FindFile(strRemoteFile))
        {
			//表示FTP里的文件没有被读走
            res = m_pFtpConnection->Remove(strRemoteFile);
        }
        
        if(res)
        {
            CFileFind IsValidFile;
            char csTempFileName[MAX_PATH] = {0};
            char csLoadFtpFileName[MAX_PATH] = {0};
            sprintf(csTempFileName,"%s\\%s",m_strFtpFileDir,g_pcTempSaveFileName);
            sprintf(csLoadFtpFileName,"%s\\%s",m_strFtpFileDir,g_pcFtpUpFileName);
            
            if(IsValidFile.FindFile(csTempFileName)) 
            {
                //将临时文件改名后在上传到FTP
 			    int nRes;
                m_lckFile.Lock();
 			    nRes = rename(csTempFileName,csLoadFtpFileName);
                m_lckFile.Unlock();
 			    if(nRes == 0)
 			    {
                    if(m_pFtpConnection->PutFile(csLoadFtpFileName,strRemoteFile))
                    {
                        gWriteLogApp.WriteLog(FOR_ALARM,"%s [FTP]主控上传文件成功\n",CTime::GetCurrentTime().Format("%H:%M:%S"));
                    }	
 			    }
            }
			//
			if(IsValidFile.FindFile(csLoadFtpFileName)) 
			{
				TRY
				{
					CFile::Remove(csLoadFtpFileName);//删除文件
				}
				CATCH( CFileException, e )
				{
					TRACE("删除文件失败\n");
				}
				END_CATCH
			}
        }
    }
    
    if(bDownFlag)
    {
        //下载文件 //?
    }

    delete m_pRemoteFinder;
    m_pRemoteFinder = NULL;
    delete m_pInetSession ;
    m_pInetSession = NULL;

    //FTP上传下载文件处理完成,通知文件处理线程
    PostThreadMessage(m_dwWriteThreadID,WM_REPORT_FTP_COMPLETE_MSG,TRUE,0);
}
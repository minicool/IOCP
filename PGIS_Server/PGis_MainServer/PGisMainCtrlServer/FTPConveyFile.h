//
#ifndef __HYT_FTP_CONVEY_FILE_H_INCLUDED_
#define __HYT_FTP_CONVEY_FILE_H_INCLUDED_

///////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagFTPConverData
{
    int nID;
    char csData[512];
}FTPConveryData;
///////////////////////////////////////////////////////////////////////////////////////////
//
class CFTPConveyFile
{
public:
    CFTPConveyFile();
    ~CFTPConveyFile();
public:
    BOOL StartFTPConvey(UINT nAppThreadID = 0);
    BOOL StopFTPConvey();
    //void WriteData(int nType,const char *format , ... ) ;
    void WriteData(int nType,const char *pcaData ) ;
private:
    BOOL LoadFtpConfig();
	void CreateFtpConveyDir();
private:
    static UINT WINAPI WriteDataThreadProc(void *lpParameter);
    BOOL OnWriteFtpFile();
    void OnTimerWriteCheck();
    void OnFTPUpDownCompleteMsg(BOOL bSuccess);

    BOOL DispFTPUpDownState();

private:
    static UINT WINAPI FTPDownUpThreadProc(void *lpParameter);
    void OnFtpStartMessage(BOOL bUpFlag, BOOL bDownFlag);
    
private:
    BOOL m_bFTPStart;
    UINT m_dwAppThreadID;

    CList<FTPConveryData,FTPConveryData&> m_bufferList;
	::CCriticalSection m_csbufferList;

    ::CCriticalSection m_lckFile;

    UINT m_dwWriteThreadID;
    HANDLE m_hWriteThread;
    HANDLE m_hWriteThreadEndEvt;

private:
    UINT m_dwFtpThreadID;

    HANDLE m_hFTPThread;
    HANDLE m_hFTPThreadEndEvt;
   
    int m_nftpWorkState;
    
    char m_strFTPURL[512];
    int m_nFTPWorkMode;
    char m_strFTPLoginUser[256];
    char m_strFTPLoginPSWD[256];
    char m_strDownLoadSavePath[MAX_PATH];

    CInternetSession *m_pInetSession;
 
	TCHAR m_strFtpFileDir[_MAX_PATH];

	int m_nFTPTimerOut;
	int m_nFTPPort;
};

#endif
/*********************************************************************
* Copyright (c) 2009
* All rights reserved.
* 
* .文件名称：WrteLogThread.h
* 文件标识：
* 摘    要：写日志的公共接口
* 历史修改记录：
*   作者            日期            版本            描述
*   
*********************************************************************/

#ifndef __MY_WRITELOG_THREAD_H_INCLUDED_
#define __MY_WRITELOG_THREAD_H_INCLUDED_

typedef enum enumLogType
{
    FOR_ALARM = 0,
    FOR_TRACE,
    FOR_DEBUG
}LOG_TYPE;

typedef struct tagLogMessage
{
    LOG_TYPE logType;
    char LogData[512];
}LogMessage;

class CWriteLogThread
{
public:
    CWriteLogThread();
    ~CWriteLogThread();
    
    void WriteLog(LOG_TYPE mode,const char *format , ... );
    void WriteLog(LOG_TYPE mode,CString strLogTrace);
    void SetLogLevel(int level);
    void SetLogFileCleanupDate(int nDateCount);

    BOOL StartLog(const char *pcsLogFileName,int level = 0,int nCleanupDate = 0);
    void StopLog();

protected:
    static UINT WINAPI WriteLogThreadProc(void *lpParameter);
    
    void CreateCurrAppLogDir();

    void UpDataLogFileName();
    void RefreshLogFileName();
    void OnLogListFull();

    int GetSaveRecordDiskFreeSpace(); 

    void CheckCleanupFileLogFile(); 
    BOOL WriteLogFile();

private:

    BOOL m_bLogListFull ;
    int m_nLogListCount;
        
    BOOL m_bStartFlag;
    UINT m_dwLogThreadID;
    HANDLE m_hLogThread;
    HANDLE m_hLogThreadEndEvt;

    CList<LogMessage,LogMessage&> m_LogList;
    CCriticalSection m_csLogList;

    

     TCHAR m_strAppLogDir[_MAX_PATH];

     char m_LogFileName[MAX_PATH];

     char m_csLogFileName[64];

     int m_nLogLevel;

     int m_nLogFileIndex;
     UINT m_uBytesWritten;
     UINT m_uMaxFileSize;

     CString m_strCurrData;
     CString m_strLastCleanupLogFileDate;

     int m_nLogFileCleanupDate;

private:
    ULARGE_INTEGER m_nFreeBytesAvailableOfBuf;
    ULARGE_INTEGER m_nTotalNumberOfBytesOfBuf;
    ULARGE_INTEGER m_nTotalNumberOfFreeBytesOfBuf;
};

#endif
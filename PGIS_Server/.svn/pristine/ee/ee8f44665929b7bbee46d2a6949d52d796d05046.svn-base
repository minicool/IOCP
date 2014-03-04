#include "stdafx.h"
#include "swmrglock.h"

CSwmrgLock::CSwmrgLock(void)
{
	m_hMutexNoWriter = NULL;
	m_hEventNoReaders = NULL;
	// modify by lvxiaoming
	InterlockedExchange(&m_hNumReaders,0);
	// end modify
	Initialize(NULL);
}

CSwmrgLock::~CSwmrgLock(void)
{
	Free();
}

BOOL CSwmrgLock::Initialize(LPCTSTR lpszName)
{
	m_hMutexNoWriter = CreateMutex(NULL, FALSE, lpszName);
	m_hEventNoReaders = CreateEvent(NULL, TRUE, TRUE, lpszName);

	if ((NULL == m_hMutexNoWriter) || (NULL == m_hEventNoReaders))
	{
		Free();
		return FALSE;
	}
	// modify by lvxiaoming
	InterlockedExchange(&m_hNumReaders,0);
	// end modify
	return TRUE;
}

void CSwmrgLock::Free(void)
{
	if (NULL != m_hMutexNoWriter)
	{
		CloseHandle(m_hMutexNoWriter);
		m_hMutexNoWriter = NULL;
	}
	if (NULL != m_hEventNoReaders)
	{
		CloseHandle(m_hEventNoReaders);
		m_hEventNoReaders = NULL;
	}
	// modify by lvxiaoming
	InterlockedExchange(&m_hNumReaders,0);
	// end modify
}

DWORD CSwmrgLock::WaitToWrite(DWORD dwTimeOut)
{
	// modify by lvxiaoming
	HANDLE aHandles[2] = { m_hMutexNoWriter,m_hEventNoReaders };
	DWORD dw = WaitForMultipleObjects(sizeof(aHandles)/sizeof(HANDLE),aHandles,TRUE,dwTimeOut);
	// end modify
	return dw;
}

void CSwmrgLock::DoneWriting(void)
{
	ReleaseMutex(m_hMutexNoWriter);
}

DWORD CSwmrgLock::WaitToRead(DWORD dwTimeout)
{
	DWORD dw = WaitForSingleObject(m_hMutexNoWriter, dwTimeout);
	if(dw == WAIT_OBJECT_0)
	{
		//以前无读者，现在有读者，将无读者事件重置
		if(m_hNumReaders == 0)
			//Sets the state of the event to nonsignaled until explicitly set to signaled by the SetEvent member function.
			ResetEvent(m_hEventNoReaders);
		// modify by lvxiaoming
		InterlockedIncrement(&m_hNumReaders);
		// end modify
		ReleaseMutex(m_hMutexNoWriter);
	}

	return dw;
}

void CSwmrgLock::DoneReading(void)
{
	WaitForSingleObject(m_hMutexNoWriter, INFINITE) ;
	//将无读者事件重置

	// modify by lvxiaoming
	InterlockedDecrement(&m_hNumReaders);
	// end modify
	if( m_hNumReaders == 0 )
	{
		SetEvent(m_hEventNoReaders);
	}
	
	ReleaseMutex(m_hMutexNoWriter);		
}

void CSwmrgLock::WriteToRead()
{
	// modify by lvxiaoming
	if(m_hNumReaders == 0)
	{
		InterlockedIncrement(&m_hNumReaders);
		ResetEvent(m_hEventNoReaders);
	}
	// end modify
	
	ReleaseMutex(m_hMutexNoWriter);	
}

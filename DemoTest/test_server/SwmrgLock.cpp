#include "stdafx.h"
#include "swmrglock.h"

CSwmrgLock::CSwmrgLock(void)
{
	m_hMutexNoWriter = NULL;
	m_hEventNoReaders = NULL;
	m_hNumReaders = 0;
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

	m_hNumReaders = 0;
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
	m_hNumReaders = 0;
}

DWORD CSwmrgLock::WaitToWrite(DWORD dwTimeOut)
{
	DWORD dw;
	HANDLE aHandles[2];

	//等待无读写者
	aHandles[0] = m_hMutexNoWriter;
	aHandles[1] = m_hEventNoReaders;
	dw = WaitForMultipleObjects(2, aHandles, TRUE, dwTimeOut);
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
		m_hNumReaders++;
		
		ReleaseMutex(m_hMutexNoWriter);
	}

	return dw;
}

void CSwmrgLock::DoneReading(void)
{
	WaitForSingleObject(m_hMutexNoWriter, INFINITE) ;
	//将无读者事件重置
	--m_hNumReaders;
	if(m_hNumReaders == 0)
		SetEvent(m_hEventNoReaders);
	
	ReleaseMutex(m_hMutexNoWriter);		
}

void CSwmrgLock::WriteToRead()
{
	if(m_hNumReaders++ == 0)
		ResetEvent(m_hEventNoReaders);
	
	ReleaseMutex(m_hMutexNoWriter);	
}

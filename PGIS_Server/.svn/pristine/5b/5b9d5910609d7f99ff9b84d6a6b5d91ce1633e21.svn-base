// Thread.h

#ifndef	__THREAD_H__
#define __THREAD_H__

#define PRIORITY_HIGHEST_OUR	THREAD_PRIORITY_HIGHEST
#define PRIORITY_HIGH_OUR		THREAD_PRIORITY_ABOVE_NORMAL
#define PRIORITY_NORMAL_OUR		THREAD_PRIORITY_NORMAL
#define PRIORITY_LOW_OUR		THREAD_PRIORITY_BELOW_NORMAL
#define PRIORITY_LOWEST_OUR		THREAD_PRIORITY_LOWEST

class  CThread
{
public:
	CThread()
	{
		m_hThread = NULL;
	}

	virtual ~CThread(){	}
	

public:
	HANDLE	m_hThread;
	DWORD	m_dwThreadID;

public:

	virtual BOOL StartThread(void){

		if ( m_hThread==NULL )
		{
			m_hThread = CreateThread(
						NULL,
						0,
						InitThreadProc,
						this,
						0,
						&m_dwThreadID);
                
		}
		return	m_hThread != NULL;
	}

	virtual DWORD Suspend(void){return SuspendThread(m_hThread);}
	virtual DWORD Resume(void){return ResumeThread(m_hThread);}
	virtual BOOL  SetPriority(int nPriority){return SetThreadPriority(m_hThread,nPriority);}
	virtual BOOL GetExitCode(DWORD *pdwExitCode){return ::GetExitCodeThread(m_hThread,pdwExitCode);}

	virtual	void StopThread(void)
	{
		if (m_hThread != NULL){			
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	virtual DWORD	ThreadProcEx() { return 0;}
protected:
	static DWORD WINAPI InitThreadProc(PVOID pObj)
	{
		return ((CThread *)pObj)->ThreadProcEx();
	}
};

#endif
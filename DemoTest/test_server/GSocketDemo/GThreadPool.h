#pragma once

#include "GSocket.h"

typedef struct _GTHREAD
{
	_GTHREAD*			pNext;
	void*				pfnThreadProc;
	char*				pThreadName;
	DWORD				dwThreadId;
	HANDLE				hFinished;

	DWORD				ttType;
	DWORD				dwRunCount;
	DWORD				dwState;
	BOOL				bIsShutdown;
	void*				pData;
}GTHREAD, *PGTHREAD;

typedef enum _GTHREAD_STATE
{
	GTHREAD_STATE_SLEEP,
	GTHREAD_STATE_WORKING1,
	GTHREAD_STATE_WORKING2,
	GTHREAD_STATE_WORKING3,
	GTHREAD_STATE_WORKING4,
	GTHREAD_STATE_WORKING5,
}GTHREAD_STATE;

typedef	void(*PFN_ON_GTHREAD_PROC)(PGTHREAD pThread);

void GThrd_CreateThread(PGTHREAD pThread, GTHREAD_TYPE ttType, char* szThreadName, PFN_ON_GTHREAD_PROC pfnThreadProc);
void GThrd_DestroyThread(PGTHREAD pThread);

void GThrd_Create(void);
void GThrd_Destroy(void);

template<class T>
class GThreadPool
{
public:
	typedef typename T::HANDLE HANDLE;

	GThreadPool(void)
	{
	}
public:

	~GThreadPool(void)
	{
	}

void GThrd_CreateThread(PGTHREAD pThread, GTHREAD_TYPE ttType, char* szThreadName, PFN_ON_GTHREAD_PROC pfnThreadProc);
};

template<class T>
void GThreadPool::GThrd_CreateThread(T* pThread, GTHREAD_TYPE ttType, char* szThreadName, PFN_ON_GTHREAD_PROC pfnThreadProc)
{

}


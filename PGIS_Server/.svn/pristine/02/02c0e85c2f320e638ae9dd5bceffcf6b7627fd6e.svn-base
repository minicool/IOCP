#pragma once

class CSwmrgLock
{
public:
	CSwmrgLock(void);
	~CSwmrgLock(void);
private:
	BOOL Initialize(LPCTSTR lpszName);
	void Free(void);

	HANDLE m_hMutexNoWriter;
	HANDLE m_hEventNoReaders;
	DWORD  m_hNumReaders;
//	HANDLE m_hSemNumReaders;	
public:
	DWORD WaitToWrite(DWORD dwTimeOut);
	void DoneWriting(void);
	
	void DoneReading(void);
	void WriteToRead();
	DWORD WaitToRead(DWORD dwTimeout);
};

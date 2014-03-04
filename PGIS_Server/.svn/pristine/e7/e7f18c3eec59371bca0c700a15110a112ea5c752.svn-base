// MapManager.h: interface for the CMapManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPMANAGER_H__31072674_DCA3_4F0D_8DAD_0EE8BB8C6F28__INCLUDED_)
#define AFX_MAPMANAGER_H__31072674_DCA3_4F0D_8DAD_0EE8BB8C6F28__INCLUDED_
#include <algorithm>
#include <map>
#include "SwmrgLock.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef BOOL(*PFN_ON_TRAVERSAL)(const DWORD dwInstance,const DWORD dwClientContext,const DWORD dwDataContext);

template <class T, class Y>
class CMapManager : public std::map<T, Y>, public CSwmrgLock
{
	typedef typename map<T, Y>::iterator mapIterator;
public:

	BOOL IsEmpty();
	void ClearAll();
	BOOL FindItem(T key, Y& Data);
	BOOL Find(T key);
	BOOL GetItem(int nCount, Y& Data);	//BAD CODE!
	void AddItem(T key, Y Data);
	void DelItem(T key);
	void DeleteItem(T key, Y& Data);
	BOOL Delete(T key);
	int GetSize();

	void TraversalItem(const DWORD,const DWORD ,PFN_ON_TRAVERSAL);
	CMapManager();
	virtual ~CMapManager();
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T, class Y>
CMapManager<T,Y>::CMapManager()
{
}

template <class T, class Y>
CMapManager<T,Y>::~CMapManager()
{
	clear();
}

template <class T, class Y>
void CMapManager<T,Y>::AddItem(T key, Y Data)
{
	pair<T, Y> value(key, Data);
	WaitToWrite(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
		erase(mapIter);
	insert(value);
	DoneWriting();
}

template <class T, class Y>
void CMapManager<T,Y>::DelItem(T key)
{
	WaitToWrite(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
		erase(mapIter);
	DoneWriting();
}

template <class T, class Y>
void CMapManager<T,Y>::DeleteItem(T key, Y& Data)
{
	WaitToWrite(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
	{
		Data = (*mapIter).second;
		erase(mapIter);
	}
	DoneWriting();
}

template <class T, class Y>
BOOL CMapManager<T,Y>::Delete(T key)
{
	BOOL RetV = FALSE;
	WaitToWrite(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
	{
		erase(mapIter);
		RetV = TRUE;
	}
	DoneWriting();	

	return RetV;
}


template <class T, class Y>
BOOL CMapManager<T,Y>::FindItem(T key, Y& Data)
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
	{
		Data = (*mapIter).second;
		RetV = TRUE;
	}
	DoneReading();

	return RetV;
}

template <class T, class Y>
BOOL CMapManager<T,Y>::Find(T key)
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	mapIterator mapIter = find(key);
	if (mapIter != end())
	{
		RetV = TRUE;
	}
	DoneReading();

	return RetV;
}

template <class T, class Y>
BOOL CMapManager<T,Y>::GetItem(int nCount, Y& Data)
{
	BOOL RetV = FALSE;
	int iRetV = 0;
	WaitToRead(INFINITE);
	mapIterator mapIter = begin();
	while (mapIter != end())
	{
		if (iRetV == nCount)
		{
			Data = (*mapIter).second;
			RetV = TRUE;
			return RetV;
		}
		++mapIter;
		++iRetV;
	}
	DoneReading();

	return RetV;
}

template <class T, class Y>
void CMapManager<T,Y>::TraversalItem(const DWORD dwInstance,const DWORD dwClientContext,PFN_ON_TRAVERSAL pfnTraversal)
{
	WaitToRead(INFINITE);
	mapIterator mapIter = begin();
	while (mapIter != end())
	{
		pfnTraversal(dwInstance,dwClientContext,DWORD(&(*mapIter).second));
		++mapIter;
	}
	DoneReading();
}

template <class T, class Y>
int CMapManager<T,Y>::GetSize()
{
	int iRetV = 0;
	WaitToRead(INFINITE);
	iRetV = size();
	DoneReading();

	return iRetV;
}


template <class T, class Y>
void CMapManager<T,Y>::ClearAll()
{
	WaitToWrite(INFINITE);
	clear();
	DoneWriting();
}


template <class T, class Y>
BOOL CMapManager<T,Y>::IsEmpty()
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	RetV = empty();
	DoneReading();

	return RetV;
}

#endif // !defined(AFX_MAPMANAGER_H__31072674_DCA3_4F0D_8DAD_0EE8BB8C6F28__INCLUDED_)

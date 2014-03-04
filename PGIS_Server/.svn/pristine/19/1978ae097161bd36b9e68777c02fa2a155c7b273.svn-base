#pragma once

#if !defined(AFX_HASHMAPMANAGER_H)
#define AFX_HASHMAPMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <hash_map>
#include <algorithm>
#include "SwmrgLock.h"
using namespace stdext;
using namespace std;

typedef BOOL(*PFN_ON_TRAVERSAL)(const DWORD dwInstance,const DWORD dwClientContext,const DWORD dwDataContext);

template <class T, class Y>
class CHashmapManager : public hash_map<T, Y>, public CSwmrgLock
{
	typedef typename hash_map<T, Y>::iterator hashmapIterator;
public:

	BOOL IsEmpty();
	void ClearAll();
	BOOL FindItem(T key, Y& Data);
	BOOL Find(T key);
	void AddItem(T key, Y Data);
	void DelItem(T key);
	void DeleteItem(T key, Y& Data);
	BOOL Delete(T key);
	int GetSize();

	void TraversalItem(const DWORD,const DWORD ,PFN_ON_TRAVERSAL);
	CHashmapManager();
	virtual ~CHashmapManager();
};

template <class T, class Y>
CHashmapManager<T,Y>::CHashmapManager()
{
}

template <class T, class Y>
CHashmapManager<T,Y>::~CHashmapManager()
{
	clear();
}

template <class T, class Y>
void CHashmapManager<T,Y>::AddItem(T key, Y Data)
{
	pair<T, Y> value(key, Data);
	WaitToWrite(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
		erase(mapIter);
	insert(value);
	DoneWriting();
}

template <class T, class Y>
void CHashmapManager<T,Y>::DelItem(T key)
{
	WaitToWrite(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
		erase(mapIter);
	DoneWriting();
}

template <class T, class Y>
void CHashmapManager<T,Y>::DeleteItem(T key, Y& Data)
{
	WaitToWrite(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
	{
		Data = (*mapIter).second;
		erase(mapIter);
	}
	DoneWriting();
}

template <class T, class Y>
BOOL CHashmapManager<T,Y>::Delete(T key)
{
	BOOL RetV = FALSE;
	WaitToWrite(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
	{
		erase(mapIter);
		RetV = TRUE;
	}
	DoneWriting();	

	return RetV;
}


template <class T, class Y>
BOOL CHashmapManager<T,Y>::FindItem(T key, Y& Data)
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
	{
		Data = (*mapIter).second;
		RetV = TRUE;
	}
	DoneReading();

	return RetV;
}

template <class T, class Y>
BOOL CHashmapManager<T,Y>::Find(T key)
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	hashmapIterator mapIter = find(key);
	if (mapIter != end())
	{
		RetV = TRUE;
	}
	DoneReading();

	return RetV;
}

template <class T, class Y>
void CHashmapManager<T,Y>::TraversalItem(const DWORD dwInstance,const DWORD dwClientContext,PFN_ON_TRAVERSAL pfnTraversal)
{
	WaitToRead(INFINITE);
	hashmapIterator mapIter = begin();
	while (mapIter != end())
	{
		pfnTraversal(dwInstance,dwClientContext,DWORD(&(*mapIter).second));
		++mapIter;
	}
	DoneReading();
}

template <class T, class Y>
int CHashmapManager<T,Y>::GetSize()
{
	int iRetV = 0;
	WaitToRead(INFINITE);
	iRetV = size();
	DoneReading();

	return iRetV;
}


template <class T, class Y>
void CHashmapManager<T,Y>::ClearAll()
{
	WaitToWrite(INFINITE);
	clear();
	DoneWriting();
}


template <class T, class Y>
BOOL CHashmapManager<T,Y>::IsEmpty()
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	RetV = empty();
	DoneReading();

	return RetV;
}

#endif

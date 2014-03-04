// ListManager.h: interface for the CListManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTMANAGER_H__AF521644_7CB1_4B64_95A2_94578A4177A5__INCLUDED_)
#define AFX_LISTMANAGER_H__AF521644_7CB1_4B64_95A2_94578A4177A5__INCLUDED_
#include <list>
#include <algorithm>
#include "SwmrgLock.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class T>
class CListManager : public std::list <T>, public CSwmrgLock
{
	typedef list<T>::iterator listIterator;
public:
	CListManager();
	virtual ~CListManager();
	int GetSize();
	BOOL IsEmpty();
	void PushFront(T x);
	T PopFront();
	void PushBack(T x);
	T PopBack();
	void Delete(T x);
	BOOL DelItem(T x);
	BOOL FindItem(T x);
	BOOL AddItem(T x);
	void ClearAll();
	int GetFront(T& x);
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
CListManager<T>::CListManager()
{
}

template <class T>
CListManager<T>::~CListManager()
{
	clear();
}

template <class T>
void CListManager<T>::PushFront(T x)
{
	WaitToWrite(INFINITE);
	push_front(x);
	DoneWriting();
}

template <class T>
void CListManager<T>::PushBack(T x)
{
	WaitToWrite(INFINITE);
	push_back(x);
	DoneWriting();
}

template <class T>
T CListManager<T>::PopFront()
{
	T RetV = NULL;

	WaitToWrite(INFINITE);
	listIterator listIter = begin();
	if (listIter != end())
	{
		RetV = *listIter;
		erase(listIter);
	}
	DoneWriting();	

	return RetV;
}

template <class T>
int CListManager<T>::GetFront(T& x)
{
	int iRetv = 0;
	WaitToWrite(INFINITE);
	listIterator listIter = begin();
	if (listIter != end())
	{
		x = *listIter;
		erase(listIter);
		iRetv = size();
	}
	else
		iRetv = 0;
	DoneWriting();

	return iRetv;
}


template <class T>
T CListManager<T>::PopBack()
{
	T RetV = NULL;

	WaitToWrite(INFINITE);
	if (!empty())
	{
		listIterator listIter = end();
		listIter--;
		RetV = *listIter;
		erase(listIter);
	}
	DoneWriting();	

	return RetV;
}

template <class T>
BOOL CListManager<T>::IsEmpty()
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	RetV = empty();
	DoneReading();

	return RetV;
}

template <class T>
void CListManager<T>::Delete(T x)
{
	WaitToWrite(INFINITE);
	listIterator listIter = find(begin(), end(), x);
	if (listIter != end())
		erase(listIter);
	DoneWriting();
}

template <class T>
BOOL CListManager<T>::DelItem(T x)
{
	BOOL RetV = FALSE;
	WaitToWrite(INFINITE);
	listIterator listIter = find(begin(), end(), x);
	if (listIter != end())
	{
		erase(listIter);
		RetV = TRUE;
	}
	DoneWriting();		
	return RetV;
}


template <class T>
BOOL CListManager<T>::FindItem(T x)
{
	BOOL RetV = FALSE;
	WaitToRead(INFINITE);
	listIterator listIter = find(begin(), end(), x);
	if (listIter != end())
		RetV = TRUE;
	DoneReading();	

	return RetV;
}

template <class T>
BOOL CListManager<T>::AddItem(T x)
{
	BOOL RetV = TRUE;
	WaitToWrite(INFINITE);
	listIterator listIter = find(begin(), end(), x);
	if (listIter != end())
		RetV = FALSE;
	else
		push_back(x);	
	DoneWriting();	

	return RetV;
}

template <class T>
int CListManager<T>::GetSize()
{
	WaitToRead(INFINITE);
	int RetV = size();
	DoneReading();

	return RetV;
}


template <class T>
void CListManager<T>::ClearAll()
{
	WaitToWrite(INFINITE);
	clear();
	DoneWriting();
}


#endif // !defined(AFX_LISTMANAGER_H__AF521644_7CB1_4B64_95A2_94578A4177A5__INCLUDED_)

// IOSocketUdp.h: interface for the CIOSocketUdp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOSOCKETUDP_H__7FA79554_1138_4FC4_ACE5_F7025D5B790C__INCLUDED_)
#define AFX_IOSOCKETUDP_H__7FA79554_1138_4FC4_ACE5_F7025D5B790C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UDP_DATA 0x0F

#include "Thread.h"

class CIOSocketUdp : public CThread
{
public:
	CIOSocketUdp();
	virtual ~CIOSocketUdp();

	typedef DWORD (WINAPI * LPFUDPDATACALLBACK)(UINT uMsg,DWORD dwParam,DWORD dwInstance);

public:
	int UdpSendTo(PBYTE pbData, int nDataLen,int nDestIP, unsigned short usDestPort);
	int UdpSendTo(PBYTE pbData, int nDataLen,char* szDestIP, unsigned short usDestPort);
	BOOL Start(char* szIPAddr, unsigned short usPort, LPFUDPDATACALLBACK lpfUdpdDataCallBack,DWORD dwInstance);
	void Stop();

protected:
	virtual DWORD	ThreadProcEx();
	
private:
	HANDLE			m_evStop;					//Stop Event
	SOCKET			m_hSocket;					//UDP SOCKET
	BOOL			m_bThreadRun;						//Thread Run
	LPFUDPDATACALLBACK	m_pfudpcallback;			//
	DWORD			m_dwInstance;
};

#endif // !defined(AFX_IOSOCKETUDP_H__7FA79554_1138_4FC4_ACE5_F7025D5B790C__INCLUDED_)

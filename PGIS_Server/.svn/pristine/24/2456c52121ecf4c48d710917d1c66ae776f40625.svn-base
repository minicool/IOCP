// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FCB67C10_306B_4838_9372_4CF8994D9CDB__INCLUDED_)
#define AFX_STDAFX_H__FCB67C10_306B_4838_9372_4CF8994D9CDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4996)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

#include <afxmt.h>			//CCriticalSection
#include <afxtempl.h>		//CList
#include <winsvc.h>
#include <winsock2.h>
#include <process.h>

#include <afxinet.h>
#include <afxtempl.h>

#include "WriteLogThread.h"
#include "TcpInterface.h"

#include <string>
#include <sstream>
#pragma comment(lib, "ws2_32")


#define __HYT_PUB_NET_

#ifndef _DEBUG
#define __SERVICE_MODE_
#endif

//消息队列中消息数据的最大长度
#define MAX_GPS_MESSAGE_LEN             256

#pragma pack(1)


typedef struct tagRecvMessage
{
    int             nClientID;  //TCP连接ID
    unsigned short	length;
    //消息内容
    char MessagePacket[MAX_GPS_MESSAGE_LEN];
}RECV_MESSAGE;

typedef struct tagDispatchMessage
{
    unsigned char   aPPID; // =0 表示发给终端，=1:表示发给主控
    unsigned char   TypeID;
    unsigned short	length;
    //消息内容
    char MessagePacket[MAX_GPS_MESSAGE_LEN];
}DISPATCH_MESSAGE;

#pragma pack()
// TODO: reference additional headers your program requires here
///////////////////////////////////////////////////////////////////////////////////////////////
//
#define SIMUL_USER_MESSAGE_BASE 300

#define WM_WRITE_LOG_MSG                        WM_USER+SIMUL_USER_MESSAGE_BASE + 10

#define WM_LOGLIST_FULL_MSG                     WM_USER+SIMUL_USER_MESSAGE_BASE + 11
//线程停止
#define WM_THREAD_STOP                          WM_USER+SIMUL_USER_MESSAGE_BASE + 20

#define WM_SEND_ACK_TERMINAL_MESSAGE            WM_USER+SIMUL_USER_MESSAGE_BASE + 30

#define WM_RECVICE_PUBGPS_MESSAGE	            WM_USER+SIMUL_USER_MESSAGE_BASE + 31

#define WM_DISPATCH_DATA_MESSAGE                WM_USER+SIMUL_USER_MESSAGE_BASE + 33

#define WM_SET_TERMINAL_GPSINTERVAL_MESSAGE     WM_USER+SIMUL_USER_MESSAGE_BASE + 34

#define WM_SET_TERMINAL_GPSMONITOR_MESSAGE      WM_USER+SIMUL_USER_MESSAGE_BASE + 35

#define WM_FTP_DOWNLOAD_OVER_MESSAGE            WM_USER+SIMUL_USER_MESSAGE_BASE + 36 //FTP下载文件完成

#define WANGZH
#define WANGZH_ALL
#define WANGZH_H6
#define WANGZH_USER

//WANGZH
#define WM_RECVICE_MESSAGE						WM_USER+SIMUL_USER_MESSAGE_BASE + 37
#define WM_SEND_ACK_H6TERMINAL_MESSAGE						WM_USER+SIMUL_USER_MESSAGE_BASE + 38
#define WM_RECVICE_OLD_MESSAGE						WM_USER+SIMUL_USER_MESSAGE_BASE + 39
///////////////////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FCB67C10_306B_4838_9372_4CF8994D9CDB__INCLUDED_)

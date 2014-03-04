/*******************************************************************************
* 版权所有(C) 2013All right reserved
* 文件名称 : SendTransmitManager.h
* 当前版本 : 1.0.0.1
* 作    者 : WANGZH (wangzhenhua@szhitom.com)
* 设计日期 : 2013年4月17日星期三
* 内容摘要 : 
* 修改记录 : 
* 日    期  版    本  修改人   修改摘要

********************************************************************************/
#ifndef HT_SENDTRANSMIT
#define HT_SENDTRANSMIT
/**********************************  头文件 ************************************/
#include "iocp/ListManager.h"
#include "iocp/MapManager.h"
#include "CompletionRoutineServer.h"
/********************************** 常量和宏 **********************************/

/********************************** 数据类型 **********************************/

/********************************** 函数声明 **********************************/

/********************************** 类定义 ***********************************/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSendTransmitManager
{
public:
	CSendTransmitManager(void);
public:
	~CSendTransmitManager(void);

	void Start(DWORD dwInstanced);
	BOOL Add(DWORD socketID,CString DeptNum);
	void Stop();
	BOOL SendAllData(CString DeptNum,const char* pzData, int nDataLen);
	BOOL ViewListData(CString DepNum);

	CListManager<DWORD>* GetCListManagerData(CString DepNum);

private:
	typedef	CMapManager<CString,CListManager<DWORD>* > MAPSendTransmitManager;
	MAPSendTransmitManager m_MapSendTransmitManager;

	CCompletionRoutineServer* m_pTcpServGisSocket;
};

#endif
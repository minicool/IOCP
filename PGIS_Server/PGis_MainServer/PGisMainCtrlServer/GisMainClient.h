/*
**	FILENAME			GisMainClient.h
**
*/
#ifndef __GISMAINCLIENT_H_
#define __GISMAINCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpClient.h"

class CGisMainClient : public CTcpClient
{
public:

    CGisMainClient();
    ~CGisMainClient();

    BOOL FindTerminalNumber(const char *strNumber);
	CString GetLoginName();
	CString GetDeptNum();
	void SetLoginInfo(const char *strLoginName,const char *strDeptNum,const char *strMac = NULL);

	BOOL CheckSendState(CString strDeptNum);

	CString GetLoginMac();

	void ResetLoginInfo();
private:

    BOOL LoadTerminalInfo();

private:
    
	CString m_strLoginName;
	CString m_strLoginMac;
	BOOL m_bNewMode;
	CString m_strDeptNum;
    CList<TERMINALInfo, TERMINALInfo&> m_TerminalInfoList; //终端信息列表,通过查询数据库获取	//WANGZH BUG ？

};

#endif
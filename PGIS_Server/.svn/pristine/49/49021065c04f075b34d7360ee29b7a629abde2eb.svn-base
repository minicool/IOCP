// GisMainClient.cpp: implementation of the Crwini class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "GisMainClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
CGisMainClient::CGisMainClient()
{
	m_strLoginName = _T("");
	m_strDeptNum = _T("");
}

CGisMainClient::~CGisMainClient()
{

}

//////////////////////////////////////////////////////////////////////
//
CString CGisMainClient::GetLoginName()
{
	return m_strLoginName;
}

CString CGisMainClient::GetLoginMac()
{
	return m_strLoginMac;
}

void CGisMainClient::ResetLoginInfo()
{
	m_strLoginName = "";
	m_strLoginMac = "";
	m_strDeptNum = "";
}

CString CGisMainClient::GetDeptNum()
{
	return m_strDeptNum;
}

void CGisMainClient::SetLoginInfo(const char *strLoginName,const char *strDeptNum,const char *strMac)
{
	if(strLoginName != NULL)
	{
		m_strLoginName.Format("%s",strLoginName);
	}

	if(strDeptNum != NULL)
	{
		m_strDeptNum.Format("%s",strDeptNum);
	}

	if(strMac != NULL)
	{
		m_strLoginMac.Format("%s",strMac);
		m_bNewMode = TRUE;
	}
	else
	{
		m_strLoginMac = "";
		m_bNewMode = FALSE;
	}
}

//通过web Service查询数据库,获取属于该GIS客户端管理的终端信息
BOOL CGisMainClient::LoadTerminalInfo()
{
    return TRUE;
}

//查询终端号码是否存在
BOOL CGisMainClient::FindTerminalNumber(const char *strNumber)
{
    POSITION pos = NULL;
    if( m_TerminalInfoList.IsEmpty() )
    {
        return FALSE;
    }

    pos=m_TerminalInfoList.GetHeadPosition();
    while(pos) 
    { 
        //GetNext()返回的是pos指向的当前节点，而不是返回下一个节点,在返回当前节点的同时，
        //pos被GetNext()修改成指向下一个节点
        TERMINALInfo &tempTermainalInfo = m_TerminalInfoList.GetNext(pos);  

        if( strcmp(tempTermainalInfo.strNumber,strNumber) == 0 )
        {
            return TRUE;
        }
    }
   
    return FALSE;
}

BOOL CGisMainClient::CheckSendState(CString strDeptNum)
{
	if(GetConnectState() == TCP_LOGIN_SUCCESS)
	{
		if(strDeptNum != "")
		{
			int nLen = m_strDeptNum.GetLength();
			if(nLen > strDeptNum.GetLength())
			{
				return FALSE;
			}
			strDeptNum = strDeptNum.Mid(0,nLen);
			int nRes = strDeptNum.Compare(m_strDeptNum);
			if(nRes == 0)
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}
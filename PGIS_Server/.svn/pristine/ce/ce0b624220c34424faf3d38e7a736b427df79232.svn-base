// rwini.cpp: implementation of the Crwini class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include "rwini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Crwini::Crwini()
{
    
    memset(m_FilePath,0,sizeof(MAX_PATH)*sizeof(char));
    
    GetModuleFileName(NULL,m_FilePath,255); 
    (_tcsrchr(m_FilePath,'\\'))[1] = 0;   //strrchr

    //strcat(FilePath,fn);//_tcsncat
}

Crwini::~Crwini()
{

}

char * Crwini::GetCurrPath()
{
    return m_FilePath;
}

bool Crwini::WriteString(LPCTSTR appname, LPCTSTR keyname,LPCTSTR s, LPCTSTR fn)
{
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);
	WritePrivateProfileString(appname,keyname,s,strFilePath);
	return 1;
}

void Crwini::WriteInt(LPCTSTR appname,LPCTSTR keyname, int i,LPCTSTR fn)
{
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);

    //_itoa(i,r,10);
    CString strValue;
    strValue.Format(_T("%d"),i);
    WritePrivateProfileString(appname,keyname,strValue,strFilePath);
}

int Crwini::GetInt(LPCTSTR appname, LPCTSTR keyname, LPCTSTR fn)
{
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);
    int ret;
    ret = GetPrivateProfileInt(appname,keyname,0,strFilePath);
    return ret;
}

int Crwini::GetIntX(LPCTSTR appname, LPCTSTR keyname, LPCTSTR fn)
{
	CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);
    int ret;
    ret = GetPrivateProfileInt(appname,keyname,-1,strFilePath);
    return ret;
}

int Crwini::GetIntValue(LPCTSTR appname, LPCTSTR keyname, LPCTSTR fn)
{
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);
    int ret;
    ret = GetPrivateProfileInt(appname,keyname,-1,strFilePath);
    return ret;
}

int Crwini::ReadString(LPCTSTR appname, LPCTSTR keyname, LPCTSTR fn, LPTSTR strContent)
{
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fn);
	int ret;
	ret = ::GetPrivateProfileString(appname,keyname,NULL,strContent,255*sizeof(TCHAR),strFilePath);	
    return ret;
}

BOOL Crwini::CheckIniFile(LPCTSTR fileName)
{
    CFileFind IsValidFile;
    //检查入参合法性
    CString strFilePath;
    strFilePath.Format(_T("%s%s"),m_FilePath,fileName);
    
	if(IsValidFile.FindFile(strFilePath)) 
    {
        return TRUE;
    }
    return FALSE;
}

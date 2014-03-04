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
#ifndef HT_SENDDATA
#define HT_SENDDATA
/**********************************  头文件 ************************************/
#include "../../IOCP/ListManager.h"
#include "../../IOCP/MapManager.h"
#include "rwini.h"
#include <string>
#include <vector>
using namespace std;
/********************************** 常量和宏 **********************************/

/********************************** 数据类型 **********************************/
typedef enum _ENUM_WORDCODE
{
	ALL = 0,
	GPSINFO,
	DEVID,
	DEVTYPE,
	DEPTNUM,
	PATRINFO,
	USERCODE,
	USERTYPE,
}WORDCODE;
/********************************** 函数声明 **********************************/

/********************************** 类定义 ***********************************/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SubscriptionManager
{
public:
	SubscriptionManager(void);
	~SubscriptionManager(void);

private:
	typedef CListManager<std::string> SubscriptionUnit_ListManager,*PSubscriptionUnit_ListManager;
//	SubscriptionUnit_ListManager m_SubscriptionUnit_ListManager;

	typedef	CMapManager<std::string,PSubscriptionUnit_ListManager> SubscriptionData_MAPManager,*PSubscriptionData_MAPManager;
//	typedef	CMapManager<const char*,PSubscriptionUnit_ListManager> SubscriptionData_MAPManager,*PSubscriptionData_MAPManager;
//	SubscriptionData_MAPManager m_SubscriptionData_MAPManager;

	typedef	CMapManager<std::string,PSubscriptionData_MAPManager> SubscriptionInfo_MAPManager,*PSubscriptionInfo_MAPManager;
	SubscriptionInfo_MAPManager m_SubscriptionInfo_MAPManager;

public:
	BOOL StartManager();
	void StopManager();

	BOOL AddSubscription(const char* pszData, const DWORD dwContext);
	BOOL DelSubscription(const char* pszData, const DWORD dwContext);

	BOOL GetSubscriptionInfo_ShanHai(string loginName,const DWORD dwDataContext);
	BOOL GetSubscriptionInfo_GIS(string gpsinfo,const DWORD dwClientContext,const DWORD dwDataContext);
	BOOL ViewAllSubscriptionData();

private:
	static BOOL GetSubscriptionData(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext);
	static BOOL GetSubscriptionUnit(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext);
	static BOOL GetUnitData(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext);

public:
	std::vector<std::string> split(std::string str,std::string pattern);

public:
	BOOL Gis_Subscription_DeptNum(const char *pszLoginNum,const char* pszDeptNum);
	BOOL Shanhai_Subscription_Config(const char *pszLoginNum,const char* psSubscription);
};


#endif
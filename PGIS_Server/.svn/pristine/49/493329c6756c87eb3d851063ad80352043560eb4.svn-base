#include "StdAfx.h"
#include "SubscriptionManager.h"

#include "PGisMainCtrlServer.h"
extern CWriteLogThread gWriteLogApp;
extern CPgisMainSer gPGisMainCtrlProc;

SubscriptionManager::SubscriptionManager(void)
{

}

SubscriptionManager::~SubscriptionManager(void)
{

}

BOOL SubscriptionManager::StartManager()
{
	return TRUE;
}

void SubscriptionManager::StopManager()
{

}

std::vector<std::string> SubscriptionManager::split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

BOOL SubscriptionManager::AddSubscription(const char* pszData, const DWORD dwContext)
{
	std::vector<std::string> temp_vector;		
	std::string temp_str = pszData;
	temp_vector = split(temp_str,",");

	if("" == temp_vector[0] || "" == temp_vector[1])
	{
		return FALSE;
	}

	PSubscriptionData_MAPManager temp_pSubscriptionData_MAPManager;
	PSubscriptionUnit_ListManager temp_pSubscriptionUnit_ListManager;
	if (m_SubscriptionInfo_MAPManager.FindItem(temp_vector[0],temp_pSubscriptionData_MAPManager))
	{
		if (temp_pSubscriptionData_MAPManager->FindItem(temp_vector[1],temp_pSubscriptionUnit_ListManager))
		{
			for (size_t n = 2;n<temp_vector.size();++n)
			{
				temp_pSubscriptionUnit_ListManager->AddItem(temp_vector[n]);
				gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [SYS] [AddSubscription][ID:%s][SUBID:%s][SUBINFO:%s] \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
					temp_vector[0].c_str(),temp_vector[1].c_str(),temp_vector[n].c_str());
			}
		}
		else
		{
			PSubscriptionUnit_ListManager new_pSubscriptionUnit_ListManager = new SubscriptionUnit_ListManager;
//			std::auto_ptr<SubscriptionUnit_ListManager>  new_pSubscriptionUnit_ListManager(new SubscriptionUnit_ListManager);
			for (size_t n = 2;n<temp_vector.size();++n)
			{
				new_pSubscriptionUnit_ListManager->AddItem(temp_vector[n]);
				gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [SYS] [AddSubscription][ID:%s][SUBID:%s][SUBINFO:%s] \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
					temp_vector[0].c_str(),temp_vector[1].c_str(),temp_vector[n].c_str());
			}			

 			temp_pSubscriptionData_MAPManager->AddItem(temp_vector[1],new_pSubscriptionUnit_ListManager);
		}
	}
	else
	{
		PSubscriptionUnit_ListManager new_pSubscriptionUnit_ListManager = new SubscriptionUnit_ListManager;
		int m = temp_vector.size();
		for (size_t n = 2;n<temp_vector.size();++n)
		{
			new_pSubscriptionUnit_ListManager->AddItem(temp_vector[n]);
			gWriteLogApp.WriteLog(FOR_DEBUG,_T("%s [SYS] [AddSubscription][ID:%s][SUBID:%s][SUBINFO:%s] \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),
				temp_vector[0].c_str(),temp_vector[1].c_str(),temp_vector[n].c_str());
		}
		
		PSubscriptionData_MAPManager new_pSubscriptionData_MAPManager = new SubscriptionData_MAPManager;
		new_pSubscriptionData_MAPManager->AddItem(temp_vector[1],new_pSubscriptionUnit_ListManager);

		m_SubscriptionInfo_MAPManager.AddItem(temp_vector[0],new_pSubscriptionData_MAPManager);
	}

	return TRUE;
}

BOOL SubscriptionManager::DelSubscription(const char* pszData, const DWORD dwContext)
{
	std::vector<std::string> temp_vector;		
	std::string temp_str = pszData;
	temp_vector = split(temp_str,",");

	if("" == temp_vector[0] || "" == temp_vector[1])
	{
		return FALSE;
	}

	PSubscriptionData_MAPManager temp_pSubscriptionData_MAPManager;
	PSubscriptionUnit_ListManager temp_pSubscriptionUnit_ListManager;
	if (m_SubscriptionInfo_MAPManager.FindItem(temp_vector[0],temp_pSubscriptionData_MAPManager))
	{
		if (temp_pSubscriptionData_MAPManager->FindItem(temp_vector[1],temp_pSubscriptionUnit_ListManager))
		{
			for (int n = 2;n<temp_vector.size();++n)
			{
				temp_pSubscriptionUnit_ListManager->DelItem(temp_vector[n]);
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SubscriptionManager::GetSubscriptionInfo_GIS(string gpsinfo,const DWORD dwClientContext,const DWORD dwDataContext)
{
	BOOL bResIs = FALSE;

	PSubscriptionData_MAPManager temp_pSubscriptionData_MAPManager;
	PSubscriptionUnit_ListManager temp_pSubscriptionUnit_ListManager;

	CBusinessManage_ConToPub::PTERMINAL_INFO ptemp_datainfo = (CBusinessManage_ConToPub::PTERMINAL_INFO)dwDataContext;

	CBusinessManage_GISTerminal::DATA_GISTERMINAL temp_gisdatainfo;
	if (!gPGisMainCtrlProc.m_BusinessManage_GISTerminal.m_HashmapDataGISTerminal.FindItem(dwClientContext,temp_gisdatainfo))
	{
		gWriteLogApp.WriteLog( FOR_DEBUG,_T("%s [SubscriptionManager] [GetSubscriptionInfo_GIS] 订阅用户信息 %s 未查询到GIS订阅列表 \n"),
			CTime::GetCurrentTime().Format("%H:%M:%S"),ptemp_datainfo->szDeviceNum);
		return FALSE;
	}

	gWriteLogApp.WriteLog( FOR_DEBUG,_T("%s [SubscriptionManager] [GetSubscriptionInfo_GIS] 订阅用户信息 %s %s %s  订阅登录名称：%s \n"),
		CTime::GetCurrentTime().Format("%H:%M:%S"),ptemp_datainfo->szDeviceNum,ptemp_datainfo->szDeviceType,ptemp_datainfo->szDeptNum,temp_gisdatainfo.szLoginName);
	
	if (m_SubscriptionInfo_MAPManager.FindItem(temp_gisdatainfo.szLoginName,temp_pSubscriptionData_MAPManager))
	{	
		int nKeyWord = ALL;
		string temp_Searchkeyword = "";
		string temp_Keyword = "";
		while(nKeyWord<=DEPTNUM)
		{
			switch(nKeyWord)
			{
			case ALL:
				temp_Keyword = "ALL";
				break;
			case GPSINFO:
				temp_Keyword = "GPSINFO";
				temp_Searchkeyword = gpsinfo;
				break;
			case DEVID:
				temp_Keyword = "DEVID";
				temp_Searchkeyword = ptemp_datainfo->szDeviceNum;
				break;
			case DEVTYPE:
				temp_Keyword = "DEVTYPE";
				temp_Searchkeyword = ptemp_datainfo->szDeviceType;
				break;
			case DEPTNUM:
				temp_Keyword = "DEPTNUM";
				temp_Searchkeyword = ptemp_datainfo->szDeptNum;
				break;
// 			case USERCODE:
// 				temp_Keyword = "USERCODE";
// 				temp_Searchkeyword = ptemp_datainfo->szUserCode;
// 				break;
// 			case USERTYPE:
// 				temp_Keyword = "USERTYPE";
// 				temp_Searchkeyword = itoa(ptemp_datainfo->nUserType);
// 				break;
// 			case PATRINFO:
// 				temp_Keyword = "PATRINFO";
// 				temp_Searchkeyword = itoa(ptemp_datainfo->bPatrolStart);
// 				break;
			}

			++nKeyWord;

			if (temp_pSubscriptionData_MAPManager->FindItem(temp_Keyword,temp_pSubscriptionUnit_ListManager))
			{
				if ("ALL" == temp_Keyword)
				{
					return TRUE;
				}
				if (/*NULL == strncmp("DEPTNUM",temp_Searchkeyword.c_str(),temp_Searchkeyword.size())*/DEPTNUM == (nKeyWord -1))
				{
					string tmp_strkey = temp_Searchkeyword;
					for (size_t ncount = 0;ncount < (tmp_strkey.size()/3);++ncount)
					{
						string tmp_str = tmp_strkey.substr(0,(ncount+1)*3);
						if (temp_pSubscriptionUnit_ListManager->FindItem(tmp_str))
						{
							bResIs = TRUE;
						}
					}
				}else if (temp_pSubscriptionUnit_ListManager->FindItem(temp_Searchkeyword))
				{
					bResIs = TRUE;
				}
				else if (0 != temp_pSubscriptionUnit_ListManager->GetSize())
				{
					return FALSE;
				}
			}
			else
			{
				continue;
			}
		}
	}

	return bResIs;
}

BOOL SubscriptionManager::GetSubscriptionInfo_ShanHai(string loginName,const DWORD dwDataContext)
{
	BOOL bResIs = FALSE;

	PSubscriptionData_MAPManager temp_pSubscriptionData_MAPManager;
	PSubscriptionUnit_ListManager temp_pSubscriptionUnit_ListManager;

	CBusinessManage_ConToPub::PTERMINAL_INFO ptemp_datainfo = (CBusinessManage_ConToPub::PTERMINAL_INFO)dwDataContext;

	if (m_SubscriptionInfo_MAPManager.FindItem(loginName,temp_pSubscriptionData_MAPManager))
	{	
		int nKeyWord = ALL;
		string temp_Searchkeyword = "";
		string temp_Keyword = "";
		while(nKeyWord<=DEPTNUM)
		{
			switch(nKeyWord)
			{
			case ALL:
				temp_Keyword = "ALL";
				break;
// 			case GPSINFO:
// 				temp_Keyword = "GPSINFO";
// 				temp_Searchkeyword = gpsinfo;
// 				break;
			case DEVID:
				temp_Keyword = "DEVID";
				temp_Searchkeyword = ptemp_datainfo->szDeviceNum;
				break;
			case DEVTYPE:
				temp_Keyword = "DEVTYPE";
				temp_Searchkeyword = ptemp_datainfo->szDeviceType;
				break;
			case DEPTNUM:
				temp_Keyword = "DEPTNUM";
				temp_Searchkeyword = ptemp_datainfo->szDeptNum;
				break;
			}

			++nKeyWord;

			if (temp_pSubscriptionData_MAPManager->FindItem(temp_Keyword,temp_pSubscriptionUnit_ListManager))
			{
				if ("ALL" == temp_Keyword)
				{
					return TRUE;
				}
				if (DEPTNUM == (nKeyWord -1))
				{
					string tmp_strkey = temp_Searchkeyword;
					for (size_t ncount = 0;ncount < (tmp_strkey.size()/3);++ncount)
					{
						string tmp_str = tmp_strkey.substr(0,(ncount+1)*3);
						if (temp_pSubscriptionUnit_ListManager->FindItem(tmp_str))
						{
							bResIs = TRUE;
						}
					}
				}else if (temp_pSubscriptionUnit_ListManager->FindItem(temp_Searchkeyword))
				{
					bResIs = TRUE;
				}
				else if (0 != temp_pSubscriptionUnit_ListManager->GetSize())
				{
					return FALSE;
				}
			}
			else
			{
				continue;
			}
		}
	}

	return bResIs;
}

BOOL SubscriptionManager::ViewAllSubscriptionData()
{
	m_SubscriptionInfo_MAPManager.TraversalItem((DWORD)(this),NULL,SubscriptionManager::GetSubscriptionData);

 	return TRUE;
}

BOOL SubscriptionManager::GetSubscriptionData(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	SubscriptionManager* temp_this = (SubscriptionManager*)dwInstaned;
	PSubscriptionData_MAPManager* ptemp_datainfo = (PSubscriptionData_MAPManager*)dwDataContext;
	(*ptemp_datainfo)->TraversalItem(dwInstaned,dwClientContext,SubscriptionManager::GetSubscriptionUnit);

	return TRUE;
}

BOOL SubscriptionManager::GetSubscriptionUnit(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	SubscriptionManager* temp_this = (SubscriptionManager*)dwInstaned;
	PSubscriptionUnit_ListManager* ptemp_datainfo = (PSubscriptionUnit_ListManager*)dwDataContext;
	(*ptemp_datainfo)->TraversalItem(dwInstaned,dwClientContext,SubscriptionManager::GetUnitData);

	return TRUE;
}

BOOL SubscriptionManager::GetUnitData(const DWORD dwInstaned,const DWORD dwClientContext,const DWORD dwDataContext)
{
	SubscriptionManager* temp_this = (SubscriptionManager*)dwInstaned;
	string* ptemp_datainfo = (string*)dwDataContext;
	gWriteLogApp.WriteLog(FOR_TRACE,_T("%s [SYS] [SubscriptionManager] %s \n"),CTime::GetCurrentTime().Format("%H:%M:%S"),(*ptemp_datainfo).c_str());
	return TRUE;
}


BOOL SubscriptionManager::Gis_Subscription_DeptNum(const char *pszLoginNum,const char* pszDeptNum)
{
	char sendBuf[SEND_DATA_PACKAGE_LEN] = {0};
	if (0 == strcmp(pszDeptNum,"0000"))
	{
		sprintf(sendBuf,"%s,ALL,0000",pszLoginNum);			
	}
	else
	{
		sprintf(sendBuf,"%s,DEPTNUM,%s",pszLoginNum,pszDeptNum);
	}
	return AddSubscription(sendBuf,strlen(sendBuf));
}

BOOL SubscriptionManager::Shanhai_Subscription_Config(const char *pszLoginNum,const char* psSubscription)
{
	char sendBuf[SEND_DATA_PACKAGE_LEN*2] = {0};
	sprintf(sendBuf,"%s,%s",pszLoginNum,psSubscription);
	return AddSubscription(sendBuf,strlen(sendBuf));
}
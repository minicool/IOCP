// HttpInterface.h: interface for the CHttpInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPINTERFACE_H__02A1857E_3DB2_4BC4_BA77_752543FE2480__INCLUDED_)
#define AFX_HTTPINTERFACE_H__02A1857E_3DB2_4BC4_BA77_752543FE2480__INCLUDED_


typedef BOOL (*PFN_CallBackDownFile)(LPVOID lparam,int nSize,CString strBaseInfo);
typedef BOOL (*PFN_CallBackUploadFile)(LPVOID lparam,int nPos,CString strBaseInfo);

typedef enum __tagENJXSystemType
{
	en_JX_UserRightAction = 0,
	en_JX_IndexManageAction,
	en_JX_PlanManageAction,
	en_JX_TempManageAction,
	en_JX_IndexExamineManageAction,
	en_JX_ScoreReleaseAction,
	en_JX_WorkLogAction,
	en_JX_ScoreShowAction,
	en_JX_OtherBusinessAction,
	en_JX_UserLoginSfzhAction,
	en_JX_LoginOutAction
}ENJXSystemType;
#define MAX_PARAM_ITEM_COUNT	50
typedef struct __tagParamItem
{
	char  szParamName[MAX_PATH];
	char* lpParam;
}TParamItem,*ParamItem;
typedef struct __tagTHttpInterfaceData
{
	ENJXSystemType enType;
	char		szFunName[MAX_PATH];
	char		szErrorCode[MAX_PATH];
	int			nResultCode;
	TParamItem	tInParam[MAX_PARAM_ITEM_COUNT];	
	TParamItem	tOutParam[MAX_PARAM_ITEM_COUNT];
	int			nInItemCount;
	int			nOutItemCount;
}THttpInterfaceData,*PTHttpInterfaceData;

class CHttpInterface
{
public:
	CHttpInterface();
	virtual ~CHttpInterface();
	static BOOL m_bBadNet;
	static BOOL m_bInCheck;
	static void	Init();
	static void OnDestory();
	static BOOL ShowWait();
	static BOOL HideWait();

	static CString GetResultXML(CString strValue);
	BOOL		DownloadTable(CString strTableName,CString strTime, CString strFile);

	//下载文件
	static BOOL DownFile(CString strURL,CString strSave);

	//下载文件，带有信息提示和进度
	static BOOL DownFileEx(CString strURL,CString strSave,PFN_CallBackDownFile pfn,LPVOID lparam,CString strBaseInfo);

	static CString GetWebTextHttps(CString strURL);

	//得到文本信息
	static CString GetWebText(CString strURL);
	static BOOL GetWebTxtExt(CString strURL, char* lpBuffer, int nBufLen);

	//测试是否处于联通状态
	static BOOL TestWebConnect(CString strURL);

	//上传文件
	static BOOL UpLoadFile(CString strURL, CString strFile,CString &strResult,CString strID = "",PFN_CallBackUploadFile pfn = NULL,LPVOID lparam = NULL);

	//POST数据
	static BOOL PostData(CString strURL, char* lpData, int nLen, CString &strResult);

	//服务器接口
	static PTHttpInterfaceData CHI_New(ENJXSystemType enType);

	//添加方法名
	static BOOL	CHI_AddFunName(PTHttpInterfaceData pIData, char* lpFunName);
	static BOOL CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, char* lpParam);
	static BOOL CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, int nParam);
	static BOOL CHI_AddOneInItem(PTHttpInterfaceData pIData, char* lpInName, float fParam);
	static BOOL CHI_AddOneOutItem(PTHttpInterfaceData pIData,char* lpOutName);
	static char* CHI_GetOneOutItem(PTHttpInterfaceData pIData,char* lpOutName);
	static int	 CHI_GetOneOutItemInt(PTHttpInterfaceData pIData,char* lpOutName);
	static float CHI_GetOneOutItemFloat(PTHttpInterfaceData pIData,char* lpOutName);
	static BOOL	CHI_ProcIterface(PTHttpInterfaceData pIData);
	static BOOL	CHI_ProcIterfaceEx(PTHttpInterfaceData pIData,CString& strReturnXml,CString strInputXML = "");
	static BOOL CHI_DeleteInterface(PTHttpInterfaceData pIData);
};

#endif // !defined(AFX_HTTPINTERFACE_H__02A1857E_3DB2_4BC4_BA77_752543FE2480__INCLUDED_)

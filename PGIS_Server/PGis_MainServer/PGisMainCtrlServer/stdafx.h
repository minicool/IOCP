// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__61B59C68_23BA_426A_9EAA_DA895A53A9B0__INCLUDED_)
#define AFX_STDAFX_H__61B59C68_23BA_426A_9EAA_DA895A53A9B0__INCLUDED_

#define _TIME			0

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#include <string>

#include "WriteLogThread.h"
#include "TcpInterface.h"
#include "WebService.h"

#include <comutil.h>
#import <msxml6.dll>

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"comsupp.lib")
#pragma comment(lib,"netapi32.lib")

//typedef CEvent AfxmtEvent;
// TODO: reference additional headers your program requires here

//#define __SYS_ENCRYPT_CHECK_   //是否需要加密检测


#ifndef _DEBUG
#define __SERVICE_MODE_
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//
const int MAX_IPADDRESS_LEN = 32;   //IP地址字符串最大长度

///////////////////////////////////////////////////////////////////////////////////////////////
//主控与公网接入服务器数据定义
typedef enum enumGisAcmdType
{    
	A_TYPE_LOGIN = 0x00,     //主控登录
	A_TYPE_LOGIN_ACK = 0x00, //登录应答

	A_TYPE_TICK = 0x02,		//握手协议
	A_TYPE_TICK_ACK = 0x02, //握手应答

	A_TYPE_TERMINAL_LINE = 0x03, //终端公网（GPRS/CDMA）上下线协议 1	GIS<-TER

	A_TYPE_QUERY_MY_LOCATION = 0x04, //终端查询我的位置

	A_TYPE_QUERY_DESTINATION_POSITION = 0x05, //查询地理位置经纬度

//	A_TYPE_SET_TEMPGROUP_ACK = 0x0A, //设置临时编组应答 1		

	A_TYPE_APPCMD_TERMINAL = 0x0A, //下发应用命令给终端协议 1	GIS->TER

	A_TYPE_TERMINAL_GPS = 0x08,  //终端上传GPS协议 1			GIS<-TER

// 	A_TYPE_CTRLCMD_TERMINAL = 0x0C, //下发控制命令协议
// 	A_TYPE_CTRLCMD_TERMINAL_ACK = 0x0C, //终端上传应答控制命令协议
// 
// 	A_TYPE_TERMINAL_ALERT = 0x0D, //终端报警协议
// 	A_TYPE_TERMINAL_ALERT_ACK = 0x0D, //下发应答和解除终端报警协议
// 
// 	A_TYPE_SETCMD_TERMINAL = 0x0E, //下发设置参数给终端协议 1	GIS->TER
// 	A_TYPE_SETCMD_TERMINAL_ACK = 0x0E, //终端应答参数设置协议
// 
// 	A_TYPE_QUERYCFG_TERMINAL = 0x0F, //下发读取参数给终端协议
// 	A_TYPE_QUERYCFG_TERMINAL_ACK = 0x0F, //终端应答参数读取协议

	A_TYPE_TERMINALNEW_GPS = 0x10,
}GIS_A_CMD_TYPE; //


typedef enum enumGisMainSetCmd
{
    SET_CMD_COMMUNICATION_MODEL = 0, //设置当前通讯方式参数命令
    SET_CMD_GPRS_PARAMETER, //设置GPRS通讯参数命令
    SET_CMD_GSM_PARAMETER, //设置GSM通讯参数
    SET_CMD_TERMINAL_NUM ,//设置终端专网号码命令
    SET_CMD_TEMPGROUP_NUM, //设置专网动态重组号码
    SET_CMD_ALERT_NUM ,//设置专网倒放报警号码命令
    SET_CMD_SAFETYCHECK_NUM, //设置专网安检号码命令
    SET_CMD_GPSCENTER_NUM, //设置专网GPS中心号码命令
    SET_CMD_BACK_MODEL ,//设置专网背景扫描模式命令

}GIS_MAIN_SETCMD;

typedef enum enumGisMainQueryCmd
{
    QUERY_CMD_COMMUNICATION_MODEL, //00表示读取当前通讯方式参数命令
    QUERY_CMD_GPS_PARAMETER, //01表示读取GPRS通讯参数命令
    QUERY_CMD_GSM_PARAMETER, //02表示读取GSM通讯参数
    QUERY_CMD_TERMINAL_NUM, //03表示读取终端专网号码命令
    QUERY_CMD_TEMPGROUP_NUM, //04表示读取专网动态重组号码命令
    QUERY_CMD_ALERT_NUM, //05表示读取专网倒放报警号码命令
    QUERY_CMD_SAFETYCHECK_NUM, //06表示读取专网安检号码命令
    QUERY_CMD_GPSCENTER_NUM, //07表示读取专网GPS中心号码命令
    QUERY_CMD_BACK_MODEL, //08表示读取专网背景扫描模式命令
    QUERY_CMD_MAGNETIC_BOUND, //09表示读取专网场强参数命令
    QUERY_CMD_TERMINAL_TYPE, //0A表示读取终端机型参数命令
    QUERY_CMD_WAKEUP_STATE, //0B表示读取专网遥毙遥醒状态命令
    QUERY_CMD_GPSSEND_INTERVAL, //0C表示读取GPS固定上传时间间隔命令

}GIS_MAIN_QUERYCMD;

//
typedef enum enumGisClientAppCmd
{
    c_APPCMD_SetFixGpsDt = 0, //设置固定上传GPS信息时间间隔
    c_APPCMD_GpsTrace = 1, //即时跟踪命令
    c_APPCMD_AreaGpsCall = 2,//区域点名
    c_APPCMD_SendMessage = 3, //
}GISCLIENT_APP_CMD;

typedef enum enumGisClientCtrlCmd
{
	c_CTRL_PRIVATENET_ONLINE, //表示控制专网上线
    c_CTRL_PRIVATENET_OFFLINE,
    c_CTRL_TERMINAL_SLEEP, //表示控制专网遥毙
    c_CTRL_tTERMINAL_WAKEUP, //表示控制专网遥醒
}GISCLIENT_CTRL_CMD;

typedef enum enumReportFailCase
{
    CASE_TERMINAL_OFFLINE = 0,
    CASE_PUBNET_DISCONNECT = 1,   
	CASE_GROUPNUM_ERROR = 2,
    CASE_LOCATION_CONNERROR = 2,
    CASE_WEBSER_CONNERROR = 3,
    CASE_GPSCHECK_CONNERROR = 4

}REPORT_FAIL_CASE; 

//主控与终端管理软件之间协议定义
typedef enum enumGisDcmdType
{

}GIS_D_CMD_TYPE;

///////////////////////////////////////////////////////////////////////////////////////////////
//
const int MAX_DATA_PACKAGE_LEN = 256;  //GIS主控发来的包的最大长度
const int MAIN_DATA_PACKAGE_LEN =  8;  //GIS主控发来的包的最小长度

const int SEND_DATA_PACKAGE_LEN = 512;

const int PACKAGE_HEAD_DATA_STX = 0x02; //0x02 ;  //包头
const int PACKAGE_TAIL_DATA_ETX = 0x03; //0x03 ;  //包尾

#define LINE_ON		1   //上线
#define LINE_OFF	0   //下线
#define PACKAGE_LEN 6 //A类协议包公共部分长度 Len+命令字

const int MAX_RECVICE_BUFFER = 8096;

typedef enum enumTcpConnState
{
    TCP_DISCONNECT, //TCP连接断开
    TCP_LOGIN_WAIT, //等待登录
    TCP_LOGIN_SUCCESS, //登录成功
}TCP_CONN_STATE;

typedef enum enumWebSerCMD
{
    WEB_QUERY_INFO,
    WEB_SET_STATE,
    WEB_WRITE_GPSDDATA,
	WEB_REPT_SENDMSG,
	WEB_UPDATA_LOCATION,
	//WEB_CREATE_TEMPGROUP,
	//WEB_REPT_TEMPGROUP_SETACK,
}WEB_SERVICE_CMD;

#pragma pack(push, 1)

#define WEB_FAIL_AFRESH_YES	0
#define WEB_FAIL_AFRESH_NO  1

#define MAX_LEN_LOCATION_INFO 193
#define MAX_LEN_TEMPGROUPID	32
#define MAX_LEN_GPS_DATA	128
#define MAX_LEN_DEPTNUM		64
#define MAX_LEN_DEVICECODE  51

typedef struct tagTimerTcpTick
{
    int nTcpConnState;
    int nTickPackageCount;
    BOOL m_StartTimerFlag;
    int m_NowTimerVal; //时间记数
    int m_TimerVal;
}TimerTcpTick;

typedef struct tagTerminalInfo
{
	BOOL bQueryInfoOk; //是否查询到了绑定关系
	BOOL bPatrolStart; //是否在进行巡防
	int nQueryFailCount;//查询失败计数
    int nLineState; //上下线状态
	int nUserType; //绑定的用户类型
    CString strNumber; //终端号码
	CString strUserCode; //绑定的用户编码
	CString strDeptNum; //绑定的用户部门
    CString strDevType; //终端类型
}TERMINALInfo;

typedef struct tagRecvClientData
{
    int             nAppID;
    int             nSockID;  //TCP连接ID
	unsigned short	length;
	int				nCmdType;
    //消息内容
	char MessagePacket[MAX_DATA_PACKAGE_LEN];
}RECV_CLIENTDATA;

#define MAX_SEND_SER_DATAT_LEN	128
typedef struct tagSendToServerData
{
	unsigned short	length;
	int				nDataType; //0:gps数据,1:同步数据
    //消息内容
	char MessagePacket[MAX_SEND_SER_DATAT_LEN];
}SEND_TOSERVERDATA;

typedef struct tagDeviceGpsData
{
	unsigned int nQueryIndex;
	char DeviceCode[MAX_LEN_DEVICECODE];
	char LATITUDE[12];
	char LONGITUDE[12];

}DEVICE_GPSDATA;


typedef struct tagRecvServerData
{
    int             nAppID;
	unsigned short	length;
	char Data[MAX_DATA_PACKAGE_LEN];
}RECV_SERVERDATA;


typedef struct tagLocationData
{
	unsigned int nQueryIndex;
	char DeviceCode[MAX_LEN_DEVICECODE];
	char Location[MAX_LEN_LOCATION_INFO];
}ReptLocationData;

typedef struct tagLocationHead
{
	unsigned int nQueryIndex;
	char DeviceCode[MAX_LEN_DEVICECODE];
}ReptLocationDataHead;

typedef struct tagWebReportSendMsgOK
{
	unsigned int nMsgID;
	char TerminalNum[MAX_LEN_DEVICECODE];
}WebReportSendMsgOK;


typedef struct tagWebReportGpsData
{
	int returnIndex;
	char deptNum[MAX_LEN_DEPTNUM];
	char gpsData[MAX_LEN_GPS_DATA];
}WebReportGpsData;

/*******************************************************************************************/
/*								C:GIS
/*******************************************************************************************/
//主控与GIS客户端之间协议定义
typedef enum enumGisCcmdType
{
	C_TYPE_LOGIN = 0x00,     //主控登录	
	C_TYPE_LOGIN_ACK = 0x00, //登录应答

	C_TYPE_TICK = 0x01, //握手协议
	C_TYPE_TICK_ACK = 0x01, //握手应答

	C_TYPE_SYNC_REPORT = 0x02,
	C_TYPE_SYNC_NOTIFY = 0x02,

	C_TYPE_CREATE_TEMPGROUP = 0x03,//设置临时编组
	C_TYPE_REPORT_FAIL = 0x04,//下发终端指令失败协议

	C_TYPE_PRIVATENET_LINE = 0x05, //终端公网（GPRS/CDMA）上下线协议

	C_TYPE_APPCMD_TERMINAL = 0x07, //下发应用命令给终端协议
	C_TYPE_TERMINAL_GPS = 0x08,  //终端上传GPS协议

	C_TYPE_CTRLCMD_TERMINAL = 0x09, //下发控制命令协议
	C_TYPE_CTRLCMD_TERMINAL_ACK = 0x0A, //终端上传应答控制命令协议

	C_TYPE_TERMINAL_ALERT = 0x0B, //终端报警协议
	C_TYPE_TERMINAL_ALERT_ACK = 0x0C, //下发应答和解除终端报警协议

	C_TYPE_GPSANGLY = 0x0D,

	C_TYPE_SUBSCRIPTION_ON = 0x0E,
	C_TYPE_SUBSCRIPTION_OFF = 0x0F,

	C_TYPE_QUERY_MY_LOCATION = 0X06
}GIS_C_CMD_TYPE; //

typedef enum enumSnycAcmdType
{    
	C_SNYC_TERMINLBIND = 0x00,		//终端绑定
	C_SNYC_POLICEINFO = 0x01,		//警员信息
	C_SNYC_CARSINFO = 0x02,			//车辆信息
	C_SNYC_DECICEINFO = 0x03,		//设备信息
	C_SNYC_AREARECT = 0x04,			//巡访区域
	C_SNYC_AREALOAD = 0x05,			//巡访线路
	C_SNYC_SCHEDUGROUPID = 0x06,	//调度组号
	C_SNYC_DEPTINFO = 0x07,			//部门信息
	C_SNYC_ROLEINFO = 0x08,			//角色信息
	C_SNYC_USEINFO = 0x09,			//用户信息
	C_SNYC_NOMERSNYC = 0x0A,		//常规同步	
}C_SNYC_CMD_TYPE; //


/*******************************************************************************************/
/*								E:Location
/*******************************************************************************************/
#define GPS_LOCATION_GIS	0xFF

typedef enum enumLocationAcmdType
{    
	E_TYPE_LOGIN = 0x00,				//登录地理信息服务
	E_TYPE_LOGIN_ACK = 0x00,			//登录应答

	E_TYPE_TICK = 0x01,					//握手协议
	E_TYPE_TICK_ACK = 0x01,				//握手应答

	E_TYPE_LOCATION_EVENT = 0x05,		//位置查询协议	
	E_TYPE_LOCATION_EVENT_ACK = 0x05	//位置回应协议
}E_LOC_CMD_TYPE; //

/*******************************************************************************************/
/*								F:GPSCHECK
/*******************************************************************************************/
typedef enum enumGPSCheckAcmdType
{    
	F_TYPE_LOGIN = 0x00,				//登录巡访报警服务
	F_TYPE_LOGIN_ACK = 0x00,			//登录应答

	F_TYPE_TICK = 0x01,					//握手协议
	F_TYPE_TICK_ACK = 0x01,				//握手应答
	
	F_TYPE_SNYCBROADCAST = 0x02,		//同步信息广播协议	客户端软件（主控服务器）-> 巡访报警服务软件	

	F_TYPE_GPSUPATE_EVENT = 0x05,		//位置上报协议	
	F_TYPE_PATRALERT_EVENT_ACK = 0x05	//位置越区报警提示协议
}F_GPSCHECK_CMD_TYPE; //

/*******************************************************************************************/
/*								WEBSERVER
/*******************************************************************************************/
#define MAX_WEBSERVICE_LISTBUF_LEN	1024*640

/*******************************************************************************************/
/*								山海经纬
/*******************************************************************************************/
typedef struct Shanhai_HeadDate
{
	unsigned short us_Header;    
	unsigned short us_cmd;      
	unsigned short us_version;
	unsigned int un_Len;
}SH_Head;

typedef struct ShanHai_Gps
{	
	unsigned char  GpsId[6];       
	double lon;
	double lat;
	unsigned char speed;
	unsigned char dir;	
	unsigned int  state;
	unsigned short nMsgtype;    
	unsigned short height;
	double time;
}SH_Gps_Old;

typedef struct ShanHai_TGpsDataSS
{	
	unsigned char  GpsId[20];    
	double lon;
	double lat;
	unsigned short speed;
	unsigned short dir;	
	unsigned short height;
	unsigned short nMsgtype;
	unsigned short nYear;
	byte	nMonth;
	byte	nDay;
	byte	nHour;
	byte	nMintue;
	byte	nSecond;
}SH_Gps;

typedef struct ShanHai_GpsData
{	
	SH_Head Head;
	SH_Gps	Gpsdata;
}SH_GpsData;

typedef struct ShanHai_LoginData  
{
	SH_Head Head;
// 	char UserId[10];
// 	char UserPwd[10];
	char UserPwd[50];
}SH_LoginData;

const unsigned short SH_HEAD = 0xAAAA;
const unsigned short SH_VERION = 0x1200;

const unsigned short SH_TYPE_LOGIN = 0xBBBB;
const unsigned short SH_TYPE_LOGIN_ACK = 0xBBCC;
const unsigned short SH_TYPE_GPSDATA = 0xCCCC;
const unsigned short SH_TYPE_TICK = 0xEEEE;
const unsigned short SH_TYPE_TICK_ACK = 0xEEEE;
const unsigned short SH_TYPE_DISCONNET = 0xFFFF;
const unsigned short SH_TYPE_ALERT = 0xDDDD;

#define MIN_SHANHAI_PACKAGE_LEN     2
#define MAX_SHANHAI_PACKAGE_LEN     128

#pragma pack(pop)
///////////////////////////////////////////////////////////////////////////////////////////////
//
#define SOCKET_PUBSERVER_TYPEID      0
#define SOCKET_GISCLIENT_TYPEID         1

#define SOCKET_SERVER_GPSCHECK_TYPEID   2
#define SOCKET_SERVER_LOCATION_TYPEID	3
///////////////////////////////////////////////////////////////////////////////////////////////
//
#define SIMUL_USER_MESSAGE_BASE 300
//写日志
#define WM_WRITE_LOG_MSG                        WM_USER+SIMUL_USER_MESSAGE_BASE + 10
//日志队列满
#define WM_LOGLIST_FULL_MSG						WM_USER+SIMUL_USER_MESSAGE_BASE + 11
//线程停止
#define WM_THREAD_STOP                          WM_USER+SIMUL_USER_MESSAGE_BASE + 20

#define WM_SENDDATA_TO_TCPSERVER_MSG			WM_USER+SIMUL_USER_MESSAGE_BASE + 22
//TCP客户端接收数据
#define WM_RECVICE_CLIENT_MESSAGE               WM_USER+SIMUL_USER_MESSAGE_BASE + 30
//接收到TCP服务端的数据
#define WM_RECVICE_SERVER_MESSAGE				WM_USER+SIMUL_USER_MESSAGE_BASE + 31

//调用WebService
#define WM_WEB_SERVICE_MESSAGE					WM_USER+SIMUL_USER_MESSAGE_BASE + 40
#define WM_WEB_LOADPATROL_MESSAGE				WM_USER+SIMUL_USER_MESSAGE_BASE + 41

#define WM_SERV_CONNERROR_MESSAGE               WM_USER+SIMUL_USER_MESSAGE_BASE + 50

#define WM_TERMINAL_ONLINE_MESSAGE               WM_USER+SIMUL_USER_MESSAGE_BASE + 51

#define WM_FTP_DOWNLOAD_OVER_MESSAGE            WM_USER+SIMUL_USER_MESSAGE_BASE + 36 //FTP下载文件完成

#define WM_RECVICE_GPS_REP_SH_MSG				WM_USER+SIMUL_USER_MESSAGE_BASE + 60

//-------------------------------WANGZH----------------------------------
#define WM_RECVICE_MESSAGE						WM_USER+SIMUL_USER_MESSAGE_BASE + 61
#define WM_RECVICE_CLIENT_MESSAGE_GIS           WM_USER+SIMUL_USER_MESSAGE_BASE + 62
#define WM_RECVICE_CLIENT_MESSAGE_PUB           WM_USER+SIMUL_USER_MESSAGE_BASE + 63
typedef	enum tagEnumProtocol
{
	NULL_PROTOCOL,
	TCP_SERVER,
	TCP_CLIENT,
	UDP_SERVER,
	UDP_CLIENT,
	FTP_PROTOCOL,
}EnumProtocol;

typedef struct tagCMDTypeList
{
	UINT CMDID;
	char *HeadData;
	char *TailData;
}CMDTypeList;

#define SOCKET_PUBNETCLIENT_TYPEID      0
#define SOCKET_GISCLIENT_TYPEID         1
#define SOCKET_SERVER_GPSCHECK_TYPEID   2
#define SOCKET_SERVER_LOCATION_TYPEID	3

static CMDTypeList Cmdtypelist[]={
	{SOCKET_PUBNETCLIENT_TYPEID,"*HITOM","#"},
	{SOCKET_GISCLIENT_TYPEID,"*HYT","#"},
	{SOCKET_SERVER_GPSCHECK_TYPEID,"",""},
	{SOCKET_SERVER_LOCATION_TYPEID,"",""},
};

typedef enum enumSocketInfo
{
	TCP_CLIENT_DISCONNECT = 0,
	TCP_CLIENT_CONNECTED,
	TCP_SERVER_DISCONNECT
}SOCKET_INFO;
//--------------------------------END------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__61B59C68_23BA_426A_9EAA_DA895A53A9B0__INCLUDED_)

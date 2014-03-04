/*
**	FILENAME			tcpinterface.h
**
*/
#ifndef __TCPINTERFACE_H_
#define __TCPINTERFACE_H_

const int TCPSERVER_STARTSUCCESS = 0;
const int TCPSERVER_CLOSECLIENTSUCCESS = 0;

const int TCPCLIENT_CONNECTSUCCESS = 1;
const int TCPCLIENT_CREATERECVTHREADSUCCESS = 1;
const int TCPCLIENT_SENDSUCCESS = 1;

typedef enum enumTcpServerEvent
{
	TCPSERVER_EVENT_CLIENTJOIN,	   //客户端接入
	TCPSERVER_EVENT_CLIENTEXIT,	   //客户端退出
	TCPSERVER_EVENT_RECVDATA	   //收到客户端数据	
}TCPSERVER_EVENT;

typedef enum enumTcpClientEvent
{
	TCPCLIENT_EVENT_RECVDATA = 0x0A,//接收到数据
	TCPCLIENT_EVENT_DISCONNECT//与服务器连接断开
}TCPCLIENT_EVENT;

typedef enum enumTcpServerError
{
	TCPSERVER_ERROR_PORTEXCEEDRANGE = -1, //端口超出范围[0,65535]
	TCPSERVER_ERROR_NOSOCKDLL = -2,		  //找不到Sock动态库,一般为操作系统问题
	TCPSERVER_ERROR_NORIGHTSOCKDLL = -3,  //找不到正确的Sock动态库,一般为操作系统问题
	TCPSERVER_ERROR_NONBLOCKING = -4,	  //设置成非阻塞模式失败,一般为操作系统问题	
	TCPSERVER_ERROR_CREATELISTNESOCK = -5,//创建Listen Socket时失败
	TCPSERVER_ERROR_BIND = -6,			  //bind 端口失败
	TCPSERVER_ERROR_LISTEN = -7,			  //监听失败
	TCPSERVER_ERROR_CREATEACCEPTTHREADFAIL = -8,	//Accept线程创建失败
	TCPSERVER_ERROR_CREATERECVTHREADFAIL = -9,//recv 线程创建失败
	TCPSERVER_ERROR_NOTRIGHTSOCKID = -10,	//传入的Sock ID错误，超出范围或该ID不能用客户已经退出
	TCPSERVER_ERROR_SENDDATA = -11,			//发送数据时，传入的数据错误，数据指针为空或数据长度<=0	
	TCPSERVER_ERROR_SEND = -12,				//数据发送错误
    TCPSERVER_INIT_ERROR = -13,
}TCPSERVER_ERROR;

typedef enum enumTcpClientError
{
	TCPCLIENT_ERROR_PORTEXCEEDRANGE = -1, //端口超出范围[0,65535]
	TCPCLIENT_ERROR_NOSOCKDLL = -2,		  //找不到Sock动态库,一般为操作系统问题
	TCPCLIENT_ERROR_NORIGHTSOCKDLL = -3,  //找不到正确的Sock动态库,一般为操作系统问题
	TCPCLIENT_ERROR_NONBLOCKING = -4,	  //设置成非阻塞模式失败,一般为操作系统问题	
	TCPCLIENT_ERROR_CREATESOCK = -5,//创建Socket时失败
	TCPCLIENT_ERROR_CONNECT = -6,		//Connect 主机时错误,服务器未开或网络不通
	TCPCLIENT_ERROR_CREATERECVTHREADFAIL = -7,//创建接收线程失败
	TCPCLIENT_ERROR_BLOCKING = -8,//设置成阻塞模式失败,一般为操作系统问题
	TCPCLIENT_ERROR_TIMEOUT = -9,//Connect时,超时设置错误，范围为(0,30]
	TCPCLIENT_ERROR_SENDDATA = -11,			//发送数据时，传入的数据错误，数据指针为空或数据长度<=0	
	TCPCLIENT_ERROR_SEND = -12,				//数据发送错误
	TCPCLIENT_ERROR_NOTRIGHTSOCK = -13//发送的Socket不能用,一般为未正常连接服务器
}TCPCLIENT_ERROR;

typedef struct tagTcpConnect
{
	char *pszRecvData;//只有在事件为TCPCLIENT_EVENT_RECVDATA时有效，否则为NULL
	int nRecvDataLen;//只有在事件为TCPCLIENT_EVENT_RECVDATA时有效，否则为0
}TcpConnect;

typedef struct tagTcpServerClient
{
	char szIP[32];//Ip地址，如:192.168.21.122
    int nAppID;
	int nID;//客户ID，当有客户端接入时需保存该信息，在发送数据时使用
	int nPort;//客户使用端口
	char *pszRecvData;//只有在事件为TCPSERVER_EVENT_RECVDATA时有效，否则为NULL
	int nRecvDataLen;//只有在事件为TCPSERVER_EVENT_RECVDATA时有效，否则为0
}TcpServerClient;

//WANGZH_ALL
const int MAX_DATA_PACKAGE_LEN = 256;  //GIS主控发来的包的最大长度
const int MAIN_DATA_PACKAGE_LEN =  8;  //GIS主控发来的包的最小长度

const int PACKAGE_HEAD_DATA_STX = 0x02; //0x02 ;  //包头
const int PACKAGE_TAIL_DATA_ETX = 0x03; //0x03 ;  //包尾
const int MAX_RECVICE_BUFFER = 1024;
const int PACKAGE_PUB_LEN = 6;

typedef	enum tagEnumProtocol
{
	NULL_PROTOCOL,
	UDP_PROTOCOL,
	FTP_PROTOCOL
}EnumProtocol;

typedef struct tagSocketInfo
{
	int nAppID;
	int nSocketID;
	int nPort;
	char szIP[32];
	int nRecvDataLen;
	char *pszRecvData;
}SocketInfo,*pSocketInfo;

typedef struct tagRecvClientData
{
	int             nAppID;
	int             nSockID;  //TCP连接ID
	unsigned short	length;
	int				nCmdType;
	char MessagePacket[MAX_DATA_PACKAGE_LEN];
}RECV_CLIENTDATA;

#define WM_RECVICE_CLIENT_MESSAGE               WM_USER+SIMUL_USER_MESSAGE_BASE + 30

typedef enum enumAcmdType
{
	TYPE_LOGIN = 0x00,				
	TYPE_LOGIN_ACK = 0x00,

	TYPE_LOGINOUT = 0x01,
	TYPE_LOGINOUT_ACK = 0x01,

	TYPE_TICK = 0x02,		
	TYPE_TICK_ACK = 0x02,
}CMD_TYPE;

typedef enum enumSocketInfo
{
	TCP_CLIENT_DISCONNECT = 0x0F,
	TCP_CLIENT_CONNECTED,
	TCP_SERVER_DISCONNECT
}SOCKET_INFO;

typedef struct tagCMDTypeList
{
	UINT CMDID;
	char *HeadData;
	char *TailData;
}CMDTypeList;

#define SOCKET_PUBNETCLIENT_TYPEID      0
#define SOCKET_NEWCLIENT_TYPEDID		1 
#define SOCKET_OLDCLIENT_TYPEDID		2

static CMDTypeList Cmdtypelist[]={
	{SOCKET_PUBNETCLIENT_TYPEID,"*HITOM","#"},
	{SOCKET_NEWCLIENT_TYPEDID,"*HITOM","#"},
	{SOCKET_OLDCLIENT_TYPEDID,"*HYT,","#"},
};

///////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_RECVICE_BUFFER          /*1024+256*/4096
#define SOCKET_SERVER_PUBLICNET_ID     0
#define SOCKET_CLIENT_GISMAIN_ID       1


#define MAIN_PUBLIC_PACKAGE_LEN         4
#define MAX_PUBLIC_PACKAGE_LEN          256

#define MAX_PUBLICNET_DATE_TYPE         20

///////////////////////////////////////////////////////////////////////////////////////////

typedef enum enumTimerType
{
	TCP_CONN_FAIL,
	TCP_CONN_SUCCESS,
	TCP_WAIT_LOGIN_ACK,
	TCP_LOGIN_SUCCESS,
	TCP_WAIT_LOGIN_MSG, //tcp连接成功后等待收GPRS连接登录消息定时
	WAIT_CONN_TICK,
}TCPCONN_TYPE;

//GPS数据类型
typedef enum enumGpsDataType
{
	GPS_DATA_PRIVATENET_LINEOFF,//00表示专网下线GPS
	GPS_DATA_PRIVATENET_LINEON,//01表示专网上线GPS
	GPS_DATA_TINMEROUT,//02表示固定上传GPS
	GPS_DATA_MONITOR,//03表示即时跟踪GPS
	GPS_DATA_BOUNDCALL,//04表示区域点名GPS
	GPS_DATA_MY_POSITION,//05表示请求"我的位置"的GPS
	GPS_DATA_MY_OFFDUTY,//06表示下班GPS
	GPS_DATA_MY_ONDUTY,//07表示上班GPS
}GPS_DATA_TYPE;

//上行协议命令字定义
typedef enum enumPubProtocalUp
{
	GPS_TINMEROUT_REPORT = 0x00,            //固定时间间隔主动上传GPS信息-00
	GPS_MONITOR_RESPONSE = 0x01,            //应答即时跟踪GPS协议-01
	GPS_BOUNDCALL_RESPONSE = 0x02,          //应答区域点名GPS协议-02
	SEND_MESSAGE_ANSWER = 0x03,             //文字信息回复信息

	TERMINAL_LOGIN_REPORT = 0x10,           //GPRS连接登录协议-10
	TERMINAL_OPEN_REPORT = 0x11,            //开机信息协议-11 response
	GPS_TERMINAL_OPEN_REPORT = 0x12,        //开机GPS信息协议-12
	TCPCONNECT_TICK_REPORT = 0x13,          //握手信息信息协议-13
	PRIVATENET_LINEON_REPORT = 0x14,        //专网上线协议-14
	PRIVATENET_LINEOFF_REPORT = 0x15,       //专网下线协议-15
	QUERY_MY_POSITION =0x16,                //上传请求查询我的位置信息协议-16
	QUERY_DESTINATION_POSITION = 0x17,      //上传请求查询目的地经纬度协议-17
	CTL_TERMINAL_MOUNTGUARD_RESPONSE = 0x18,//应答控制终端上班下班-18
	GPS_HISTORY_REPORT = 0x19,              //历史GPS数据

	CTL_PRIVATENET_LINE_RESPONSE = 0x20,    //应答控制专网上线和专网下线命令-20
	CTL_TERMINAL_WAKEUP_RESPONSE = 0x21,    //应答控制终端集群遥毙和摇醒-21

	GPS_ALERTING_REPORT = 0x30,             //报警协议-30

	SET_COMMUNICATION_MODEL_RESPONSE = 0x60,//应答设置通讯方式参数-60
	SET_GPRS_PARAMETER_RESPONSE = 0x61,		//应答设置GPRS参数-61
	SET_GPS_PARAMETER_RESPONSE = 0x62,		//应答设置GSM参数-62
	SET_TERMINAL_NUM_RESPONSE = 0x63,		//应答设置终端专网号码-63
	SET_TEMP_NUM_RESPONSE = 0x64,           //应答设置动态临时编组号码-64
	SET_ALERT_NUM_RESPONSE = 0x65,          //应答设置专网倒放告警号码-65
	SET_SAFETYCHECK_NUM_RESPONSE = 0x66,    //应答设置专网安检号码-66
	SET_GPSCENTER_NUM_RESPONSE = 0x67,      //应答设置专网GPS中心号码-67
	SET_BACK_MODEL_RESPONSE = 0x68,         //应答设置专网背景模式-68

	QUERY_COMMUNICATION_MODEL_RESPONSE=0x80,//应答读取通讯方式参数-80
	QUERY_GPS_PARAMETER_RESPONSE = 0x81,	//应答读取GPRS参数-81
	QUERY_GSM_PARAMETER_RESPONSE = 0x82,	//应答读取GSM参数-82
	QUERY_TERMINAL_NUM_RESPONSE = 0x83,		//应答读取终端专网号码-83
	QUERY_TEMP_NUM_RESPONSE = 0x84,         //应答读取专网动态重组号码-84
	QUERY_ALERT_NUM_RESPONSE = 0x85,        //应答读取专网倒放告警号码-85
	QUERY_SAFETYCHECK_NUM_RESPONSE = 0x86,  //应答读取专网安检号码-86
	QUERY_GPSCENTER_NUM_RESPONSE= 0x87,     //应答读取专网GPS中心号码-87
	QUERY_BACK_MODEL_RESPONSE = 0x88,		//应答读取专网背景扫描模式-88
	QUERY_MAGNETIC_BOUND_RESPONSE = 0x89,	//应答读取专网场强参数-89
	QUERY_TERMINAL_TYPE_RESPONSE = 0x8A,    //应答读取终端机型参数-8A
	QUERY_WAKEUP_RESPONSE = 0x8B,           //应答读取终端遥毙摇醒状态-8B

	QUERY_GPS_SEND_TINMEROUT = 0x8C,        //应答读取终端固定上传GPS信息时间间隔-8C

	MAX_PROTOCOL_UP,

}PUBLIC_PROTOCAL_UP;

//下行协议
typedef enum  enumPubProtocalDown
{
	SET_GPS_TINMEROUT = 0,              //设置固定上传GPS时间间隔命令-00
	SET_GPS_MONITOR = 0x01,             //中心下发即时跟踪指令-01
	SET_GPS_BOUNDCALL = 0x02,           //区域点名信息-02
	APP_SEND_MESSAGE = 0x03,            //下发文字信息-03

	ACK_TERMINAL_LOGIN = 0x10,          //应答终端GPRS登录信息-10
	ACK_TERMINAL_OPEN = 0x11,           //应答终端开机信息-11
	ACK_GPS_TERMINAL_OPEN = 0x12,       //应答终端开机GPS信息-12
	ACK_TCPCONNECT_TICK = 0x13,         //应答终端握手信息-13
	ACK_PRIVATENET_LINEON = 0x14,       //应答专网上线-14
	ACK_PRIVATENET_LINEOFF = 0x15,      //应答专网下线-15 
	ACK_TERMINAL_MOUNTGUARD = 0x18,     //应答“上、下班”信息-18

	CTL_PRIVATENET_LINE = 0x20,         //控制专网上线和专网下线命令-20
	CTL_TERMINAL_WAKEUP = 0x21,         //控制终端集群遥毙和摇醒-21

	ACK_GPS_ALERTING = 0x30,            //应答报警及解除报警-30

	CFG_COMMUNICATION_MODEL_CMD = 0x60, //设置通讯方式参数-60
	CFG_GPRS_PARAMETER_CMD = 0x61,      //设置GPRS参数-61
	CFG_GSM_PARAMETER_CMD = 0x62,       //设置GSM参数-62
	CFG_TERMINAL_NUM_CMD = 0x63,        //设置终端专网号码-63
	CFG_TEMPGROUP_NUM_CMD = 0x64,       //设置动态临时编组号码-64
	CFG_ALERT_NUM_CMD = 0x65,           //设置专网倒放告警号码-65
	CFG_SAFETYCHECK_NUM_CMD = 0x66,     //设置专网安检号码-66
	CFG_GPSCENTER_NUM_CMD = 0x67,       //设置专网GPS中心号码-67
	CFG_BACK_MODEL_CMD = 0x68,          //设置专网背景模式-68

	QUERY_COMMUNICATION_MODEL_CMD=0x80, //读取通讯方式参数-80
	QUERY_GPS_PARAMETER_CMD = 0x81,     //读取GPRS参数-81
	QUERY_GSM_PARAMETER_CMD = 0x82,     //读取GSM参数-82
	QUERY_TERMINAL_NUM_CMD = 0x83,      //读取终端专网号码-83
	QUERY_TEMPGROUP_NUM_CMD = 0x84,     //读取专网动态重组号码-84
	QUERY_ALERT_NUM_CMD = 0x85,         //读取专网倒放告警号码-85
	QUERY_SAFETYCHECK_NUM_CMD = 0x86,   //读取专网安检号码-86
	QUERY_GPSCENTER_NUM_CMD= 0x87,      //读取专网GPS中心号码-87
	QUERY_BACK_MODEL_CMD = 0x88,        //读取专网背景扫描模式-88
	QUERY_MAGNETIC_BOUND_CMD = 0x89,    //读取专网场强参数-89
	QUERY_TERMINAL_TYPE_CMD = 0x8A,     //读取终端机型参数-8A
	QUERY_WAKEUP_CMD = 0x8B,            //读取终端遥毙摇醒状态-8B
	QUERY_GPSSEND_INTERVAL_CMD = 0x8C,  //读取GPS固定上传时间间隔参数

	MAX_PUB_PROTOCAL_DOWN,

}PUBLIC_PROTOCAL_DOWN;

//终端型号
typedef enum enumTermainalKind
{
	KIND_UNKNOWN = 0,
	KIND_TM800 = 1,
	KIND_TC890GM =2,
}TERMAINAL_KIND;

//开启GPS模式
typedef enum enumOpenGpsModel
{
	OPEN_GPS_NOTCTRL, //服务程序不控制
	OPEN_GPS_GPRSLOGIN,
	OPEN_GRPS_SOON, //GPS立即上传
}OPEN_GPS_MODEL;

///////////////////////////////////////////////////////////////////////////////////////////
//
typedef enum enumGisAcmdType
{
	A_TYPE_LOGIN = 0x00,     //主控登录
	A_TYPE_LOGIN_ACK = 0x00, //登录应答

	A_TYPE_TICK = 0x02,		//握手协议
	A_TYPE_TICK_ACK = 0x02, //握手应答

	A_TYPE_TERMINAL_LINE = 0x03, //终端公网（GPRS/CDMA）上下线协议 1	GIS<-TER

	A_TYPE_QUERY_MY_LOCATION = 0x04, //终端查询我的位置

	A_TYPE_QUERY_DESTINATION_POSITION = 0x05, //查询地理位置经纬度

	//A_TYPE_SET_TEMPGROUP_ACK = 0x0A, //设置临时编组应答 1		

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
}GIS_A_CMD_TYPE; 

typedef enum enumCtrlAckType
{
	aCTRL_ACK_PRIVATENET_ONLINE, //表示应答控制专网上线
	aCTRL_ACK_PRIVATENET_OFFLINE,
	aCTRL_ACK_SLEEP, //表示应答控制专网遥毙
	aCTRL_ACK_WAKEUP, //表示应答控制专网遥醒
}CTRL_ACK_TYPE;

typedef enum enumGisMainSetCmd
{
	aSET_CMD_COMMUNICATION_MODEL = 0, //设置当前通讯方式参数命令
	aSET_CMD_GPRS_PARAMETER, //设置GPRS通讯参数命令
	aSET_CMD_GSM_PARAMETER, //设置GSM通讯参数
	aSET_CMD_TERMINAL_NUM ,//设置终端专网号码命令
	aSET_CMD_TEMPGROUP_NUM, //设置动态编组号码
	aSET_CMD_ALERT_NUM ,//设置专网倒放报警号码命令
	aSET_CMD_SAFETYCHECK_NUM, //设置专网安检号码命令
	aSET_CMD_GPSCENTER_NUM, //设置专网GPS中心号码命令
	aSET_CMD_BACK_MODEL ,//设置专网背景扫描模式命令
}
GIS_MAIN_SETCMD;
///////////////////////////////////////////////////////////////////////////////////////////
#endif
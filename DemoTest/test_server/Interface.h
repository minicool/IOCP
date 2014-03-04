#define MAX_RECVICE_BUFFER          1024+256
typedef enum enumCMDTypeID
{
	SOCKET_PUBNETCLIENT_TYPEID = 0,
	SOCKET_GISCLIENT_TYPEID,
	SOCKET_SERVER_GPSCHECK_TYPEID,
	SOCKET_SERVER_LOCATION_TYPEID,
	SOCKET_H6CLIENT_TYPEDID,
}EnumCMDTypeID;

typedef struct tagCMDTypeList
{
	UINT CMDID;
	char *HeadData;
	char *TailData;
}CMDTypeList;

static CMDTypeList m_Cmdtypelist[]={
	{SOCKET_PUBNETCLIENT_TYPEID,"*HITOM","#"},
	{SOCKET_GISCLIENT_TYPEID,"*HYT","#"},
	{SOCKET_SERVER_GPSCHECK_TYPEID,"",""},
	{SOCKET_SERVER_LOCATION_TYPEID,"",""},
	{SOCKET_H6CLIENT_TYPEDID,"*LBS,","#"},
};

typedef enum enumH6AcmdType
{
	H6_TYPE_TERMINAL_GPS = 0x21,		//上传GPS协议		【终端】

	H6_TYPE_LOGIN = 0x2A,				//登录				【终端】
	H6_TYPE_LOGIN_ACK = 0x2A,			//登录应答			【服务端】

	H6_TYPE_TICK = 0x2D,				//握手				【终端】
	H6_TYPE_TICK_ACK = 0x2D,			//握手应答			【服务端】

	H6_TYPE_STATE = 0x30,				//终端信息			【服务端】
	H6_TYPE_STATE_ACK = 0x30,			//信息应答			【终端】

	H6_TYPE_TERMINAL_ALERT = 0x3E,		//终端报警			【终端】
	H6_TYPE_TERMINAL_ALERT_ACK = 0x3E	//解除终端报警		【服务端】
}H6_CMD_TYPE; //
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
	int nAppID;
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

typedef enum enumGISTOTER
{
	OLD_GISTOTER = 0,
	NEW_GISTOTER,
}GisToTer;

typedef enum enumTermainalKind
{
	KIND_UNKNOWN = 0,
	KIND_TM800 = 1,
	KIND_TC890GM =2,
	KIND_H5 =3,
	KIND_H6 =4,
}TERMAINAL_KIND;

typedef enum enumSendDataType
{
	WEBSERVER_TYPE =0,
	DATASERVER_TYPE,
	LOCALSTATION_TYPE,
	SHANHAI_TYPE,
	GPSCHECK_TYPE,
}SENDDATA_TYPE;
#endif
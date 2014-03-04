#ifndef __HYT_PUBNET_CONTROL_H_INCLUDED_
#define __HYT_PUBNET_CONTROL_H_INCLUDED_

#include "PubTerminalManage.h"
#include "CompletionRoutineServer.h"
#include "FTPConveyFile.h"
///////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_RECVICE_BUFFER          1024+256
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
    A_TYPE_LOGIN = 0,     //主控登录
    A_TYPE_LOGIN_ACK = 0, //登录应答
    A_TYPE_TICK = 1, //握手协议
    A_TYPE_TICK_ACK = 1, //握手应答
    
    A_TYPE_SET_TEMPGROUP_ACK = 0x03, //设置临时编组应答
    
    A_TYPE_TERMINAL_LINE = 0x05, //终端公网（GPRS/CDMA）上下线协议
    A_TYPE_TERMINAL_TICK = 0x06, //终端握手协议
    
    A_TYPE_APPCMD_TERMINAL = 0x07, //下发应用命令给终端协议
    A_TYPE_TERMINAL_GPS = 0x08,  //终端上传GPS协议
    A_TYPE_CTRLCMD_TERMINAL = 0x09, //下发控制命令协议
    A_TYPE_CTRLCMD_TERMINAL_ACK = 0x0A, //终端上传应答控制命令协议
    A_TYPE_TERMINAL_ALERT = 0x0B, //终端报警协议
    A_TYPE_TERMINAL_ALERT_ACK = 0x0C, //下发应答和解除终端报警协议
    
    A_TYPE_SETCMD_TERMINAL = 0x0D, //下发设置参数给终端协议
    A_TYPE_SETCMD_TERMINAL_ACK = 0x0E, //终端应答参数设置协议
    A_TYPE_QUERYCFG_TERMINAL = 0x0F, //下发读取参数给终端协议
    A_TYPE_QUERYCFG_TERMINAL_ACK = 0x10, //终端应答参数读取协议
    
    A_TYPE_QUERY_MY_LOCATION = 0x11, //终端查询我的位置
    A_TYPE_QUERY_DESTINATION_POSITION = 0x12, //查询地理位置经纬度
        
}GIS_A_CMD_TYPE; //

///////////////////////////////////////////////////////////////////////////////////////////
//
class CPubNetControl
{
public:
    CPubNetControl();
    ~CPubNetControl();

    BOOL StartHYTPubNetSystem();
    BOOL StopHYTPubNetSystem();

private:
    BOOL CheckIPAddressFormat(const char *strIPAddress);
    int GetChksum(const char *strMsg);
    BOOL GetCpsxIdValue(const char *strValue, char *strOut);
    CString GetCpsxFltId(const char *strValue);
    BOOL GetCpsxFltId(const char *strValue, char *strOut);
private:
    void InitMyVersion();
    void InitFieldStatisticTable();
    
    BOOL LoadSystemConfig();
    void SetLog();
    
    BOOL StartThread();
    void StopThread();
    BOOL StartPublicTcpServer();
    void StopPublicTcpServer();
    
protected:
    BOOL SendDataToPubNetTerminal(int nClientID,int nType,const char *DataBuf,int nBufLen);
    int GetClientID(const char *strDeviceCode);

private:
    static DWORD WINAPI TcpServerPublicCallBack(UINT uMsg,DWORD dwParam,DWORD dwInstance);
    void TcpClientConnected(int typeID,int nClientID = -1,const char *strIP = NULL,int nPort = -1);
    void TcpClientDisconnect(int typeID,int nClientID = -1,const char *strIP = NULL,int nPort = -1,int nExitCode = -1);
    void PublicNetDataAnalyse(int nClientID,const char *recvData, int nRecvLen);

protected:
    static UINT WINAPI DataDispatchThreadProc(void *lpParameter); //Dispatch
    //设置固定上传时间间隔
    void OnSetTerminalGpsIntervalMessage(WPARAM wParam, LPARAM lparam);
    //设计及时跟踪
    void OnSetTerminalGpsMonitorMessage(WPARAM wParam, LPARAM lparam);
    //将ACK数据发给终端
    BOOL OnAnswerAckToTerminal(WPARAM wParam, LPARAM lparam);

    BOOL OnDispatchDataMessage();

    //检测是否有需要重发的文字消息
    void OnCheckSendMessageWaitAnswerTimerOut();

    //重发文字消息
    BOOL ResendMessgToPubDevice(int nPubDeviceID,const char *pcSendMsgInfo,int nAckVaule);

    void OnCheckGetSendMsgTxtTimerOut();

public:
    BOOL OnReadMainFtpConveyFile();  //ftp文件下载完成
protected:
    static UINT WINAPI DataAnalyseThreadProc(void *lpParameter); //
    void OnCheckTerminalTickOverTimer(); //收终端登录消息定时器,每隔1秒检测一次
    BOOL OnAnalysePublicServerRecvData(); //分析接收公网接入的数据包
    //BOOL OnReadMainFtpConveyFile();  //ftp文件下载完成

    //处理GPRS登录
    int TerminalLoginProccess( int nClientID,  
            const char *strDeviceCode, //SIM卡号
            const char *strTrunkNum, //集群ID
            const char *strModel, //本机型号
            BOOL isOldProtocol = FALSE //是否为老协议版本
        );
    //处理GPS数据
    BOOL PublicGpsDataProcess(GPS_DATA_TYPE gpsDataType,
        int nClientID,
        const char *SIMNum, //SIM卡号
        const char * strTimer, //时间
        const char *nsFlag,   //N/S
        const char *latitude, //纬度
        const char *ewFlag,  //E/W
        const char *longitude, //经度
        const char *avFlag,  //是否有效
        const char *speed,   //速度
        const char *Direction,   //方向
        const char *Date,   //日期;
        const char *state,
        const char *strAllData
    );
    void DispatchAckToTerminal(int nClientID,int AckType,const char *AckValue);
    //设置设置定时上传时间
    void DispatchSetFixGpsDtCmdToTerminal(int nClientID);
    void SendMessageAnswerOkProcess(int nPubDeviceID,const char *pcDeviceNum,int nAckValue);

    BOOL ReportDataToGisMain(int nType,const char *DataBuf,int nBufLen);
    //
   
    BOOL FtpMainDataAnalyse(const char *pstrMainDataPacket);
    //读文件读到主控启动成功消息处理,将所有的在线终端写文件
    void MainStartSuccessResponse();

private:
    //
    BOOL m_bStartFlag; //系统是否启动
    CPubTerminalManage m_tbPubTerminalInfo[MAX_PUBLICNET_CLIENT_CONNECT];

    CCompletionRoutineServer m_SockPubNetServer;

    CCriticalSection m_csDispatchList;
    CList<DISPATCH_MESSAGE, DISPATCH_MESSAGE&> m_DispatchList;

    int m_nAckCount[MAX_PUB_PROTOCAL_DOWN];

private:
    unsigned char m_FieldStatisticTB[MAX_PROTOCOL_UP];

    int m_nOpenGpsModel;
    int m_nInitFixGpsDt;
    
    int m_nMonitorGPSCount;

    int m_nPubNetServerPort;
    char m_LocalIpAddress[32];

    int m_nLoginTimeOut;
    int m_nConnTickTimeOut;
    BOOL m_bDispatchZeroGPS;
    int m_nL;
    int m_nM;
    int m_nS;
    
    int m_nSendTxtMsgWaitAckTimeOut;
private:
    BOOL m_bThreadStart;
    
    UINT m_dwDataAnalyseThreadID;
    HANDLE m_hDataAnalyseThread;
    HANDLE m_hDataAnalyseThreadEndEvt;
    
    //DataDispatch
    UINT m_dwDataDispatchThreadID;
    HANDLE m_hDataDispatchThread;
    HANDLE m_hDataDispatchThreadEndEvt;

    CFTPConveyFile m_myFtpConveyFile;
    char m_strFTPDownLoadFileName[MAX_PATH];
    
//
protected:

};
#endif
#if !defined __HEAD_PUB_TERMINAL_MANAGE_H_
#define __HEAD_PUB_TERMINAL_MANAGE_H_


///////////////////////////////////////////////////////////////////////////////////////////////

//最大允许TCP连接的终端个数
const int MAX_PUBLICNET_CLIENT_CONNECT = 4096;
///////////////////////////////////////////////////////////////////////////////////////////////
//
const int MAX_DEVICE_CODE_LEN = 64; //SIM卡号码最大长度
const int MAX_IPADDRESS_LEN = 32;   //IP地址字符串最大长度
const int TRUNK_NUMBER_LEN = 10;    //集群号码最大长度

///////////////////////////////////////////////////////////////////////////////////////////////
//

typedef enum enumConnectState
{
    CONNECT_NULL, 
    CONNECT_WAIT_GPRSLOGIN, //连接成功，等待GPRS登录
    CONNECT_GPRSLOGIN_OK, //GPRS登录成功

}CONNECT_STATE;


//记录下发文字信息表
typedef struct tagSendMsgWaitAckInfo
{
    BOOL bStart;
    int nAckValue;
    int nTimerVal;
    int nTimeOutValue;
    int nSendMsgID;
    int nSendCount;
    char MessageInfo[256];
}SendMsgWaitAckInfo;

typedef struct tagMessageTxtInfo
{
    int nMsgID;
    int nAckValue;
    char TxtInfo[256];
}MessageTxtInfo;
///////////////////////////////////////////////////////////////////////////////////////////////
//
class CPubTerminalManage
{
public:
    CPubTerminalManage();
    ~CPubTerminalManage();

    void SetConnectState(CONNECT_STATE connectState,int nTimeOutValue = 0);

    void DataParseProcess(int nSockID,const  char * recvData ,int nRecvLen); //chang to unsigned char

    int CheckTimerOut();

	char *GetDeptNum();
	void SetDeptNum(const char *csDeviceNum);
    char *GetDeviceNum();
    void SetDeviceNum(const char *csDeviceNum);
    char *GetDeviceKind();
    void SetDeviceKind(const char *csDeviceType);

    int GetConnectState();
    void ResetTerminalInfo();

    BOOL StartSendMessageWaitAnswer(int nAckValue,int nSendMsgID,const char *pcSendMsgInfo,int nTimeOutValue);
    void StopSendMessageWaitAnswer();
    BOOL SendMessageAnswerOkCheck(int nAckValue);

    //文字消息重发
    int GetSendMsgID();
    const char * GetMessageInfo();
    int GetMsgAckValue();
    int CheckResendMsgInfoTimerOut();

    void AddMsgTxtToBuffer(int nMsgID, const char * pcsMsgTxt);
    BOOL GetMsgTxtTimerCheck(char * pcsMsgTxtBuf,int &nMsgID);

    int CheckResendTempGroupSetTimerout();
    BOOL StartGroupSetWaitAnswer(int nAckValue,const char *pcGroupNumber,BOOL bSetFlag,int nTimeOutValue);
    void StopGroupSetWaitAnswer();

public:
    
    unsigned char m_nConnectState; 

    char m_csDeviceCode[MAX_DEVICE_CODE_LEN]; //SIM卡号
    char m_csDeviceType[MAX_DEVICE_CODE_LEN];  //终端种类
    char m_csDeptCode[MAX_DEVICE_CODE_LEN]; //部门卡号

    BOOL m_bOldProtocol; //是否为老协议

public:
    unsigned short m_nNowTimerVal; //时间记数
    unsigned short m_nTimeOutValue; //超时时间

public:
    SendMsgWaitAckInfo m_SendMsgWaitAck;

    CList<MessageTxtInfo,MessageTxtInfo&> m_MsgBufferList;
    int m_nSendMsgTxtTimeVal ;

    char *m_pRecvBuf; //change to unsigned char
    int m_nRecvLen;

    //动态编组重发相关参数
    BOOL m_bSendGroupSetWaitAckStart;
    int m_nGroupSetTimerVal;
    int m_nGroupSetTimeOutValue;
    int m_nGroupSetSendCount; //设置动态组重复计数
    
    int m_nGroupSetAckValue;
    char m_csGroupSetNum[16];
    BOOL m_bGroupSetFlag; //创建还是取消标志

};

#endif
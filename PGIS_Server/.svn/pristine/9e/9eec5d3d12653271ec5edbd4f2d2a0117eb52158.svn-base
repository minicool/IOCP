#include "StdAfx.h"
#include "Encrypt.h"

#include"iphlpapi.h"
#include <nb30.h>


#pragma comment(lib,"Iphlpapi.lib")
/////////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _ASTAT_{
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff[ 30 ];
}ASTAT,*PASTAT;

ASTAT Adapter;


//常量 作为密钥来使用
BYTE KEY[]={0x01,0x2D,0x6C,0x6A};

//进行加密的字符串,呵呵 随机拼凑的
static char EncryptStrTable[64]={'S','1','n','4','3','M','6','K',
                                '8','7','*','#','A','f','J','D',
                                'i','F','U','H','p','C','9','L',
                                'a','N','O','c','Q','R','&','T',
                                'G','V','W','7','Y','c','5','b',
                                'P','d','e','B','g','h','E','j',
                                'k','l','m','2','o','I','8','r',
                                '5','t','D','3','w','4','1','z'};

//随机拼凑的
static char EncryptLimitTB[10] = {'W','K','A','L','O','S','Y','G','V','J'};

#define MAX_CLEAR_TXT_LEN 16
/////////////////////////////////////////////////////////////////////////////////////////////
//
CEncrypt::CEncrypt()
{
	m_LocalMacList.RemoveAll();

	GetMacAddressList();
}

CEncrypt::~CEncrypt()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//获取主机的所有MAC,保存在LocalMacList
void CEncrypt::GetMacAddressList()
{
	if(!m_LocalMacList.IsEmpty())
	{
		m_LocalMacList.RemoveAll();
	}

	IP_ADAPTER_INFO *IpAdaptersInfo = NULL;
	IP_ADAPTER_INFO *IpAdaptersInfoHead = NULL;
	ULONG ulOutBufLen;
	DWORD dwRetVal;
	CString strMsg;
	CString strOutPut;
	IpAdaptersInfo = (IP_ADAPTER_INFO *) GlobalAlloc(GPTR,sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);   

	if(GetAdaptersInfo(IpAdaptersInfo,&ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		GlobalFree(IpAdaptersInfo);
		IpAdaptersInfo = (IP_ADAPTER_INFO*)GlobalAlloc(GPTR,ulOutBufLen);
	}

	if(dwRetVal = GetAdaptersInfo(IpAdaptersInfo,&ulOutBufLen))
	{
		//strMsg.Format("Call to GetAdaptersInfo failed.Return Value:%08x\r\n",dwRetVal);
		//strOutPut+=strMsg;
	}
	else   
	{
		IpAdaptersInfoHead = IpAdaptersInfo;
		do
		{
			strMsg.Format("%02X%02X%02X%02X%02X%02X",
				IpAdaptersInfo->Address[0],   
				IpAdaptersInfo->Address[1],   
				IpAdaptersInfo->Address[2],   
				IpAdaptersInfo->Address[3],   
				IpAdaptersInfo->Address[4],   
				IpAdaptersInfo->Address[5]);  

			strOutPut.Format("%s",IpAdaptersInfo->Description);
			int nFindPos = -1;
			nFindPos = strOutPut.Find("Wireless");
			if(nFindPos > 0)
			{
				//printf("无线网卡MAC:%s ,type:%d\n",strMsg,IpAdaptersInfo->Type);
			}
			else
			{
				//printf("非无线网卡MAC:%s ,type:%d\n",strMsg,IpAdaptersInfo->Type);
				m_LocalMacList.AddTail(strMsg);
			}
			

			IpAdaptersInfo = IpAdaptersInfo->Next;
		}
		while(IpAdaptersInfo);
	}
	GlobalFree(IpAdaptersInfoHead);

	strMsg = _T("0013EA4B6C0B");
	m_LocalMacList.AddTail(strMsg);
}

//获取磁盘序列号
BOOL CEncrypt::GetDiskSequence(char *psDiskSeq)
{
     LPCTSTR lpRootPathName="c:\\"; //取C盘
     LPTSTR lpVolumeNameBuffer=new char[12];//磁盘卷标
     DWORD nVolumeNameSize=12;// 卷标的字符串长度
     DWORD VolumeSerialNumber;//硬盘序列号
     DWORD MaximumComponentLength;
     LPTSTR lpFileSystemNameBuffer =new char[10];
     DWORD nFileSystemNameSize = 10;
     DWORD FileSystemFlags; //文件系统的一此标志

     BOOL bRes;
     bRes = ::GetVolumeInformation(lpRootPathName,
                    lpVolumeNameBuffer, 
                    nVolumeNameSize,
                    &VolumeSerialNumber,
                    &MaximumComponentLength,
                    &FileSystemFlags,
                    lpFileSystemNameBuffer,
                    nFileSystemNameSize);
     
     if(bRes)
     {
         sprintf(psDiskSeq,"%lx",VolumeSerialNumber); 
     }
     
     delete[] lpFileSystemNameBuffer;
     lpFileSystemNameBuffer = NULL;

     delete[] lpVolumeNameBuffer;
     lpVolumeNameBuffer = NULL;

     return bRes;
      
}

//根据网卡、磁盘序列号生成明文串
int CEncrypt::CreateClearText(char *psClearText,const char *psMacAddress)
{
    char sDiskSeq[MAX_CLEAR_TXT_LEN] = {0};
    char tempStr[MAX_CLEAR_TXT_LEN] = {0};
    int i;
    
    if(!GetDiskSequence(sDiskSeq))
    {
        sprintf(sDiskSeq,"1345764209"); //有些品牌的磁盘是没有序列号的,如果没有磁盘序列号的默认串
    }

    for(i = 0; i< MAX_CLEAR_TXT_LEN; i++)
    {
        tempStr[i] = (sDiskSeq[i] + psMacAddress[i]) % 64;
    }
    
    int nLen = strlen(psMacAddress);
    for(i = 0 ; i < nLen; i++)
    {
        psClearText[i] = EncryptStrTable[tempStr[i]];
    }

    //将网卡、磁盘序列号求和作为校验
    int nSum1 = 0;
    int nSum2 = 0;
    int nSum = 0;
    for(i = 0; i < MAX_CLEAR_TXT_LEN; i++)
    {
        nSum1 = nSum1 + sDiskSeq[i];
        nSum2 = nSum2 + psMacAddress[i];
    }
    nSum = nSum1 + nSum2;

    return nSum;
    
}

//加密
CString CEncrypt::EncryptText(CString S) 
{
	CString Result,str;
	int i,j;
	int ikey=0;
	BYTE bM;

	Result=S; // 初始化结果字符串
	for(i=0; i<S.GetLength(); i++) //依次对字符串中各字符进行操作
	{
		bM=KEY[ikey]+S.GetAt(i);
        if(bM > 255)
        {
            bM -= 256;
        }
		Result.SetAt(i,bM);

		ikey++;
		if(ikey>=4)
        {
            ikey=0;
        }
	}
	S=Result; // 保存结果
	Result.Empty(); // 清除结果
	for(i=0; i<S.GetLength(); i++) // 对加密结果进行转换
	{
		j=(BYTE)S.GetAt(i); // 提取字符
		// 将字符转换为两个字母保存
		str="12"; // 设置str长度为2
		str.SetAt(0, 49+j/26);
		str.SetAt(1, 65+j%26);
		Result += str;
	}

    return Result;
}

//解密
CString CEncrypt::DecryptText(CString S) // 解密函数
{
	CString Result,str;
	int i,j;
	int ikey=0;
	BYTE bM;

	Result.Empty(); // 清除结果
	for(i=0; i < S.GetLength()/2; i++) //将字符串两个字母一组进行处理
	{
		j = ((BYTE)S.GetAt(2*i)-49)*26;
		j += (BYTE)S.GetAt(2*i+1)-65;
		str="1"; // 设置str长度为1
		str.SetAt(0, j);
		Result+=str; // 追加字符，还原字符串
	}
	S=Result; // 保存中间结果
	for(i=0; i<S.GetLength(); i++) //依次对字符串中各字符进行操作
	{
		bM=S.GetAt(i);
		if(bM>=KEY[ikey])
        {
            bM=bM-KEY[ikey];
        }	
		else
        {
            bM=256+bM-KEY[ikey];
        }
        
		Result.SetAt(i,bM); //将密钥移位后与字符异或
		ikey++;
		if(ikey>=4)
        {
            ikey=0;
        }

	}
	return Result;
}

//验证加密串
BOOL CEncrypt::CompareEncryptText(CString strEncryptTxt,int &nClientCount)
{
    char sClearText[MAX_CLEAR_TXT_LEN] ={0};
	char sMacAddress[MAX_CLEAR_TXT_LEN] = {0};
    int nSum = 0;
	nClientCount = -1;
	CString strTemp;
	int nMacCount = m_LocalMacList.GetCount();
	POSITION posList = NULL;
	posList = m_LocalMacList.GetTailPosition();
	for(int i = 0; i < nMacCount;i++)
	{
		CString strTempMac = m_LocalMacList.GetAt(posList);

		memset(sClearText,0,MAX_CLEAR_TXT_LEN);
		memset(sMacAddress,0,MAX_CLEAR_TXT_LEN);
		
		_snprintf(sMacAddress,MAX_CLEAR_TXT_LEN -1,"%s",strTempMac);

		sMacAddress[0] = 'M'; //mac地址的第一个字符初时为M

		nSum = CreateClearText(sClearText,sMacAddress);

		//加密处理
		strTemp.Format("%s",sClearText);
		CString strEncrypt1 = EncryptText(strTemp);


		int nPos = nSum %24;
		int nTmpData = nPos%2;
		if(nTmpData > 0)
		{
			nPos = nPos - 1;
		}
		int nLen = strEncryptTxt.GetLength();
		int nLen1 = strEncrypt1.GetLength();
		nLen = nLen - nLen1;
		if(nLen < 0)
		{
			//移动到下一个节点
			m_LocalMacList.GetPrev(posList);
			continue;
		}

		CString str1 = strEncryptTxt.Mid(0,nPos);
		CString str2 = strEncryptTxt.Mid(nPos+nLen);

		CString strEncrypt2;
		strEncrypt2.Format("%s%s",str1,str2);

		
		if(strEncrypt1.Compare(strEncrypt2) != 0)
		{
			//移动到下一个节点
			m_LocalMacList.GetPrev(posList);
			continue;
		}

		CString strLimitInfo = strEncryptTxt.Mid(nPos,nLen);
		CString str3 = strLimitInfo.Mid(0,2);
		char cTemp[3] ={0};
		memcpy(cTemp,(char *)(LPCSTR)str3,2);

		int nFlag = -1;
		nFlag = cTemp[0] - '0';
		if(nFlag < 0 || nFlag > 9)
		{
			//移动到下一个节点
			m_LocalMacList.GetPrev(posList);
			continue;
		}
		else
		{
			if( EncryptLimitTB[nFlag] != cTemp[1])
			{
				//移动到下一个节点
				m_LocalMacList.GetPrev(posList);
				continue;
			}
		}

		//分析出最大连接数限制
		nFlag = nFlag%2; 
		if(nFlag) //表示有最大连接数限制
		{
			strLimitInfo = strLimitInfo.Mid(2);
			//解密
			strLimitInfo = DecryptText(strLimitInfo);
			nLen = strLimitInfo.GetLength();
			nClientCount = atoi(strLimitInfo);
			strLimitInfo.Format("%d",nClientCount);
			nLen1 = strLimitInfo.GetLength();
			if(nLen != nLen1) //表示解密出来的字符串不是全为数字
			{
				//移动到下一个节点
				nClientCount = -1;
				m_LocalMacList.GetPrev(posList);
				continue;
			}
		}

		return TRUE;
	}

	return FALSE;
}

//输出明文串(经过简单加密)，当加密串不正确时候输出该字符串,用户将该字符串给HYT,由HYT来生成密文
CString CEncrypt::OutputPWSDText()
{
    CString strResult;
    strResult = _T("");
    char sDiskSeq[MAX_CLEAR_TXT_LEN] = {0};
    char sMacAddress[MAX_CLEAR_TXT_LEN] = {0};
    char tempStr[MAX_CLEAR_TXT_LEN] = {0};

	if(m_LocalMacList.IsEmpty())
	{
		sprintf(sMacAddress,"0013EA4B6C0B");
	}
	else
	{
		CString strMac = m_LocalMacList.GetHead();
		_snprintf(sMacAddress,MAX_CLEAR_TXT_LEN-1,"%s",strMac);
	}

    int i = 0; 
    int nTempLen = strlen(sMacAddress);
    for(i = 0; i< nTempLen; i++)
    {
        switch(sMacAddress[i]) //随即生成的对应关系,唯一对应
        {
        case '0':
            sMacAddress[i] = 'K';
            break;
        case '1':
            sMacAddress[i] = 'C';
            break;
        case '2':
            sMacAddress[i] = '8';
            break;
        case '3':
            sMacAddress[i] = '#';
            break;
        case '4':
            sMacAddress[i] = 'D';
            break;
        case '5':
            sMacAddress[i] = '1';
            break;
        case '6':
            sMacAddress[i] = 'R';
            break;
        case '7':
            sMacAddress[i] = '0';
            break;
        case '8':
            sMacAddress[i] = 'G';
            break;
        case '9':
            sMacAddress[i] = 'P';
            break;
        case 'A':
            sMacAddress[i] = 'A';
            break;
        case 'B':
            sMacAddress[i] = 'T';
            break;
        case 'C':
            sMacAddress[i] = 'L';
            break;
        case 'D':
            sMacAddress[i] = '6';
            break;
        case 'E':
            sMacAddress[i] = '4';
            break;
        case 'F':
            sMacAddress[i] = 'X';
            break;
        }
    }
    
    if(!GetDiskSequence(sDiskSeq))
    {
        sprintf(sDiskSeq,"1345764209"); //有些品牌的磁盘是没有序列号的,如果没有磁盘序列号的默认串
    }
    CString strTemp1;
    strTemp1.Format("%s",sDiskSeq);

    CString strTemp2;
    strTemp2 = EncryptText(strTemp1);

    //MAC地址第一个字符随机取一个大写字符
    int nrnad = rand();
    sMacAddress[0] = 65+nrnad%26;

    strResult.Format("%s-%s",sMacAddress,strTemp2);

    return strResult;
    
}

//根据经过简单加密的明文串输出密文串
BOOL CEncrypt::OutputEncryptText(CString strPswd,BOOL isLimit,int nLimitCount, CString &strOutput)
{
    char sDiskSeq[MAX_CLEAR_TXT_LEN] = {0};
    char sMacAddress[MAX_CLEAR_TXT_LEN] = {0};
    char tempBuf[256] = {0};

    memcpy(tempBuf,(char *)(LPCSTR)strPswd,strPswd.GetLength());
    
    strtok(tempBuf,"-");
	char * psMacAddress = tempBuf;
	char * psDiskSeq = strtok(NULL,"-");

    if(psMacAddress == NULL || psDiskSeq == NULL)
    {
        return FALSE;
    }

    int i = 0;
    int nLen = strlen(psMacAddress);
    if(nLen != 12)
    {
        return FALSE;
    }

    sMacAddress[0] = 'M';
    for(i = 1; i < nLen; i++)
    {
        switch(psMacAddress[i])
        {
        case 'K':
            sMacAddress[i] = '0';
            break;
        case 'C':
            sMacAddress[i] = '1';
            break;
        case '8':
            sMacAddress[i] = '2';
            break;
        case '#':
            sMacAddress[i] = '3';
            break;
        case 'D':
            sMacAddress[i] = '4';
            break;
        case '1':
            sMacAddress[i] = '5';
            break;
        case 'R':
            sMacAddress[i] = '6';
            break;
        case '0':
            sMacAddress[i] = '7';
            break;
        case 'G':
            sMacAddress[i] = '8';
            break;
        case 'P':
            sMacAddress[i] = '9';
            break;
        case 'A':
            sMacAddress[i] = 'A';
            break;
        case 'T':
            sMacAddress[i] = 'B';
            break;
        case 'L':
            sMacAddress[i] = 'C';
            break;
        case '6':
            sMacAddress[i] = 'D';
            break;
        case '4':
            sMacAddress[i] = 'E';
            break;
        case 'X':
            sMacAddress[i] = 'F';
            break;
        default:
            return FALSE;
        }
    }

    CString strTemp;
    strTemp.Format("%s",psDiskSeq);
    CString strDiskSeq = DecryptText(strTemp);

    nLen = strDiskSeq.GetLength();
    if(nLen > 16)
    {
        return FALSE;
    }
    memcpy(sDiskSeq,(char *)(LPCSTR)strDiskSeq,nLen);

    //将网卡、磁盘序列号求和作为校验
    int nSum1 = 0;
    int nSum2 = 0;
    int nSum = 0;

    
    for(i = 0; i < MAX_CLEAR_TXT_LEN; i++)
    {
        nSum1 = nSum1 + sDiskSeq[i];
        nSum2 = nSum2 + sMacAddress[i];
    }
    nSum = nSum1 + nSum2;

    char tempStr[MAX_CLEAR_TXT_LEN] = {0};
    char strClearText[MAX_CLEAR_TXT_LEN] ={0};

    for(i = 0; i< MAX_CLEAR_TXT_LEN; i++)
    {
        tempStr[i] = (sDiskSeq[i] + sMacAddress[i]) % 64;
    }

    nLen = strlen(sMacAddress);
    for(i = 0 ; i < nLen; i++)
    {
        strClearText[i] = EncryptStrTable[tempStr[i]];
    }
    strTemp.Format("%s",strClearText); 
    strOutput = EncryptText(strTemp);

    //根据nSum值在密文后面在加0-4位可变长度字符,字符从EncryptStrTable表中取
    int nPos = nSum %24;
    if((nPos%2) > 0)
    {
        nPos = nPos - 1;
    }
    strTemp = strOutput.Mid(0,nPos);
    CString strTemp2;
    strTemp2 = strOutput.Mid(nPos);

    int nrnad = rand();
    int nTempData;
    char cLimit[3] ={0};
    if(isLimit) 
    {
        //生成一个偶数数字字符1,3,5,7,9
        nTempData = (nrnad%5) * 2 + 1;
    }
    else
    {
        //生成一个奇数的数字字符,0 2 4 6 8 
        nTempData = (nrnad%5) * 2;

        nLimitCount = nrnad%100;
    }
    cLimit[0] = nTempData + '0';
    cLimit[1] = EncryptLimitTB[nTempData];

    CString strTemp3;
    strTemp3.Format("%d",nLimitCount);

    strTemp3 = EncryptText(strTemp3);

    strOutput.Format("%s%s%s%s",strTemp,cLimit,strTemp3,strTemp2);

    return TRUE; 
}
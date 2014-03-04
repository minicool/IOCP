
#ifndef __GISMAIN_ENCRYPT_CLASS_
#define __GISMAIN_ENCRYPT_CLASS_

class CEncrypt
{
public:
    CEncrypt();
    ~CEncrypt();

    CString OutputEncryptText();
    CString OutputPWSDText();
    
    BOOL OutputEncryptText(CString strPswd,BOOL isLimit,int nLimitCount, CString &strOutput);
    
    BOOL CompareEncryptText(CString strEncryptTxt,int &nClientCount);

	void GetMacAddressList();
private:
    BOOL GetDiskSequence(char *psDiskSeq);

    int CreateClearText(char *psClearText,const char *psMacAddress);

    CString EncryptText(CString S);
    CString DecryptText(CString S); // ½âÃÜº¯Êý

	CList<CString, CString&> m_LocalMacList; 

    

};
#endif
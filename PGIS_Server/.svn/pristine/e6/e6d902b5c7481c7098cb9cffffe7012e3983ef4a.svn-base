/*********************************************************************
* Copyright (c) 2008, HYT
* All rights reserved.
* 
* 文件名称：AudioCodec.h
* 文件标识：interface for the AudioCodec class.
* 摘    要：内存池/FIFO的封装类
*
* 历史修改记录：
*   作者            日期            版本            描述
*
*********************************************************************/

#ifndef __MEMORYPOOL_H_INCLUDED_
#define __MEMORYPOOL_H_INCLUDED_

//////////////////////////////////////////////////////////////////////
// Macro/Struct definition
//////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
//环形队列定义
struct BUFFER_TYPE 
{
	unsigned int buffer_size; //缓冲区以字节为单位的大小
	char *pointer;     //缓冲区内存首地址
	unsigned short message_num; //消息个数
	unsigned int head;        //队列头元素距离缓冲区内存首地址的字节偏移
	unsigned int tail;        //队列尾元素距离缓冲区内存首地址的字节偏移
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////
class CMemoryPool 
{
public:
    CMemoryPool();
    virtual ~CMemoryPool(){};

    BOOL InitBuffer(unsigned int size );
    void FreeBuf();
    void ClearBuf();
    BOOL SendToBuffer( char * msg, unsigned short length );
    unsigned char InsertToBuffer(  char * msg, unsigned short length );
    unsigned int GetFromBuffer( char *  msg);
    unsigned int GetMsgLenFromBuffer();

    unsigned GetMessageNum();
    unsigned char GetOccupyPercent(); //取得占用百分比

protected:
    void RoundInc( unsigned int *c, unsigned int round );
    void RoundDec( unsigned int *c, unsigned int round );

protected:
    struct BUFFER_TYPE m_BufPool;
    CRITICAL_SECTION  m_CriSec;   //该缓冲所需临界区
};

#endif
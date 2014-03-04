// MemoryPool.cpp: implementation of the MemoryPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryPool.h"

//////////////////////////////////////////////////////////////////////
// Macro/Macro for design
//////////////////////////////////////////////////////////////////////

#define MAX_MSG_LENGTH 256

//////////////////////////////////////////////////////////////////////
// Class Function Definition
//////////////////////////////////////////////////////////////////////
CMemoryPool::CMemoryPool()
{
    memset(&m_BufPool, 0, sizeof(struct BUFFER_TYPE));
}

BOOL CMemoryPool::InitBuffer(unsigned int size )
{
	if(size == 0)
	{
		return FALSE;
	}
    /*allocate a block of buffer for buffer-pool.*/
	if ( ( m_BufPool.pointer = ( char *) malloc( size ) ) == NULL )
    {
		return FALSE;
    }
    
    m_BufPool.buffer_size = size;
    m_BufPool.tail        = 0;
    m_BufPool.head        = 0;
    m_BufPool.message_num = 0;
	InitializeCriticalSection(&m_CriSec);

    return TRUE ;
}

//清空FIFO中的数据
void CMemoryPool::ClearBuf()
{
	m_BufPool.head = m_BufPool.tail;
	m_BufPool.message_num = 0;
}

//释放FIFO申请的地址空间
void CMemoryPool::FreeBuf()
{
    m_BufPool.head = m_BufPool.tail;
    m_BufPool.message_num = 0;
	m_BufPool.buffer_size = 0;
	if(m_BufPool.pointer)
	{
		free(m_BufPool.pointer);
        //delete []m_BufPool.pointer;
	}
	m_BufPool.pointer = NULL;
	DeleteCriticalSection( &m_CriSec );
}

//插入一条消息到消息池最前端
unsigned char CMemoryPool::InsertToBuffer(  char * msg, unsigned short length )
{
	unsigned short i;
    struct BUFFER_TYPE * buf;
    buf = &m_BufPool;

	if( buf->pointer == NULL )
    {
        return  0;
    }
	
	/* Limit the maximum length of  message. */
	if( length > MAX_MSG_LENGTH )
    {
        length = MAX_MSG_LENGTH ;
    }
	
    EnterCriticalSection(&m_CriSec);
	if ( ( ( buf->head + buf->buffer_size - buf->tail )
				% buf->buffer_size +  length + 2 + 1  ) > buf->buffer_size )
    {
        LeaveCriticalSection(&m_CriSec);
		/* buffer is full */
		return 0;
    }
	if( buf->head < buf->tail )
	{
		memcpy(buf->pointer + buf->tail - length,msg ,length) ;
		buf->tail -= length ;
	}
	else
	{
		i = length - buf->tail ;
		if( buf->tail >= length )
			memcpy(buf->pointer + buf->tail - length,msg,length) ;
		else
		{
			memcpy(buf->pointer,msg + i,buf->tail) ;
			memcpy(buf->pointer + buf->buffer_size - i,msg ,i) ;
		}
		buf->tail = (buf->tail + buf->buffer_size - length) % buf->buffer_size ;
	}

	RoundDec(&(buf->tail),buf->buffer_size);
	*(buf->pointer + buf->tail) = (unsigned char) ( length >> 8 );
	RoundDec( &(buf->tail),buf->buffer_size );
	*(buf->pointer + buf->tail) = (unsigned char)length ;
	/* two byte for store length */
	buf -> message_num++ ;
    unsigned char res;
	res = (unsigned char)( ( ( buf->head + buf->buffer_size - buf->tail )
				% buf->buffer_size ) * ( unsigned int )100  / buf->buffer_size ) + 1 ;

    LeaveCriticalSection(&m_CriSec);
    
    return res;
}

//发送一条消息到消息池
BOOL CMemoryPool::SendToBuffer(char * msg, unsigned short length )
{
	unsigned int i;
    struct BUFFER_TYPE * buf;
    buf = &m_BufPool;

	if( length > MAX_MSG_LENGTH )
    {
        length = MAX_MSG_LENGTH ;
    }

	EnterCriticalSection(&m_CriSec);
	if( buf->pointer == 0 )
	{
		LeaveCriticalSection(&m_CriSec);	
		return  FALSE;
	}

	if ( ( (buf->head + buf->buffer_size - buf->tail)
		% buf->buffer_size + length + 2 + 1 ) > buf->buffer_size )
	{
		/* buffer is full */
		LeaveCriticalSection(&m_CriSec);
		return FALSE;
	}
	*( buf->pointer + buf->head ) = (unsigned char)length ;
	RoundInc( &( buf->head ),buf -> buffer_size);
	*( buf->pointer + buf->head ) = (unsigned char)(length >> 8);
    RoundInc( &( buf->head ),buf->buffer_size);

	*( buf->pointer + buf->head );
	*( buf->pointer + buf->head+ 1 );
        /* two byte for store length */
    if( buf->head < buf->tail )
    {
        memcpy(buf->pointer + buf->head ,msg ,length) ;
        buf->head += length ;
    }
    else
    {
        i = buf->buffer_size - buf->head ;
        if( i >= length )
		{
            memcpy(buf->pointer + buf->head,msg,length);
		}
        else
        {
            memcpy(buf->pointer + buf->head,msg,i);
            memcpy(buf->pointer,msg + i,length - i);
		}
        buf->head = (buf->head + length) % buf->buffer_size ;
    }
    buf->message_num++ ;

	LeaveCriticalSection(&m_CriSec);
    
    return TRUE;
}

//从消息池中取得一条消息
unsigned int CMemoryPool::GetFromBuffer(  char *  msg )
{
    unsigned short length, i;
    
    struct BUFFER_TYPE * buf;
    buf = &m_BufPool;

	EnterCriticalSection( &m_CriSec );

    if( buf->pointer == NULL )
	{
		LeaveCriticalSection(&m_CriSec);
        return  0;
	}

    if( buf->tail == buf->head )
	{
		LeaveCriticalSection(&m_CriSec);
        return 0; /* buffer is empty */
	}
	
	unsigned char lenstr[2] = {0};
	lenstr[0] = *(buf->pointer + buf->tail);
	length = lenstr[0] ;
	RoundInc( &(buf->tail), buf->buffer_size );
	lenstr[1] = *(buf->pointer + buf->tail);
	length = length  + lenstr[1] * 0x100;

	RoundInc( &(buf->tail), buf->buffer_size );

    //length = *(buf->pointer + buf->tail);
	//*(buf->pointer + buf->tail+1);
    //RoundInc( &(buf->tail), buf->buffer_size );
    //length = length  + *(buf->pointer + buf->tail) * 0x100;
    //RoundInc( &(buf->tail), buf->buffer_size );
    if( buf->head > buf->tail )
    {
        memcpy(msg,buf->pointer + buf->tail,length) ;
        buf->tail += length ;
    }
    else
    {
        i = buf->buffer_size - buf->tail ;
        if( i >= length )
		{
            memcpy(msg ,buf->pointer + buf->tail,length) ;
		}
        else
        {
            memcpy(msg ,buf->pointer + buf->tail,i) ;
            memcpy( msg+i,buf->pointer,length - i) ;
        }
        buf->tail = (buf->tail + length) % buf->buffer_size;
    }
    buf->message_num-- ;

    LeaveCriticalSection(&m_CriSec);
    
    return length;
}

//取得消息池中未处理的消息的总数
unsigned CMemoryPool::GetMessageNum()
{
    return m_BufPool.message_num;
}

//取得占用百分比
unsigned char CMemoryPool::GetOccupyPercent()
{
	return ( unsigned char )( ( ( m_BufPool.head + m_BufPool.buffer_size - m_BufPool.tail )
        % m_BufPool.buffer_size ) * ( unsigned int )100  / m_BufPool.buffer_size ) ;
}

//取得消息的长度
unsigned int CMemoryPool::GetMsgLenFromBuffer()
{
	unsigned int length;
    struct BUFFER_TYPE * buf;
    buf = &m_BufPool;
	if( buf->pointer == NULL )
	{
		return  0;
	}

	if( buf->tail == buf->head )
	{
		return 0; /* buffer is empty */
	}
	length = * ( buf->pointer + buf->tail );
	RoundInc( &(buf->tail), buf->buffer_size );
	length = length  + *(buf->pointer + buf->tail) * 0x100;
	RoundDec(&(buf->tail), buf->buffer_size );
	
    return length ;
}

void CMemoryPool::RoundInc( unsigned int *c, unsigned int round )
{
    if( ++ ( *c ) >= round )
	{
        *c = 0;
	}
}

void CMemoryPool::RoundDec( unsigned int *c, unsigned int round )
{
    if( ( *c ) == 0 )
	{
        *c = round - 1;
	}
    else
	{
        ( *c )--;
	}
}
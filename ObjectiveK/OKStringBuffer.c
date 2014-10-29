//
//  OKStringBuffer.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKStringBuffer.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


void    OKStringBufferAppend( struct OKStringBuffer * buf, const char* str )
{
    size_t  len = strlen(str) +1;
    if( buf->string == NULL )
    {
        buf->stringSize = ((len + 31) & ~31);
        buf->stringLength = len;
        buf->string = (char*) malloc( buf->stringSize );
        memcpy( buf->string, str, len );
    }
    else
    {
        size_t  newSize = ((buf->stringLength +(len -1) +31) & ~31);
        if( newSize != buf->stringSize )
        {
            char*   newBuf = realloc( buf->string, newSize );
            if( newBuf )
            {
                buf->string = newBuf;
                memcpy( buf->string +buf->stringLength -1, str, len );
                buf->stringSize = newSize;
                buf->stringLength += len -1;
            }
        }
        else
        {
            memcpy( buf->string +buf->stringLength -1, str, len );
            buf->stringLength += len -1;
        }
    }
}


void    OKStringBufferAppendFmt( struct OKStringBuffer * buf, const char* fmt, ... )
{
    va_list     argList;
    char        bufStr[1024] = {0};
    va_start(argList,fmt);
    vsnprintf( bufStr, sizeof(bufStr)-1, fmt, argList );
    va_end(argList);
    OKStringBufferAppend( buf, bufStr );
}

void    OKStringBufferFree( struct OKStringBuffer * buf )
{
    if( buf && buf->string )
    {
        free( buf->string );
        buf->string = NULL;
    }
}

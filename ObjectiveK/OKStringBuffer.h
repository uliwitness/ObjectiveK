//
//  OKStringBuffer.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#ifndef __ObjectiveK__OKStringBuffer__
#define __ObjectiveK__OKStringBuffer__

#include <stdio.h>

struct OKStringBuffer
{
    char*       string;         //! Buffer in which the string is stored.
    size_t      stringLength;   //! Length of the used portion of the buffer, including terminating zero.
    size_t      stringSize;     //! Size of the allocated buffer.
};

//! Append the string str to the buffer buf. If buf->string is NULL, this will allocate a copy of str and put it in the buffer.
extern void    OKStringBufferAppend( struct OKStringBuffer * buf, const char* str );

//! Append the given format string:
extern void    OKStringBufferAppendFmt( struct OKStringBuffer * buf, const char* fmt, ... );

//! De-allocate all memory used for the string buffer buf. If buf->string is NULL, do nothing.
extern void    OKStringBufferFree( struct OKStringBuffer * buf );

#endif /* defined(__ObjectiveK__OKStringBuffer__) */

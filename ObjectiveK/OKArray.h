//
//  OKArray.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 10/07/15.
//  Copyright (c) 2015 Uli Kusterer. All rights reserved.
//

#ifndef __ObjectiveK__OKArray__
#define __ObjectiveK__OKArray__

#include <stdlib.h>
#include <stdbool.h>


struct OKArray
{
    size_t  count;
    void**  entries;
    void    (*freeValue)( void* inValue );  // May be NULL.
};


bool    OKArrayAddEntry( struct OKArray * inArray, void* inElement );
void    OKArrayFree( struct OKArray * inArray );

#endif /* defined(__ObjectiveK__OKArray__) */

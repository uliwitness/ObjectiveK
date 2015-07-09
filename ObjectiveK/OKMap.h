//
//  OKMap.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 09/07/15.
//  Copyright (c) 2015 Uli Kusterer. All rights reserved.
//

/*
    Naïve implementation of a key/value data structure. Intended to be used
    for using strings as keys, and strings or another map as values.
 */

#ifndef __ObjectiveK__OKMap__
#define __ObjectiveK__OKMap__

#include <stdlib.h>
#include <stdbool.h>


struct OKMapEntry
{
    char* key;
    void* value;
};


struct OKMap
{
    size_t              count;
    void                (*freeValue)( void* inValue );
    void*               (*copyValue)( void* inValue );
    struct OKMapEntry * entries;
};


struct OKMap*   OKMallocStringToStringMap();    // Copies both keys and values.
struct OKMap*   OKMallocStringToMapMap();       // Copies keys, takes over ownership of values.

bool    OKMapAddEntry( struct OKMap * inMap, const char* key, void* value );
void*   OKMapFindEntry( struct OKMap * inMap, const char* key );

void    OKMapFree( struct OKMap * inMap );

#endif /* defined(__ObjectiveK__OKMap__) */

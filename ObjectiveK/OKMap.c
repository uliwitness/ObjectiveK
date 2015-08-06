//
//  OKMap.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 09/07/15.
//  Copyright (c) 2015 Uli Kusterer. All rights reserved.
//

#include "OKMap.h"
#include <memory.h>


void*   OKMapCopyStringValue( void* inOriginal )
{
    char*   originalStr = (char*)inOriginal;
    size_t  originalStrSize = strlen(originalStr) +1;
    void*   strCopy = malloc(originalStrSize);
    memcpy( strCopy, originalStr, originalStrSize );
    return strCopy;
}


struct OKMap*   OKMallocStringToStringMap()
{
    struct OKMap*   theMap = malloc( sizeof(struct OKMap) );
    if( theMap )
    {
        theMap->count = 0;
        theMap->entries = NULL;
        theMap->copyValue = OKMapCopyStringValue;
        theMap->freeValue = free;
    }
    return theMap;
}


struct OKMap*   OKMallocStringToRawPtrMap()
{
    struct OKMap*   theMap = malloc( sizeof(struct OKMap) );
    if( theMap )
    {
        theMap->count = 0;
        theMap->entries = NULL;
        theMap->copyValue = NULL;
        theMap->freeValue = NULL;
    }
    return theMap;
}


struct OKMap*   OKMallocStringToMapMap()
{
    struct OKMap*   theMap = malloc( sizeof(struct OKMap) );
    if( theMap )
    {
        theMap->count = 0;
        theMap->entries = NULL;
        theMap->copyValue = NULL;   // Don't copy, just take over the pointer.
        theMap->freeValue = (void (*)(void*)) OKMapFree;
    }
    return theMap;
}


bool    OKMapAddEntry( struct OKMap * inMap, const char* key, void* value )
{
    size_t  keySize = strlen(key) +1;
    char*   keyCopy = malloc(keySize);
    memcpy(keyCopy, key, keySize);
    
    if( inMap->count == 0 )
    {
        inMap->entries = malloc( sizeof(struct OKMapEntry) * inMap->count );
        if( !inMap->entries )
        {
            free(keyCopy);
            return false;
        }
        inMap->count++;
    }
    else
    {
        struct OKMapEntry * newEntries = realloc( inMap->entries, sizeof(struct OKMapEntry) * (inMap->count +1) );
        if( newEntries )
        {
            inMap->entries = newEntries;
            inMap->count++;
        }
        else
        {
            free(keyCopy);
            return false;
        }
    }

    inMap->entries[inMap->count-1].key = keyCopy;
    if( inMap->copyValue == NULL )
        inMap->entries[inMap->count-1].value = value;
    else
        inMap->entries[inMap->count-1].value = inMap->copyValue( value );

    return true;
}


void*   OKMapFindEntry( struct OKMap * inMap, const char* key )
{
    for( size_t x = 0; x < inMap->count; x++ )
    {
        if( strcmp(inMap->entries[x].key, key ) == 0 )
        {
            return inMap->entries[x].value;
        }
    }
    return NULL;
}


void    OKMapFree( struct OKMap * inMap )
{
    for( size_t x = 0; x < inMap->count; x++ )
    {
        free( inMap->entries[x].key );
        if( inMap->freeValue )
            inMap->freeValue( inMap->entries[x].value );
    }
    free( inMap );
}

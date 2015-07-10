//
//  OKArray.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 10/07/15.
//  Copyright (c) 2015 Uli Kusterer. All rights reserved.
//

#include "OKArray.h"


bool    OKArrayAddEntry( struct OKArray * inArray, void* inElement )
{
    if( inArray->count == 0 )
    {
        inArray->entries = malloc( sizeof(void*) );
        if( inArray->entries )
        {
            inArray->count++;
            inArray->entries[0] = inElement;
        }
        else
            return false;
    }
    else
    {
        void** newArray = realloc( inArray->entries, inArray->count +1 );
        if( newArray )
        {
            inArray->count++;
            inArray->entries = newArray;
        }
        else
            return false;
    }
    
    return true;
}


void    OKArrayFree( struct OKArray * inArray )
{
    if( inArray->freeValue )
    {
        for( size_t x = 0; x < inArray->count; x++ )
        {
            inArray->freeValue( inArray->entries[x] );
        }
    }
    if( inArray->entries )
    {
        free( inArray->entries );
    }
}

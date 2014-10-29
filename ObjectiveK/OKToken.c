//
//  OKToken.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKToken.h"
#include <string.h>
#include <stdio.h>


struct OKToken *   OKMallocToken( struct OKToken ** firstToken, struct OKToken ** prevToken, enum OKTokenMode tokenMode, int indentLevel, int lineNumber, const char* identifierStr, size_t identifierLen )
{
    if( tokenMode == OKTokenMode_Whitespace )
        return NULL;
    if( tokenMode != OKTokenMode_String && identifierLen == 0 )
        return NULL;
    
    struct OKToken  *   theToken = calloc( 1, sizeof(struct OKToken) +identifierLen +1 );
    theToken->tokenType = tokenMode;
    theToken->indentLevel = indentLevel;
    theToken->lineNumber = lineNumber;
    theToken->stringLen = identifierLen;
    memmove( theToken->string, identifierStr, identifierLen );
    
    if( *prevToken )
    {
        (*prevToken)->nextToken = theToken;
    }
    else
        *firstToken = theToken;
    *prevToken = theToken;
    
    return theToken;
}


void    OKFreeToken( struct OKToken * inToken )
{
    free(inToken);
}


void    OKFreeTokenList( struct OKToken * inToken )
{
    struct OKToken *    nextToken = inToken;
    while( nextToken )
    {
        struct OKToken *    currToken = nextToken;
        nextToken = currToken->nextToken;
        OKFreeToken(currToken);
    }
}


void    OKPrintToken( struct OKToken * theToken )
{
    static char        indentStr[41] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        0 };
    static size_t       indentStrCharCount = sizeof(indentStr) -1;
    char*   indentStart = indentStr +indentStrCharCount -((theToken->indentLevel > indentStrCharCount) ? indentStrCharCount : theToken->indentLevel);
    if( theToken->tokenType == OKTokenMode_LineBreak )
        printf("%s%s\n",indentStart,(theToken->string[0] == '\n')?"\\n":"\\r");
    else if( theToken->tokenType == OKTokenMode_String )
        printf("%s\"%s\"\n",indentStart,theToken->string);
    else if( theToken->stringLen > 0 )
        printf("%s%s\n",indentStart,theToken->string);
}


void    OKPrintTokenList( struct OKToken * inToken )
{
    struct OKToken *    currToken = inToken;
    while( currToken )
    {
        OKPrintToken(currToken);
        currToken = currToken->nextToken;
    }
}


bool    OKIsOperator( struct OKToken * inToken, const char* operatorName )
{
    return( inToken && inToken->tokenType == OKTokenMode_Operator && strcmp(inToken->string,operatorName) == 0 );
}


bool    OKIsIdentifier( struct OKToken * inToken, const char* identifierName )
{
    return( inToken && inToken->tokenType == OKTokenMode_Identifier && strcmp(inToken->string,identifierName) == 0 );
}


const char*    OKGetIdentifier( struct OKToken * inToken )
{
    if( inToken && inToken->tokenType == OKTokenMode_Identifier )
        return inToken->string;
    return NULL;
}

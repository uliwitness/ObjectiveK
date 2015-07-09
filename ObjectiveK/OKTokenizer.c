//
//  OKTokenizer.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKTokenizer.h"
#include <ctype.h>
#include <stdio.h>


//! Is this single character one that we treat as an operator outside of a string?
bool    OKIsOperatorChar( char currCh )
{
    switch( currCh )
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '|':
        case '\\':
        case ':':
        case ';':
        case '.':
        case ',':
        case '?':
        case '~':
        case '^':
        case '%':
        case '$':
        case '#':
        case '@':
        case '!':
        case '&':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '<':
        case '>':
        case '=':
            return true;
            break;
        default:
            return false;
    }
}


int    OKTokenize( FILE* theFile, struct OKToken ** outTokenList )
{
    struct OKToken *    prevToken = NULL;
    enum OKTokenMode    tokenMode = OKTokenMode_Whitespace;
    bool                isAtLineStart = true;
    int                 indentLevel = 0;
    char                identifierStr[1024] = {0};
    size_t              identifierLen = 0;
    int                 lineNumber = 1;
    
    *outTokenList = NULL;
    
    while( true )
    {
        char    currCh = 0;
        if( fread( &currCh, 1, sizeof(char), theFile ) != 1 )
            break;
        switch( tokenMode )
        {
            case OKTokenMode_Whitespace:    // Most of OKTokenMode_LineBreak must be identical to this!
                if( currCh == '\t' && isAtLineStart )
                    indentLevel += 4;   // Assuming 4-char tabs.
                else if( currCh == ' ' && isAtLineStart )
                    indentLevel += 1;
                else if( currCh == '\n' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else if( OKIsOperatorChar(currCh) )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_Operator, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = false;
                }
                else if( isdigit(currCh) )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Integer;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    isAtLineStart = false;
                }
                else if( currCh == '"' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                    isAtLineStart = false;
                }
                else if( currCh != ' ' && currCh != '\t' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Identifier;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    isAtLineStart = false;
                }
                break;
                
            case OKTokenMode_Integer:
                if( isdigit(currCh) )
                {
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: Integer longer than 1024 characters.\n");
                        return 2;
                    }
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                }
                else if( currCh == '.' )
                {
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: Integer longer than 1024 characters.\n");
                        return 2;
                    }
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                    tokenMode = OKTokenMode_Float;
                }
                else if( OKIsOperatorChar(currCh) )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_Operator, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Whitespace;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                }
                break;
                
            case OKTokenMode_Float:
                if( isdigit(currCh) )
                {
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: Decimal number longer than 1024 characters.\n");
                        return 2;
                    }
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                }
                else if( OKIsOperatorChar(currCh) ) // This catches any excess dots after a float and makes them operator tokens.
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_Operator, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Whitespace;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                }
                break;
                
            case OKTokenMode_Identifier:
                if( OKIsOperatorChar(currCh) )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_Operator, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else if( currCh == ' ' || currCh == '\t' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else
                {
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: Identifier longer than 1024 characters.\n");
                        return 2;
                    }
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                }
                break;
                
            case OKTokenMode_String:
                if( currCh == '"' )
                {
                    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Whitespace;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\\' )
                {
                    if( fread( &currCh, 1, sizeof(char), theFile ) != 1 )
                    {
                        fprintf(stderr,"error: Unterminated string constant in file.\n");
                        return 3;
                    }
                    
                    char    finalCh = 0;
                    switch( currCh )
                    {
                        case 'n':
                            finalCh = '\n';
                            break;
                        case 'r':
                            finalCh = '\r';
                            break;
                        case 't':
                            finalCh = '\t';
                            break;
                        case 'b':
                            finalCh = '\b';
                            break;
                        case '\\':
                            finalCh = '\\';
                            break;
                        case '0':
                            finalCh = 0;
                            break;
                    }
                    
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: String constant longer than 1024 characters.\n");
                        return 2;
                    }
                    identifierStr[identifierLen++] = finalCh;
                    identifierStr[identifierLen] = 0;
                }
                else
                {
                    if( identifierLen >= (sizeof(identifierStr) -1) )
                    {
                        fprintf(stderr,"error: String constant longer than 1024 characters.\n");
                        return 2;
                    }
                    if( currCh == '\n' )
                        lineNumber++;
                    else if( identifierLen > 0 && identifierStr[identifierLen-1] == '\r' )
                        lineNumber++;
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                }
                break;
                
            case OKTokenMode_LineBreak: // Only get here if sequence started with \r and we need to check if it's Windows-style \r\n:
                if( currCh == '\n' )
                {
                    identifierStr[1] = currCh; identifierStr[2] = 0;
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 2 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else    // \r was followed by something else, generate \r and continue as if it was whitespace:
                {
                    OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                    lineNumber++;
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                    
                    // The following is all identical to OKTokenMode_Whitespace case:
                    if( currCh == '\t' && isAtLineStart )
                        indentLevel += 4;   // Assuming 4-char tabs.
                    else if( currCh == ' ' && isAtLineStart )
                        indentLevel += 1;
                    else if( currCh == '\n' )
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        identifierStr[0] = currCh; identifierStr[1] = 0;
                        OKMallocToken( outTokenList, &prevToken, OKTokenMode_LineBreak, indentLevel, lineNumber, identifierStr, 1 );
                        lineNumber++;
                        indentLevel = 0;
                        identifierLen = 0;
                        isAtLineStart = true;
                    }
                    else if( currCh == '\r' )
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        identifierStr[0] = currCh; identifierStr[1] = 0;
                        tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                    }
                    else if( OKIsOperatorChar(currCh) )
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        identifierLen = 1;
                        identifierStr[0] = currCh;
                        identifierStr[1] = 0;
                        OKMallocToken( outTokenList, &prevToken, OKTokenMode_Operator, indentLevel, lineNumber, identifierStr, identifierLen );
                        identifierLen = 0;
                        tokenMode = OKTokenMode_Whitespace;
                    }
                    else if( isdigit(currCh) )
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        tokenMode = OKTokenMode_Integer;
                        identifierLen = 1;
                        identifierStr[0] = currCh;
                        identifierStr[1] = 0;
                    }
                    else if( currCh == '"' )
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        tokenMode = OKTokenMode_String;
                        identifierLen = 0;
                        identifierStr[0] = 0;
                    }
                    else
                    {
                        OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
                        tokenMode = OKTokenMode_Identifier;
                        identifierLen = 1;
                        identifierStr[0] = currCh;
                    }
                }
                break;
                
            case OKTokenMode_Operator:  // Are generated immediately, we should never be in this state.
            default:
                fprintf(stderr, "error: Parser ended up in unknown/impossible mode.\n");
                break;
        }
    }
    OKMallocToken( outTokenList, &prevToken, tokenMode, indentLevel, lineNumber, identifierStr, identifierLen );
    
    return 0;
}


void    OKSkipComments( struct OKToken ** ioToken )
{
    while( OKIsOperator( *ioToken, "/") && (*ioToken) != NULL && OKIsOperator( (*ioToken)->nextToken, "/") )
    {
        while( (*ioToken) && (*ioToken)->tokenType != OKTokenMode_LineBreak )
            *ioToken = (*ioToken)->nextToken;
    }
}


void    OKGoNextTokenSkippingComments( struct OKToken ** inToken )
{
    OKSkipComments(inToken);
    if( *inToken )
        *inToken = (*inToken)->nextToken;
    OKSkipComments(inToken);
}



//
//  main.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-27.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <memory.h>


enum OKTokenMode
{
    OKTokenMode_Whitespace,
    OKTokenMode_Identifier,
    OKTokenMode_Integer,
    OKTokenMode_Float,
    OKTokenMode_String,
    OKTokenMode_Operator,
    OKTokenMode_LineBreak
};


struct OKToken
{
    enum OKTokenMode    tokenType;
    int                 indentLevel;
    int                 lineNumber;
    struct OKToken*     nextToken;
    size_t              stringLen;
    char                string[0];
};


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
        printf("%s%d: %s\n",indentStart,theToken->tokenType,(theToken->string[0] == '\n')?"\\n":"\\r");
    else if( theToken->tokenType == OKTokenMode_String )
        printf("%s%d: \"%s\"\n",indentStart,theToken->tokenType,theToken->string);
    else if( theToken->stringLen > 0 )
        printf("%s%d: %s\n",indentStart,theToken->tokenType,theToken->string);
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


void    OKSkipComments( struct OKToken ** inToken )
{
    while( OKIsOperator( *inToken, "/") && (*inToken)->nextToken != NULL && OKIsOperator( (*inToken)->nextToken, "/") )
    {
        while( (*inToken) && (*inToken)->tokenType != OKTokenMode_LineBreak )
            *inToken = (*inToken)->nextToken;
    }
}


void    OKGoNextTokenSkippingComments( struct OKToken ** inToken )
{
    OKSkipComments(inToken);
    if( *inToken )
        *inToken = (*inToken)->nextToken;
    OKSkipComments(inToken);
}


void    OKPrintInitCode( const char* className, const char* varName )
{
    printf("\t(%s)->super->isa->init();\n", varName);
}


void    OKPrintDeallocCode( const char* className, const char* varName )
{
    printf("\t(%s)->super->isa->dealloc();\n", varName);
}


void    OKParseOneFunctionBody( struct OKToken ** inToken, const char* fileName, const char* className )
{
    while( (*inToken) && (*inToken)->indentLevel == 8 )
    {
        OKGoNextTokenSkippingComments(inToken); // +++ Actually parse function body.
    }
}


void    OKParseOneClassLevelConstruct( struct OKToken ** inToken, const char* fileName, const char* className )
{
    OKSkipComments( inToken );
    
    if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
    {
        OKGoNextTokenSkippingComments(inToken);
    }
    if( OKIsIdentifier( *inToken, "main" ) )
    {
        OKGoNextTokenSkippingComments( inToken );
        if( OKIsIdentifier( *inToken, "function" ) )
        {
            OKGoNextTokenSkippingComments( inToken );
            const char* funcName = OKGetIdentifier(*inToken);
            if( !funcName )
            {
                printf("error:%d: Main function needs a name.\n", (*inToken)->lineNumber);
                *inToken = NULL;
                return;
            }
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "int    main( int argc, const char** argv )\n" );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "{\n" );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "\tstruct %s this = {0};\n", className );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            OKPrintInitCode( className, "&this" );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf("\tint result = %s___%s( &this, argc, argv );\n", className, funcName );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            OKPrintDeallocCode( className, "&this" );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf("\treturn result;\n}\n" );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "int    %s___%s( %s* this", className, funcName, className);
            OKGoNextTokenSkippingComments( inToken );
            while( OKIsOperator( *inToken, "(") || OKIsOperator( *inToken, ",") )
            {
                OKGoNextTokenSkippingComments( inToken );
                if( OKIsOperator( *inToken, ")" ) )
                {
                    OKGoNextTokenSkippingComments( inToken );
                    break;
                }
                const char* typeName = OKGetIdentifier(*inToken);
                const char* paramName = NULL;
                OKGoNextTokenSkippingComments( inToken );
                if( !OKIsOperator( *inToken, ",") && !OKIsOperator( *inToken, ")") )
                {
                    paramName = OKGetIdentifier(*inToken);
                    printf( ", %s %s", typeName, paramName );
                }
                else
                    printf( ", %s", typeName );
            }
            printf( " )\n{\n");
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
            {
                OKGoNextTokenSkippingComments(inToken);
            }
            OKParseOneFunctionBody( inToken, fileName, className );
            printf( "}\n");
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
            {
                OKGoNextTokenSkippingComments(inToken);
            }
        }
    }
    else if( OKIsIdentifier( *inToken, "function" ) )
    {
        OKGoNextTokenSkippingComments( inToken );
        const char* funcName = OKGetIdentifier(*inToken);
        if( !funcName )
        {
            printf("error:%d: Function needs a name.\n", (*inToken)->lineNumber);
            *inToken = NULL;
            return;
        }
        printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
        printf( "int    %s___%s( %s* this", className, funcName, className);
        OKGoNextTokenSkippingComments( inToken );
        while( OKIsOperator( *inToken, "(") || OKIsOperator( *inToken, ",") )
        {
            OKGoNextTokenSkippingComments( inToken );
            if( OKIsOperator( *inToken, ")" ) )
            {
                OKGoNextTokenSkippingComments( inToken );
                break;
            }
            const char* typeName = OKGetIdentifier(*inToken);
            const char* paramName = NULL;
            OKGoNextTokenSkippingComments( inToken );
            if( !OKIsOperator( *inToken, ",") && !OKIsOperator( *inToken, ")") )
            {
                paramName = OKGetIdentifier(*inToken);
                printf( ", %s %s", typeName, paramName );
            }
            else
                printf( ", %s", typeName );
        }
        printf( " )\n{\n");
        OKParseOneFunctionBody( inToken, fileName, className );
        printf( "}\n");
        
        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
    }
    else if( *inToken )
    {
        fprintf(stderr, "error:%d: Unknown class-level construct %s in class %s.", (*inToken)->lineNumber, (*inToken)->string, className);
        *inToken = NULL;
    }
}


void    OKParseOneTopLevelConstruct( struct OKToken ** inToken, const char* fileName )
{
    OKSkipComments( inToken );
    
    if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
    {
        OKGoNextTokenSkippingComments( inToken );
    }
    else if( OKIsIdentifier( *inToken, "class" ) )
    {
        OKGoNextTokenSkippingComments( inToken );
        const char* className = OKGetIdentifier(*inToken);
        if( className )
        {
            const char*     superclassName = "object";
            OKGoNextTokenSkippingComments( inToken );
            if( OKIsOperator( *inToken, ":") )  // Have superclass!
            {
                OKGoNextTokenSkippingComments( inToken );
                superclassName = OKGetIdentifier(*inToken);
            }
            
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "struct %s\n{\n", className );
            printf( "#line %d \"%s\"\n", (*inToken)->lineNumber, fileName );
            printf( "\tstruct %s\tsuper;\n", superclassName );
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            while( (*inToken) && (*inToken)->indentLevel == 4 )
                OKParseOneClassLevelConstruct( inToken, fileName, className );
            printf( "};\n" );
        }
    }
    else if( *inToken )
    {
        fprintf( stderr, "error:%d: Unknown top-level construct %s.", (*inToken)->lineNumber, (*inToken)->string );
        *inToken = NULL;
    }
}


void    OKParseTokenList( struct OKToken * inToken, const char* fileName )
{
    struct OKToken *    currToken = inToken;
    while( currToken )
    {
        OKParseOneTopLevelConstruct( &currToken, fileName );
    }
}


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
                    tokenMode = OKTokenMode_Identifier;
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
                    tokenMode = OKTokenMode_Identifier;
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


int main( int argc, const char * argv[] )
{
    if( argc < 2 )
    {
        fprintf( stderr, "error: Expected source file path as parameter.\n" );
        return 1;
    }
    FILE*   theFile = fopen( argv[1], "r" );
    if( !theFile )
    {
        fprintf( stderr, "error: Can't find file \"%s\".\n", argv[1] );
        return 1;
    }
    
    struct OKToken *    tokenList = NULL;
    int result = OKTokenize( theFile, &tokenList );
    if( result != 0 )
        return result;
    OKPrintTokenList( tokenList );
    
    OKParseTokenList( tokenList, argv[1] );
    
    OKFreeTokenList( tokenList );
    
    return result;
}

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


void    OKFinishToken( enum OKTokenMode tokenMode, int indentLevel, const char* identifierStr, size_t identifierLen )
{
    static char        indentStr[41] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                            0 };
    static size_t       indentStrCharCount = sizeof(indentStr) -1;
    char*   indentStart = indentStr +indentStrCharCount -((indentLevel > indentStrCharCount) ? indentStrCharCount : indentLevel);
    if( tokenMode == OKTokenMode_LineBreak )
        printf("%s%d: %s\n",indentStart,tokenMode,(identifierStr[0] == '\n')?"\\n":"\\r");
    else if( tokenMode == OKTokenMode_String )
        printf("%s%d: \"%s\"\n",indentStart,tokenMode,identifierStr);
    else if( identifierLen > 0 )
        printf("%s%d: %s\n",indentStart,tokenMode,identifierStr);
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


int    OKTokenize( FILE* theFile )
{
    enum OKTokenMode    tokenMode = OKTokenMode_Whitespace;
    bool                isAtLineStart = true;
    int                 indentLevel = 0;
    char                identifierStr[1024] = {0};
    size_t              identifierLen = 0;
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
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
                    indentLevel = 0;
                    identifierLen = 0;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else if( OKIsOperatorChar(currCh) )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_Operator, indentLevel, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = false;
                }
                else if( isdigit(currCh) )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Integer;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    isAtLineStart = false;
                }
                else if( currCh == '"' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                    isAtLineStart = false;
                }
                else if( currCh != ' ' && currCh != '\t' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
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
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_Operator, indentLevel, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
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
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_Operator, indentLevel, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_Identifier;
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                }
                break;
                
            case OKTokenMode_Identifier:
                if( OKIsOperatorChar(currCh) )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierLen = 1;
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_Operator, indentLevel, identifierStr, identifierLen );
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                }
                else if( currCh == '"' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    tokenMode = OKTokenMode_String;
                    identifierLen = 0;
                    identifierStr[0] = 0;
                }
                else if( currCh == '\n' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh; identifierStr[1] = 0;
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else if( currCh == '\r' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                    identifierStr[0] = currCh;
                    identifierStr[1] = 0;
                    identifierLen = 1;
                    tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                }
                else if( currCh == ' ' || currCh == '\t' )
                {
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
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
                    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
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
                    identifierStr[identifierLen++] = currCh;
                    identifierStr[identifierLen] = 0;
                }
                break;
                
            case OKTokenMode_LineBreak: // Only get here if sequence started with \r and we need to check if it's Windows-style \r\n:
                if( currCh == '\n' )
                {
                    identifierStr[1] = currCh; identifierStr[2] = 0;
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 2 );
                    indentLevel = 0;
                    identifierLen = 0;
                    tokenMode = OKTokenMode_Whitespace;
                    isAtLineStart = true;
                }
                else    // \r was followed by something else, generate \r and continue as if it was whitespace:
                {
                    OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
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
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                        identifierStr[0] = currCh; identifierStr[1] = 0;
                        OKFinishToken( OKTokenMode_LineBreak, indentLevel, identifierStr, 1 );
                        indentLevel = 0;
                        identifierLen = 0;
                        isAtLineStart = true;
                    }
                    else if( currCh == '\r' )
                    {
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                        identifierStr[0] = currCh; identifierStr[1] = 0;
                        tokenMode = OKTokenMode_LineBreak;  // Could be Windows \r\n sequence, so don't generate yet.
                    }
                    else if( OKIsOperatorChar(currCh) )
                    {
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                        identifierLen = 1;
                        identifierStr[0] = currCh;
                        identifierStr[1] = 0;
                        OKFinishToken( OKTokenMode_Operator, indentLevel, identifierStr, identifierLen );
                        identifierLen = 0;
                        tokenMode = OKTokenMode_Whitespace;
                    }
                    else if( isdigit(currCh) )
                    {
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                        tokenMode = OKTokenMode_Integer;
                        identifierLen = 1;
                        identifierStr[0] = currCh;
                        identifierStr[1] = 0;
                    }
                    else if( currCh == '"' )
                    {
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
                        tokenMode = OKTokenMode_String;
                        identifierLen = 0;
                        identifierStr[0] = 0;
                    }
                    else
                    {
                        OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
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
    OKFinishToken( tokenMode, indentLevel, identifierStr, identifierLen );
    
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
    
    int result = OKTokenize(theFile);
    if( result != 0 )
        return result;
    
    return result;
}

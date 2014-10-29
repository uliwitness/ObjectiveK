//
//  main.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-27.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKToken.h"
#include "OKTokenizer.h"
#include "OKParser.h"
#include <string.h>
#include <sys/param.h>


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
    
    char                        headerFilePath[MAXPATHLEN] = {0};
    strncpy( headerFilePath, argv[1], sizeof(headerFilePath)-1 );
    strncat( headerFilePath, ".h", sizeof(headerFilePath)-1 );
    
    char                        sourceFilePath[MAXPATHLEN] = {0};
    strncpy( sourceFilePath, argv[1], sizeof(sourceFilePath)-1 );
    strncat( sourceFilePath, ".c", sizeof(sourceFilePath)-1 );
    
    struct OKParseContext       context = {0};
    context.fileName = argv[1];
    context.headerFile = fopen( headerFilePath, "w");
    context.sourceFile = fopen( sourceFilePath, "w");
    
    fprintf( context.headerFile, "//\n//  Header auto-generated from %s\n", argv[1] );
    fprintf( context.headerFile, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n\n" );
    
    fprintf( context.sourceFile, "//\n//  Source file auto-generated from %s\n", argv[1] );
    fprintf( context.sourceFile, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n" );
    fprintf( context.sourceFile, "\n#include \"%s\"\n\n", headerFilePath );
    
    OKParseTokenList( tokenList, &context );
    
    OKFreeTokenList( tokenList );
    
    return result;
}

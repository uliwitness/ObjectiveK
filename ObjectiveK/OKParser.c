//
//  OKParser.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKParser.h"
#include "OKTokenizer.h"


void    OKPrintInitCode( const char* varName, struct OKParseContext* context )
{
    fprintf( context->sourceFile, "\t(%s)->super->isa->init();\n", varName );
}


void    OKPrintDeallocCode( const char* varName, struct OKParseContext* context )
{
    fprintf( context->sourceFile, "\t(%s)->super->isa->dealloc();\n", varName);
}


void    OKParseOneFunctionBody( struct OKToken ** inToken, struct OKParseContext* context )
{
    while( (*inToken) && (*inToken)->indentLevel == 8 )
    {
        OKGoNextTokenSkippingComments(inToken); // +++ Actually parse function body.
    }
}


void    OKParseOneClassLevelConstruct( struct OKToken ** inToken, struct OKParseContext* context )
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
                fprintf( stderr, "error:%d: Main function needs a name.\n", (*inToken)->lineNumber);
                *inToken = NULL;
                return;
            }
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "int    main( int argc, const char** argv )\n" );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "{\n" );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\tstruct %s this = {0};\n", context->className );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintInitCode( "&this", context );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\tint result = %s___%s( &this, argc, argv );\n", context->className, funcName );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintDeallocCode( "&this", context );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\treturn result;\n}\n" );
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
            OKGoNextTokenSkippingComments( inToken );
            while( OKIsOperator( *inToken, "(") || OKIsOperator( *inToken, ",") )
            {
                OKGoNextTokenSkippingComments( inToken );
                if( !OKIsOperator( *inToken, ")") )
                    OKGoNextTokenSkippingComments( inToken );
                else
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
                    fprintf( context->sourceFile, ", %s %s", typeName, paramName );
                    OKGoNextTokenSkippingComments( inToken );
                }
                else
                    fprintf( context->sourceFile, ", %s", typeName );
            }
            if( OKIsOperator( *inToken, ")") )
                OKGoNextTokenSkippingComments( inToken );
            fprintf( context->sourceFile, " )\n{\n");
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
            {
                OKGoNextTokenSkippingComments(inToken);
            }
            OKParseOneFunctionBody( inToken, context );
            fprintf( context->sourceFile, "}\n");
            
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
        fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        fprintf( context->sourceFile, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
        OKGoNextTokenSkippingComments( inToken );
        while( OKIsOperator( *inToken, "(") || OKIsOperator( *inToken, ",") || OKIsOperator( *inToken, ")") )
        {
            if( !OKIsOperator( *inToken, ")") )
                OKGoNextTokenSkippingComments( inToken );
            else
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
                fprintf( context->sourceFile, ", %s %s", typeName, paramName );
                OKGoNextTokenSkippingComments( inToken );
            }
            else
                fprintf( context->sourceFile, ", %s", typeName );
        }
        fprintf( context->sourceFile, " )\n{\n");
        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
        OKParseOneFunctionBody( inToken, context );
        fprintf( context->sourceFile, "}\n");
        
        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
    }
    else if( *inToken )
    {
        fprintf(stderr, "error:%d: Unknown class-level construct %s in class %s.", (*inToken)->lineNumber, (*inToken)->string, context->className);
        *inToken = NULL;
    }
}


void    OKParseOneTopLevelConstruct( struct OKToken ** inToken, struct OKParseContext* context )
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
            
            fprintf( context->headerFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "struct %s\n{\n", className );
            fprintf( context->headerFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "\tstruct %s\tsuper;\n", superclassName );
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = className;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
            }
            fprintf( context->headerFile, "};\n" );
        }
    }
    else if( OKIsIdentifier( *inToken, "extension" ) )
    {
        OKGoNextTokenSkippingComments( inToken );
        const char* className = OKGetIdentifier(*inToken);
        if( className )
        {
            const char*     superclassName = NULL;
            OKGoNextTokenSkippingComments( inToken );
            if( !OKIsOperator( *inToken, ":") )  // Have superclass!
            {
                fprintf( stderr, "error:%d: Extension %s does not specify the class it extends.", (*inToken)->lineNumber, className );
                *inToken = NULL;
                return;
            }
            OKGoNextTokenSkippingComments( inToken );
            superclassName = OKGetIdentifier(*inToken);
            if( !superclassName )
            {
                fprintf( stderr, "error:%d: Extension %s does not specify the class it extends.", (*inToken)->lineNumber, className );
                *inToken = NULL;
                return;
            }
            OKGoNextTokenSkippingComments( inToken );
            
            fprintf( context->headerFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "// Extension %s to class %s\n", className, superclassName );
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = superclassName;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
            }
        }
    }
    else if( *inToken )
    {
        fprintf( stderr, "error:%d: Unknown top-level construct %s.", (*inToken)->lineNumber, (*inToken)->string );
        *inToken = NULL;
    }
}


void    OKParseTokenList( struct OKToken * inToken, struct OKParseContext* context )
{
    struct OKToken *    currToken = inToken;
    while( currToken )
    {
        OKParseOneTopLevelConstruct( &currToken, context );
    }
}


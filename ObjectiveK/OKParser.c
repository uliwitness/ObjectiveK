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
    OKStringBufferAppendFmt( &context->sourceString, "\t%s___init_isa();\n", context->className );
    OKStringBufferAppendFmt( &context->sourceString, "\t((struct object*)%s)->isa->init( (struct object*)%s );\n", varName, varName );
}


void    OKPrintDeallocCode( const char* varName, struct OKParseContext* context )
{
    OKStringBufferAppendFmt( &context->sourceString, "\t((struct object*)%s)->isa->dealloc( (struct object*)%s );\n", varName, varName);
}


void    OKParseOneFunctionBody( struct OKToken ** inToken, struct OKParseContext* context )
{
    while( (*inToken) && (*inToken)->indentLevel == 8 )
    {
        OKGoNextTokenSkippingComments(inToken); // +++ Actually parse function body.
    }
    OKStringBufferAppendFmt( &context->sourceString, "\treturn 0;\n");    // +++ Only until we actually parse the body.
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
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "int    main( int argc, const char** argv )\n" );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "{\n" );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "\tstruct %s this = {(struct object_isa*)&%s___isa};\n", context->className, context->className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintInitCode( "&this", context );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "\tint result = ((struct %s_isa*) ((struct object*)&this)->isa)->%s( &this );\n", context->className, funcName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "\tint (*%s)( struct %s * this );\n", funcName, context->className );
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "\t%s___%s,\n", context->className, funcName );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintDeallocCode( "&this", context );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "\treturn result;\n}\n\n" );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
            OKStringBufferAppendFmt( &context->sourceString, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
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
                    OKStringBufferAppendFmt( &context->headerString, ", %s %s", typeName, paramName );
                    OKStringBufferAppendFmt( &context->sourceString, ", %s %s", typeName, paramName );
                    OKGoNextTokenSkippingComments( inToken );
                }
                else
                {
                    OKStringBufferAppendFmt( &context->headerString, ", %s", typeName );
                    OKStringBufferAppendFmt( &context->sourceString, ", %s", typeName );
                }
            }
            if( OKIsOperator( *inToken, ")") )
                OKGoNextTokenSkippingComments( inToken );
            OKStringBufferAppendFmt( &context->headerString, " );\n");
            OKStringBufferAppendFmt( &context->sourceString, " )\n{\n");
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
            {
                OKGoNextTokenSkippingComments(inToken);
            }
            OKParseOneFunctionBody( inToken, context );
            OKStringBufferAppendFmt( &context->sourceString, "}\n\n");
            
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
        if( !context->suppressLineDirectives )
            OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        OKStringBufferAppendFmt( &context->headerString, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
        OKStringBufferAppendFmt( &context->sourceString, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
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
                OKStringBufferAppendFmt( &context->headerString, ", %s %s", typeName, paramName );
                OKStringBufferAppendFmt( &context->sourceString, ", %s %s", typeName, paramName );
                OKGoNextTokenSkippingComments( inToken );
            }
            else
            {
                OKStringBufferAppendFmt( &context->headerString, ", %s", typeName );
                OKStringBufferAppendFmt( &context->sourceString, ", %s", typeName );
            }
        }
        OKStringBufferAppendFmt( &context->headerString, " );\n");
        OKStringBufferAppendFmt( &context->sourceString, " )\n{\n");
        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
        OKParseOneFunctionBody( inToken, context );
        OKStringBufferAppendFmt( &context->sourceString, "}\n\n");
        
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
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "struct %s\n{\n", className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "\tstruct %s\tsuper;\n", superclassName );
            OKStringBufferAppendFmt( &context->headerString, "};\n\n" );
            
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "struct %s_isa\n{\n", className );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "\tstruct %s_isa\tsuper;\n", superclassName );
            
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "struct %s_isa   %s___isa =\n{\n", className, className );
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "\t{0}, // Set to %s___isa by %s___init_isa().\n", superclassName, className );
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = className;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
            }
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "};\n\n" );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "extern struct %s_isa   %s___isa;\n", className, className );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "void %s___init_isa( void );\n\n", className );
            OKStringBufferAppend( &context->headerString, context->currentVTableHeaderString.string );
            OKStringBufferFree( &context->currentVTableHeaderString );
            
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "};\n\n" );
            OKStringBufferAppend( &context->sourceString, context->currentVTableSourceString.string );
            OKStringBufferFree( &context->currentVTableSourceString );
            
            OKStringBufferAppendFmt( &context->sourceString, "void %s___init_isa( void )\n{\n", className );
            OKStringBufferAppendFmt( &context->sourceString, "\t%s___init_isa();\n", superclassName );
            OKStringBufferAppendFmt( &context->sourceString, "\t%s___isa.super = %s___isa;\n", className, superclassName );
            OKStringBufferAppendFmt( &context->sourceString, "};\n\n" );
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
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "// Extension %s to class %s\n", className, superclassName );
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


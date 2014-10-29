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
    fprintf( context->sourceFile, "\t%s___init_isa();\n", context->className );
    fprintf( context->sourceFile, "\t(%s)->super.isa->init( (struct object*)%s );\n", varName, varName );
}


void    OKPrintDeallocCode( const char* varName, struct OKParseContext* context )
{
    fprintf( context->sourceFile, "\t(%s)->super.isa->dealloc( (struct object*)%s );\n", varName, varName);
}


void    OKParseOneFunctionBody( struct OKToken ** inToken, struct OKParseContext* context )
{
    while( (*inToken) && (*inToken)->indentLevel == 8 )
    {
        OKGoNextTokenSkippingComments(inToken); // +++ Actually parse function body.
    }
    fprintf( context->sourceFile, "\treturn 0;\n");    // +++ Only until we actually parse the body.
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
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "int    main( int argc, const char** argv )\n" );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "{\n" );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\tstruct %s this = {(struct object_isa*)&%s___isa};\n", context->className, context->className );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintInitCode( "&this", context );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\tint result = %s___%s( &this );\n", context->className, funcName );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintDeallocCode( "&this", context );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->sourceFile, "\treturn result;\n}\n\n" );
            if( !context->suppressLineDirectives )
                fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
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
                    fprintf( context->headerFile, ", %s %s", typeName, paramName );
                    fprintf( context->sourceFile, ", %s %s", typeName, paramName );
                    OKGoNextTokenSkippingComments( inToken );
                }
                else
                {
                    fprintf( context->headerFile, ", %s", typeName );
                    fprintf( context->sourceFile, ", %s", typeName );
                }
            }
            if( OKIsOperator( *inToken, ")") )
                OKGoNextTokenSkippingComments( inToken );
            fprintf( context->headerFile, " );\n");
            fprintf( context->sourceFile, " )\n{\n");
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
            {
                OKGoNextTokenSkippingComments(inToken);
            }
            OKParseOneFunctionBody( inToken, context );
            fprintf( context->sourceFile, "}\n\n");
            
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
            fprintf( context->sourceFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        fprintf( context->headerFile, "int    %s___%s( struct %s* this", context->className, funcName, context->className);
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
                fprintf( context->headerFile, ", %s %s", typeName, paramName );
                fprintf( context->sourceFile, ", %s %s", typeName, paramName );
                OKGoNextTokenSkippingComments( inToken );
            }
            else
            {
                fprintf( context->headerFile, ", %s", typeName );
                fprintf( context->sourceFile, ", %s", typeName );
            }
        }
        fprintf( context->headerFile, " );\n");
        fprintf( context->sourceFile, " )\n{\n");
        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
        OKParseOneFunctionBody( inToken, context );
        fprintf( context->sourceFile, "}\n\n");
        
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
                fprintf( context->headerFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "struct %s\n{\n", className );
            if( !context->suppressLineDirectives )
                fprintf( context->headerFile, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            fprintf( context->headerFile, "\tstruct %s\tsuper;\n", superclassName );
            fprintf( context->headerFile, "};\n\n" );
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = className;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
            }
            fprintf( context->headerFile, "struct %s_isa\n{\n", className );
            fprintf( context->headerFile, "\tstruct %s_isa\tsuper;\n", superclassName );
            fprintf( context->headerFile, "};\n\n" );
            fprintf( context->headerFile, "extern struct %s_isa   %s___isa;\n", className, className );
            fprintf( context->headerFile, "void %s___init_isa( void );\n\n", className );
           
            fprintf( context->sourceFile, "struct %s_isa   %s___isa =\n{\n", className, className );
            fprintf( context->sourceFile, "\t0, // Set to %s___isa by %s___init_isa().\n", superclassName, className );
            fprintf( context->sourceFile, "};\n\n" );
            
            fprintf( context->sourceFile, "void %s___init_isa( void )\n{\n", className );
            fprintf( context->sourceFile, "\t%s___init_isa();\n", superclassName );
            fprintf( context->sourceFile, "\t%s___isa.super = %s___isa;\n", className, superclassName );
            fprintf( context->sourceFile, "};\n\n" );

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


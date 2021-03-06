//
//  OKParser.c
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#include "OKParser.h"
#include "OKTokenizer.h"
#include <memory.h>


void    OKAppendStringLiteralToStringBufferAndEscapeIt( struct OKStringBuffer * buf, const char* str )
{
    OKStringBufferAppend( buf, "\"" );
    for( size_t x = 0; str[x] != 0; x++ )
    {
        char    unescapedChar[2] = {0};
        char    escapedQuote[3] = { '\\', '"', 0 };
        char    escapedNewline[3] = { '\\', 'n', 0 };
        char    escapedReturn[3] = { '\\', 'r', 0 };
        char    escapedBackslash[3] = { '\\', '\\', 0 };
        char    escapedNull[3] = { '\\', '0', 0 };
        switch( str[x] )
        {
            case '"':
                OKStringBufferAppend( buf, escapedQuote );
                break;
            
            case '\n':
                OKStringBufferAppend( buf, escapedNewline );
                break;
                
            case '\r':
                OKStringBufferAppend( buf, escapedReturn );
                break;
                
            case '\\':
                OKStringBufferAppend( buf, escapedBackslash );
                break;
                
            case 0:
                OKStringBufferAppend( buf, escapedNull );
                break;
                
            default:
                unescapedChar[0] = str[x];
                OKStringBufferAppend( buf, unescapedChar );
        }
    }
    OKStringBufferAppend( buf, "\"" );
}


void    OKPrintInitCode( const char* varName, const char* varType, struct OKParseContext* context )
{
    OKStringBufferAppendFmt( &context->sourceString, "\t%s___init_isa();\n", varType );
    OKStringBufferAppendFmt( &context->sourceString, "\t((struct object*)%s)->isa->init( (struct object*)%s );\n", varName, varName );
}


void    OKPrintDeallocCode( const char* varName, struct OKParseContext* context )
{
    OKStringBufferAppendFmt( &context->sourceString, "\t((struct object*)%s)->isa->dealloc( (struct object*)%s );\n", varName, varName);
}


void    OKParseOneExpression( struct OKToken ** inToken, const char* operatorToEndOn, struct OKParseContext* context )
{
    const char*     strContent = OKGetStringLiteral(*inToken);
    const char*     intContent = OKGetIntegerLiteral(*inToken);
    const char*     floatContent = OKGetFloatLiteral(*inToken);
    const char*     identContent = OKGetIdentifier(*inToken);
    if( strContent )
    {
        char        constantName[1024] = {0};
        snprintf(constantName, sizeof(constantName)-1, "kString___%zu", ++context->constantsIDSeed );
        OKStringBufferAppendFmt( &context->constantsString, "struct string %s = { (struct object_isa*) &string___isa, %zu, ", constantName, strlen(strContent) );
        OKAppendStringLiteralToStringBufferAndEscapeIt( &context->constantsString, strContent );
        OKStringBufferAppend( &context->constantsString, ", false };\n" );
        OKStringBufferAppend( &context->sourceString, "&" );
        OKStringBufferAppend( &context->sourceString, constantName );
        OKGoNextTokenSkippingComments(inToken);
    }
    else if( intContent )
    {
        OKStringBufferAppend( &context->sourceString, intContent );
        OKGoNextTokenSkippingComments(inToken);
    }
    else if( floatContent )
    {
        OKStringBufferAppend( &context->sourceString, floatContent );
        OKGoNextTokenSkippingComments(inToken);
    }
    else if( identContent )
    {
        OKStringBufferAppend( &context->sourceString, identContent );
        OKGoNextTokenSkippingComments(inToken);
    }
    else
    {
        fprintf( stderr, "error:%d: Unrecognized expression starting with '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
        *inToken = NULL;
        return;
    }
}


const char* OKFindClassForMethodOfClass( const char* funcName, const char* leafClass, struct OKParseContext* context )
{
    const char* currClass = leafClass;
    while( currClass != NULL )
    {
        struct OKMap*   objMethods = OKMapFindEntry( context->classes, currClass );
        if( !objMethods )
            return NULL;
        
        const char* funcSig = OKMapFindEntry( objMethods, funcName );
        if( funcSig )
            return currClass;
        
        currClass = (const char*) OKMapFindEntry( objMethods, ":superclass" );
    }
    
    return NULL;
}


void    OKParseOneFunctionBody( struct OKToken ** inToken, struct OKParseContext* context )
{
    OKStringBufferAppendFmt( &context->sourceString, "\tint\t__returnValue = 0;\n" );
    
    while( (*inToken) && (*inToken)->indentLevel == 8 )
    {
        const char*     funcName = OKGetIdentifier(*inToken);
        if( funcName && strcmp( funcName, "var" ) == 0 )
        {
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKGoNextTokenSkippingComments(inToken);
            const char*     varType = OKGetIdentifier(*inToken);
            if( !varType )
            {
                fprintf( stderr, "error:%d: Expected identifier with variable type after 'var'.\n", (*inToken)->lineNumber );
                *inToken = NULL;
                return;
            }
            OKGoNextTokenSkippingComments(inToken);
            const char*     varName = OKGetIdentifier(*inToken);
            if( varName )
            {
                char    varTypeWithRef[256] = {0};
                strncpy(varTypeWithRef +1,varType,sizeof(varTypeWithRef)-2);
                varTypeWithRef[0] = '*';
                char    varNameWithRef[256] = {0};
                strncpy(varNameWithRef +1,varName,sizeof(varNameWithRef)-2);
                varNameWithRef[0] = '&';
                OKMapAddEntry( context->currentLocalVars, varName, (void*)varTypeWithRef );
                
                OKStringBufferAppendFmt( &context->sourceString, "\tstruct %s   %s = {(struct object_isa*)&%s___isa};\n", varType, varName, varType );
                OKPrintInitCode( varNameWithRef, varType, context );
                
                OKGoNextTokenSkippingComments(inToken);
            }
            else
            {
                fprintf( stderr, "error:%d: Expected identifier with variable name after 'var %s'.\n", (*inToken)->lineNumber, varType );
                *inToken = NULL;
                return;
            }
        }
        else if( funcName )  // Had an identifier here?
        {
            const char*     objectVariable = NULL;
            
            OKGoNextTokenSkippingComments(inToken);
            
            if( OKIsOperator( *inToken, ".") )  // Method call!
            {
                objectVariable = funcName;
                
                OKGoNextTokenSkippingComments(inToken);
                funcName = OKGetIdentifier(*inToken);
                
                if( !funcName )
                {
                    fprintf( stderr, "error:%d: Expected a function name after a '.' operator.\n", (*inToken)->lineNumber );
                    *inToken = NULL;
                    return;
                }
                OKGoNextTokenSkippingComments(inToken);
            }
            
            if( OKIsOperator( *inToken, "(") )  // Function call!
            {
                struct OKMap*   classMethods = OKMapFindEntry( context->classes, context->className );
                if( !classMethods )
                {
                    fprintf( stderr, "error:%d: Couldn't find metadata for class '%s' (internal compiler error).\n", (*inToken)->lineNumber, context->className );
                    *inToken = NULL;
                    return;
                }
                
                if( !context->suppressLineDirectives )
                    OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
                char*   methodName = OKMapFindEntry( classMethods, funcName );
                if( funcName && objectVariable )
                {
                    const char*     varTypeWithRef = (const char*) OKMapFindEntry( context->currentLocalVars, objectVariable );
                    bool            isLocal = varTypeWithRef[0] == '*';
                    const char*     varType = isLocal ? (varTypeWithRef +1) : varTypeWithRef;
                    const char*     methodClass = OKFindClassForMethodOfClass( funcName, varType, context );
                    if( methodClass == NULL )
                    {
                        fprintf( stderr, "error:%d: No method '%s' found in class '%s' or its superclasses.\n", (*inToken)->lineNumber, funcName, varType );
                        *inToken = NULL;
                        return;
                    }
                    
                    OKStringBufferAppendFmt( &context->sourceString, "\t((struct %s_isa*)((struct object*)%s%s)->isa)->%s( (struct %s*) %s%s", methodClass, isLocal?"&":"", objectVariable, funcName, methodClass, isLocal?"&":"", objectVariable );
                }
                else if( methodName )    // Unqualified virtual method of ourselves!
                {
                    OKStringBufferAppendFmt( &context->sourceString, "\t((struct %s_isa*)((struct object*)this)->isa)->%s( this", context->className, funcName );
                }
                else if( strcmp(funcName,"return") == 0 )
                {
                    OKStringBufferAppend( &context->sourceString, "\t__returnValue = " );
                    OKGoNextTokenSkippingComments(inToken);
                    OKParseOneExpression( inToken, ")", context );
                    OKStringBufferAppend( &context->sourceString, ";\n\tgoto cleanup;\n" );
                    if( !OKIsOperator( *inToken, ")") )
                    {
                        fprintf( stderr, "error:%d: Expected ')' here, found '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
                        *inToken = NULL;
                        return;
                    }
                    OKGoNextTokenSkippingComments(inToken);
                    continue;
                }
                else    // Some standalone function or language construct.
                    OKStringBufferAppendFmt( &context->sourceString, "\t%s( ", funcName );
                
                bool    needsComma = (methodName != NULL || (objectVariable != NULL && funcName != NULL));
                OKGoNextTokenSkippingComments(inToken);
                while( *inToken )
                {
                    if( OKIsOperator( *inToken, ")") )
                    {
                        break;
                    }

                    if( needsComma )
                        OKStringBufferAppend( &context->sourceString, ", " );
                    OKParseOneExpression( inToken, ",", context );
                    if( !(*inToken) )
                        return;
                    
                    if( OKIsOperator( *inToken, ")") )
                    {
                        break;
                    }
                    else if( !OKIsOperator( *inToken, ",") )
                    {
                        fprintf( stderr, "error:%d: Expected ',' or ')' here, found '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
                        *inToken = NULL;
                        return;
                    }
                    OKGoNextTokenSkippingComments(inToken);
                    needsComma = true;
                }
                if( !OKIsOperator( *inToken, ")") )
                {
                    fprintf( stderr, "error:%d: Expected ')' here, found '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
                    *inToken = NULL;
                    return;
                }
                OKGoNextTokenSkippingComments(inToken);
                OKStringBufferAppend( &context->sourceString, " );\n" );
            }
            else
            {
                fprintf( stderr, "error:%d: Unknown statement starting with '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
                *inToken = NULL;
                return;
            }
        }
        else if( OKIsLineBreak( *inToken ) )
            OKGoNextTokenSkippingComments(inToken);
        else
        {
            fprintf( stderr, "error:%d: Unknown statement starting with '%s'.\n", (*inToken)->lineNumber, (*inToken) ? (*inToken)->string : "end of file" );
            *inToken = NULL;
            return;
        }
    }
    
    OKStringBufferAppendFmt( &context->sourceString, "cleanup:\n" );
    for( size_t x = 0; x < context->currentLocalVars->count; x++ )
    {
        const char* varTypeWithRef = context->currentLocalVars->entries[x].value;
        bool        isLocal = varTypeWithRef[0] == '*';
        
        if( isLocal )
        {
            OKStringBufferAppendFmt( &context->sourceString, "\t((struct object*)%s%s)->isa->dealloc( (struct object*)%s%s );\n", isLocal?"&":"", context->currentLocalVars->entries[x].key, isLocal?"&":"", context->currentLocalVars->entries[x].key );
        }
    }
    OKStringBufferAppendFmt( &context->sourceString, "\treturn __returnValue;\n" );
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

            struct OKMap*   classMethods = OKMapFindEntry( context->classes, context->className );
            if( !classMethods )
            {
                fprintf( stderr, "error:%d: Couldn't find metadata for class '%s' (internal compiler error).\n", (*inToken)->lineNumber, context->className );
                *inToken = NULL;
                return;
            }
            
            struct OKStringBuffer paramListString = {0};
            char    internalName[1024] = {0};
            snprintf( internalName, sizeof(internalName) -1, "%s___%s", context->className, funcName );
            
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
            OKPrintInitCode( "&this", context->className, context );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "\tint result = ((struct %s_isa*) ((struct object*)&this)->isa)->%s( &this );\n", context->className, funcName );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "\tint (*%s)( struct %s * this );\n", funcName, context->className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableSourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "\t%s___%s,\n", context->className, funcName );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKPrintDeallocCode( "&this", context );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "\treturn result;\n}\n\n" );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "int    %s( struct %s* this", internalName, context->className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->sourceString, "int    %s( struct %s* this", internalName, context->className );
            OKStringBufferAppendFmt( &paramListString, "struct %s* this", context->className );
            OKGoNextTokenSkippingComments( inToken );

            context->currentLocalVars = OKMallocStringToStringMap();
            OKMapAddEntry( context->currentLocalVars, "this", (void*)context->className );
            
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
                    if( !context->suppressLineDirectives )
                        OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
                    OKStringBufferAppendFmt( &context->headerString, ", %s %s", typeName, paramName );
                    if( !context->suppressLineDirectives )
                        OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
                    OKStringBufferAppendFmt( &context->sourceString, ", struct %s * %s", typeName, paramName );
                    OKStringBufferAppendFmt( &paramListString, ", struct %s * %s", typeName, paramName );
                    OKGoNextTokenSkippingComments( inToken );
                    
                    OKMapAddEntry( context->currentLocalVars, paramName, (void*)typeName );
                }
                else
                {
                    OKStringBufferAppendFmt( &context->headerString, ", struct %s *", typeName );
                    OKStringBufferAppendFmt( &context->sourceString, ", struct %s *", typeName );
                    OKStringBufferAppendFmt( &paramListString, ", struct %s *", typeName );
                }
            }
            if( !OKMapAddEntry( classMethods, funcName, paramListString.string ) )
            {
                fprintf( stderr, "error:%d: Out of memory trying to add main method entry for '%s' in class '%s'.\n", (*inToken)->lineNumber, funcName, context->className );
                *inToken = NULL;
                OKStringBufferFree( &paramListString );
                return;
            }
            OKStringBufferFree( &paramListString );
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
            OKMapFree( context->currentLocalVars );
            context->currentLocalVars = NULL;
            
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

        struct OKMap*   classMethods = OKMapFindEntry( context->classes, context->className );
        if( !classMethods )
        {
            fprintf( stderr, "error:%d: Couldn't find metadata for class '%s' (internal compiler error).\n", (*inToken)->lineNumber, context->className );
            *inToken = NULL;
            return;
        }
        
        struct  OKStringBuffer  paramListString = {0};
        char    internalName[1024] = {0};
        snprintf( internalName, sizeof(internalName) -1, "%s___%s", context->className, funcName );
        
        OKGoNextTokenSkippingComments( inToken );   // Skip name.
        if( !OKIsOperator( *inToken, "(") )
        {
            fprintf( stderr, "error:%d: Expected '(' after method name '%s' in class '%s'.\n", (*inToken)->lineNumber, funcName, context->className );
            *inToken = NULL;
            return;
        }
        
        if( !context->suppressLineDirectives )
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        OKStringBufferAppendFmt( &context->currentVTableHeaderString, "\tint (*%s)( struct %s * this", funcName, context->className );
        
        if( !context->suppressLineDirectives )
            OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        OKStringBufferAppendFmt( &context->headerString, "int    %s( struct %s* this", internalName, context->className);
        if( !context->suppressLineDirectives )
            OKStringBufferAppendFmt( &context->sourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        OKStringBufferAppendFmt( &context->sourceString, "int    %s( struct %s* this", internalName, context->className);
        OKStringBufferAppendFmt( &paramListString, "struct %s* this", context->className);
        if( !context->suppressLineDirectives )
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
        OKStringBufferAppendFmt( &context->currentVTableSourceString, "\t%s,\n", internalName );
        
        context->currentLocalVars = OKMallocStringToStringMap();
        OKMapAddEntry( context->currentLocalVars, "this", (void*)context->className );
            
        while( OKIsOperator( *inToken, "(") || OKIsOperator( *inToken, ",") || OKIsOperator( *inToken, ")") )
        {
            if( OKIsOperator( *inToken, "(") )
                OKGoNextTokenSkippingComments( inToken );
            
            if( OKIsOperator( *inToken, ")") )
            {
                OKGoNextTokenSkippingComments( inToken );
                break;
            }
            else if( OKIsOperator( *inToken, ",") )
            {
                OKGoNextTokenSkippingComments( inToken );
            }

            const char* typeName = OKGetIdentifier(*inToken);
            const char* paramName = NULL;
            OKGoNextTokenSkippingComments( inToken );
            if( !OKIsOperator( *inToken, ",") && !OKIsOperator( *inToken, ")") )
            {
                paramName = OKGetIdentifier(*inToken);
                OKStringBufferAppendFmt( &context->headerString, ", struct %s * %s", typeName, paramName );
                OKStringBufferAppendFmt( &context->sourceString, ", struct %s * %s", typeName, paramName );
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, ", struct %s * %s", typeName, paramName );
                OKStringBufferAppendFmt( &paramListString, ", struct %s * %s", typeName, paramName );
                OKGoNextTokenSkippingComments( inToken );

                OKMapAddEntry( context->currentLocalVars, paramName, (void*)typeName );
            }
            else
            {
                OKStringBufferAppendFmt( &context->headerString, ", struct %s *", typeName );
                OKStringBufferAppendFmt( &context->sourceString, ", struct %s *", typeName );
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, ", struct %s *", typeName );
                OKStringBufferAppendFmt( &paramListString, ", struct %s *", typeName );
            }
        }
        OKStringBufferAppendFmt( &context->headerString, " );\n");
        OKStringBufferAppendFmt( &context->sourceString, " )\n{\n");
        OKStringBufferAppend( &context->currentVTableHeaderString, " );\n" );
        
        if( !OKMapAddEntry( classMethods, funcName, paramListString.string ) )
        {
            fprintf( stderr, "error:%d: Out of memory trying to add main method entry for '%s' in class '%s'.\n", (*inToken)->lineNumber, funcName, context->className );
            *inToken = NULL;
            OKStringBufferFree( &paramListString );
            return;
        }
        OKStringBufferFree( &paramListString );

        if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
        {
            OKGoNextTokenSkippingComments(inToken);
        }
        OKParseOneFunctionBody( inToken, context );
        OKStringBufferAppendFmt( &context->sourceString, "}\n\n");
        OKMapFree( context->currentLocalVars );
        context->currentLocalVars = NULL;

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
                OKGoNextTokenSkippingComments( inToken );
            }
            
            struct OKMap*   methods = OKMallocStringToStringMap();
            if( !OKMapAddEntry( context->classes, className, methods ) )
            {
                fprintf( stderr, "error:%d: Out of memory trying to add entry for class '%s'.\n", (*inToken)->lineNumber, className );
                *inToken = NULL;
                return;
            }
            
            OKMapAddEntry( methods, ":superclass", (void*)superclassName );
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "struct %s\n{\n", className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "\tstruct %s\tsuper;\n", superclassName );
            OKStringBufferAppendFmt( &context->headerString, "};\n\n" );
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "struct %s_isa\n{\n", className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "\tstruct %s_isa\tsuper;\n", superclassName );
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableSourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "struct %s_isa   %s___isa =\n{\n", className, className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableSourceString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableSourceString, "\t{0}, // Set to %s___isa by %s___init_isa().\n", superclassName, className );
            
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = className;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
                if( !(*inToken) )
                    break;
            }
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "};\n\n" );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)?(*inToken)->lineNumber:0, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "extern struct %s_isa   %s___isa;\n", className, className );
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->currentVTableHeaderString, "#line %d \"%s\"\n", (*inToken)?(*inToken)->lineNumber:0, context->fileName );
            OKStringBufferAppendFmt( &context->currentVTableHeaderString, "void %s___init_isa( void );\n\n", className );
            OKStringBufferAppend( &context->headerString, context->currentVTableHeaderString.string );
            //printf("\n%s\n",context->headerString.string );
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
                fprintf( stderr, "error:%d: Extension %s does not specify the class it extends.\n", (*inToken)->lineNumber, className );
                *inToken = NULL;
                return;
            }
            OKGoNextTokenSkippingComments( inToken );
            superclassName = OKGetIdentifier(*inToken);
            if( !superclassName )
            {
                fprintf( stderr, "error:%d: Extension %s does not specify the class it extends.\n", (*inToken)->lineNumber, className );
                *inToken = NULL;
                return;
            }
            OKGoNextTokenSkippingComments( inToken );
            
            struct OKMap*   classMethods = OKMapFindEntry( context->classes, superclassName );
            if( !classMethods )
            {
                fprintf( stderr, "error:%d: Unknown class '%s' to extend with '%s'.\n", (*inToken)->lineNumber, superclassName, className );
                *inToken = NULL;
                return;
            }
            
            if( !context->suppressLineDirectives )
                OKStringBufferAppendFmt( &context->headerString, "#line %d \"%s\"\n", (*inToken)->lineNumber, context->fileName );
            OKStringBufferAppendFmt( &context->headerString, "// Extension %s to class %s\n", className, superclassName );
            if( (*inToken) && (*inToken)->tokenType == OKTokenMode_LineBreak )
                OKGoNextTokenSkippingComments( inToken );
            context->className = superclassName;
            while( (*inToken) && (*inToken)->indentLevel == 4 )
            {
                OKParseOneClassLevelConstruct( inToken, context );
                if( !(*inToken) )
                    break;
            }
        }
    }
    else if( *inToken )
    {
        fprintf( stderr, "error:%d: Unknown top-level construct %s.\n", (*inToken)->lineNumber, (*inToken)->string );
        *inToken = NULL;
    }
}


void    OKParseTokenList( struct OKToken * inToken, struct OKParseContext* context )
{
    context->classes = OKMallocStringToMapMap();
    struct OKMap*   objectMethods = OKMallocStringToStringMap();
    OKMapAddEntry( objectMethods, "init", "struct object* this" );
    OKMapAddEntry( objectMethods, "dealloc", "struct object* this" );
    OKMapAddEntry( context->classes, "object", objectMethods );
    struct OKMap*   stringMethods = OKMallocStringToStringMap();
    OKMapAddEntry( stringMethods, ":superclass", "object" );
    OKMapAddEntry( stringMethods, "print", "struct string* this" );
    OKMapAddEntry( stringMethods, "append", "struct string* this, struct string* addThis" );
    OKMapAddEntry( context->classes, "string", stringMethods );
    
    struct OKToken *    currToken = inToken;
    while( currToken )
    {
        OKParseOneTopLevelConstruct( &currToken, context );
    }
}


void    OKParserPrintParseContext( struct OKParseContext* context )
{
    printf( "File:\t%s\nClass:\t%s\n", context->fileName, context->className );
    printf( "Classes:\n" );
    for( size_t x = 0; x < context->classes->count; x++ )
    {
        printf( "\t%s:\n", context->classes->entries[x].key );
        struct OKMap    *   currClass = context->classes->entries[x].value;
        for( size_t y = 0; y < currClass->count; y++ )
        {
            printf( "\t\t%s (%s)\n", currClass->entries[y].key, (char*)currClass->entries[y].value );
        }
    }
}


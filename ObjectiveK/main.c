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
#if 0
    OKPrintTokenList( tokenList );
#endif
    
    // Write out header for the basic structure of an object:
    char                        objectHeaderFilePath[MAXPATHLEN] = {0};
    strncpy( objectHeaderFilePath, argv[1], sizeof(objectHeaderFilePath)-1 );
    for( int x = (int)strlen(objectHeaderFilePath) -1; x >= 0; x-- )
    {
        if( objectHeaderFilePath[x] == '/' )
        {
            objectHeaderFilePath[x+1] = 0;
            break;
        }
    }
    char                        objectFilePath[MAXPATHLEN] = {0};
    strncpy( objectFilePath, objectHeaderFilePath, sizeof(objectFilePath)-1 );
    strncat( objectHeaderFilePath, "ok_object.h", sizeof(objectHeaderFilePath)-1 );
    
    FILE*   objectHeaderFile = fopen( objectHeaderFilePath, "w");
    fprintf( objectHeaderFile, "//\n//  Header auto-generated from %s\n", argv[1] );
    fprintf( objectHeaderFile, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n\n" );
    fprintf( objectHeaderFile, "#include <stdlib.h>\n" );
    fprintf( objectHeaderFile, "#include <stdbool.h>\n\n" );
    fprintf( objectHeaderFile, "struct object\n{\n" );
    fprintf( objectHeaderFile, "\tstruct object_isa*    isa;\n" );
    fprintf( objectHeaderFile, "};\n\n" );
    fprintf( objectHeaderFile, "struct object_isa\n{\n" );
    fprintf( objectHeaderFile, "\tvoid\t(*init)( struct object* this );\n" );
    fprintf( objectHeaderFile, "\tvoid\t(*dealloc)( struct object* this );\n" );
    fprintf( objectHeaderFile, "};\n\n" );
    fprintf( objectHeaderFile, "// Instance methods (clients generally don't care about these):\n" );
    fprintf( objectHeaderFile, "extern void  object___init( struct object* this );\n" );
    fprintf( objectHeaderFile, "extern void  object___dealloc( struct object* this );\n\n" );
    fprintf( objectHeaderFile, "extern void  object___init_isa( void );\n\n" );
     fprintf( objectHeaderFile, "// vtable for plain objects:\n" );
    fprintf( objectHeaderFile, "extern struct object_isa\tobject___isa;\n\n" );
    fprintf( objectHeaderFile, "// built-in string class:\n" );
    fprintf( objectHeaderFile, "struct string\n{\n\tstruct object\t\tsuper;\n\tsize_t\t\t\t\tstringLength;\n\tconst char*\t\t\tstringBuffer;\n\tbool\t\t\t\tisMutable;\n};\n\n" );
    fprintf( objectHeaderFile, "struct string_isa\n{\n" );
    fprintf( objectHeaderFile, "\tstruct object_isa super;\n" );
    fprintf( objectHeaderFile, "\tvoid\t(*print)( struct string* this );\n" );
    fprintf( objectHeaderFile, "\tvoid\t(*append)( struct string* this, struct string* other );\n" );
    fprintf( objectHeaderFile, "};\n\n" );
    fprintf( objectHeaderFile, "extern void string___print( struct string* inText );\n" );
    fprintf( objectHeaderFile, "extern void string___append( struct string* this, struct string* other );\n" );
    fprintf( objectHeaderFile, "extern void string___init_isa( void );\n\n" );
    fprintf( objectHeaderFile, "extern struct string_isa\tstring___isa;\n\n" );
    fclose( objectHeaderFile );

    printf("Generated file \"%s\".\n",objectHeaderFilePath);

    // Write out source file for the basic structure of an object:
    strncat( objectFilePath, "ok_object.c", sizeof(objectFilePath)-1 );
    
    FILE*   objectFile = fopen( objectFilePath, "w");
    fprintf( objectFile, "//\n//  Source file auto-generated from %s\n", argv[1] );
    fprintf( objectFile, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n\n" );
    fprintf( objectFile, "#include \"ok_object.h\"\n" );
    fprintf( objectFile, "#include <stdio.h>\n" );
    fprintf( objectFile, "#include <stdarg.h>\n\n" );
    fprintf( objectFile, "#include <memory.h>\n\n" );
    fprintf( objectFile, "struct object_isa     object___isa =\n{\n" );
    fprintf( objectFile, "\tobject___init,\n" );
    fprintf( objectFile, "\tobject___dealloc\n" );
    fprintf( objectFile, "};\n\n" );
    fprintf( objectFile, "void  object___init( struct object* this )\n{\n" );
    fprintf( objectFile, "}\n\n" );
    fprintf( objectFile, "void  object___dealloc( struct object* this )\n{\n" );
    fprintf( objectFile, "}\n" );
    fprintf( objectFile, "void  object___init_isa( void )\n{\n" );
    fprintf( objectFile, "\t// Nothing to do for 'object' base class.\n" );
    fprintf( objectFile, "\t// But we'll init all other built-in classes.\n" );
    fprintf( objectFile, "\tstring___init_isa();\n" );
    fprintf( objectFile, "}\n\n" );
    fprintf( objectFile, "struct string_isa     string___isa =\n{\n" );
    fprintf( objectFile, "\t{0}, // set by string___init_isa().\n" );
    fprintf( objectFile, "\tstring___print,\n" );
    fprintf( objectFile, "\tstring___append\n" );
    fprintf( objectFile, "};\n\n" );
    fprintf( objectFile, "void  string___init( struct string* this )\n{\n" );
    fprintf( objectFile, "\tthis->stringLength = 0;\n" );
    fprintf( objectFile, "\tthis->stringBuffer = \"\";\n" );
    fprintf( objectFile, "\tthis->isMutable = false;\n" );
    fprintf( objectFile, "}\n\n" );
    fprintf( objectFile, "void  string___dealloc( struct string* this )\n{\n" );
    fprintf( objectFile, "\tif( this->isMutable )\n\t\tfree( (void*)this->stringBuffer);\n" );
    fprintf( objectFile, "}\n" );
    fprintf( objectFile, "void  string___init_isa( void )\n{\n" );
    fprintf( objectFile, "\tstring___isa.super = object___isa;\n" );
    fprintf( objectFile, "\tstring___isa.super.init = (void(*)(struct object*))string___init;\n" );
    fprintf( objectFile, "\tstring___isa.super.dealloc = (void(*)(struct object*))string___dealloc;\n" );
    fprintf( objectFile, "}\n\n" );
    fprintf( objectFile, "void string___print( struct string* inText )\n{\n\tprintf(\"%%s\",inText->stringBuffer);\n}\n" );
    fprintf( objectFile, "void string___append( struct string* this, struct string* other )\n{\n" );
    fprintf( objectFile, "\tchar* nuStr = malloc(this->stringLength +other->stringLength +1);\n\tmemcpy(nuStr, this->stringBuffer, this->stringLength);\n\tmemcpy(nuStr +this->stringLength, other->stringBuffer, other->stringLength +1);\n\tif( this->isMutable )\n\t\tfree(this->stringBuffer);\n\tthis->stringBuffer = nuStr;\n\tthis->stringLength += other->stringLength;\n\tthis->isMutable = true;\n" );
    fprintf( objectFile, "}\n" );
    fclose( objectFile );
    
    printf("Generated file \"%s\".\n",objectFilePath);
    
    // Create header and source file for C code generated from ObjK code:
    char                        headerFilePath[MAXPATHLEN] = {0};
    strncpy( headerFilePath, argv[1], sizeof(headerFilePath)-1 );
    strncat( headerFilePath, ".h", sizeof(headerFilePath)-1 );
    
    char                        sourceFilePath[MAXPATHLEN] = {0};
    strncpy( sourceFilePath, argv[1], sizeof(sourceFilePath)-1 );
    strncat( sourceFilePath, ".c", sizeof(sourceFilePath)-1 );
    
    struct OKParseContext       context = {0};
    context.fileName = argv[1];
    
#if 0
    context.suppressLineDirectives = true;
#endif
    
    OKStringBufferAppendFmt( &context.headerString, "//\n//  Header auto-generated from %s\n", argv[1] );
    OKStringBufferAppendFmt( &context.headerString, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n\n" );
    OKStringBufferAppendFmt( &context.headerString, "\n#include \"ok_object.h\"\n\n" );
    
    OKStringBufferAppendFmt( &context.constantsString, "//\n//  Source file auto-generated from %s\n", argv[1] );
    OKStringBufferAppendFmt( &context.constantsString, "//  using the objk command line tool. Do not modify, modify the original source file.\n//\n" );
    OKStringBufferAppendFmt( &context.constantsString, "\n#include \"%s\"\n\n", headerFilePath );
    
    OKParseTokenList( tokenList, &context );
    
    OKFreeTokenList( tokenList );

    FILE*   headerFile = fopen( headerFilePath, "w");
    fwrite( context.headerString.string, 1, context.headerString.stringLength -1, headerFile );
    fclose( headerFile );
    printf("Generated file \"%s\".\n",headerFilePath);
    
    FILE*   sourceFile = fopen( sourceFilePath, "w");
    fwrite( context.constantsString.string, 1, context.constantsString.stringLength -1, sourceFile );
    fwrite( context.sourceString.string, 1, context.sourceString.stringLength -1, sourceFile );
    fclose( sourceFile );
    printf("Generated file \"%s\".\n",sourceFilePath);
    
#if 0
    OKParserPrintParseContext( &context );
#endif
    
    OKStringBufferFree( &context.headerString );
    OKStringBufferFree( &context.sourceString );
    OKMapFree( context.classes );
    
    printf("Done.\n");

    return result;
}

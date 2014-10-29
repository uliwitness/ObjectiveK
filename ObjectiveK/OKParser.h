//
//  OKParser.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#ifndef __ObjectiveK__OKParser__
#define __ObjectiveK__OKParser__

#include <stdio.h>
#include "OKToken.h"


/*! Holds some state needed by the various parser methods, like the files to
    write to. */
struct OKParseContext
{
    const char*     fileName;   //! Path to the current file being parsed.
    const char*     className;  //! Name of the class currently being parsed (initialize to NULL).
    FILE*           headerFile; //! The C header file to write the header code to.
    FILE*           sourceFile; //! The C source file to write source code to.
};


/*! Parse the given token list and write out a C header and C source code
    file representing the program specified by the tokens. The caller is
    responsible for setting up the given context by creating and opening
    the header and source file and passing them in the context struct, and
    specifying the name of the file the tokens were parsed from. Initialize context.className to NULL. */
extern void    OKParseTokenList( struct OKToken * inToken, struct OKParseContext* context );


/*! Called by OKParseTokenList() to parse each top-level construct in the file (i.e. classes, class extensions etc.). */
extern void    OKParseOneTopLevelConstruct( struct OKToken ** inToken, struct OKParseContext* context );


/*! Called by OKParseTokenList() to parse the contents of each class or extension.
    context.className must be set to the name of the class currently being parsed. */
extern void    OKParseOneClassLevelConstruct( struct OKToken ** inToken, struct OKParseContext* context );


/*! Called by OKParseTokenList() to parse the body of each function.
    context.className must be set to the name of the class currently being parsed. */
extern void    OKParseOneFunctionBody( struct OKToken ** inToken, struct OKParseContext* context );


/*! Called by the parser to generate code for initializing an already-allocated struct for an object. varName is the name of a variable containing a pointer to the object (pass something like "&varName" for a stack variable). */
extern void    OKPrintInitCode( const char* varName, struct OKParseContext* context );


/*! Called by the parser to generate code for tearing down an already-initialized struct for an object before it is deallocated. varName is the name of a variable containing a pointer to the object (pass something like "&varName" for a stack variable). */
extern void    OKPrintDeallocCode( const char* varName, struct OKParseContext* context );


#endif /* defined(__ObjectiveK__OKParser__) */

//
//  OKTokenizer.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#ifndef __ObjectiveK__OKTokenizer__
#define __ObjectiveK__OKTokenizer__

#include "OKToken.h"
#include <stdbool.h>
#include <stdio.h>


//! Is the given character a valid operator? Used to decide when to end an identifier, and which characters to create operator tokens for.
extern bool OKIsOperatorChar( char currCh );


/*! Read text from file theFile until we hit the end. Generate tokens for each token we encounter
 and return the first item of the linked list of tokens thus generated in outTokenList.
 */
extern int  OKTokenize( FILE* theFile, struct OKToken ** outTokenList );


/*! Examines the token in *inToken and the one follwing it, and if it is a comment (e.g. a sequence of two / operators) skip ahead until the next line
    break token and return that in ioToken. May set *ioToken to NULL if there is no line break following the comment and we encounter the file's end.
 */
extern void OKSkipComments( struct OKToken ** ioToken );


extern void OKGoNextTokenSkippingComments( struct OKToken ** inToken );


#endif /* defined(__ObjectiveK__OKTokenizer__) */

//
//  OKToken.h
//  ObjectiveK
//
//  Created by Uli Kusterer on 2014-10-29.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#ifndef __ObjectiveK__OKToken__
#define __ObjectiveK__OKToken__

#include <stdlib.h>
#include <stdbool.h>


//! States of the parser, but also types of token we can generate.
enum OKTokenMode
{
    OKTokenMode_Whitespace, //!< Whitespace. We do not generate tokens of this type, parser state only.
    OKTokenMode_Identifier, //!< An unquoted identifier like 'foo'.
    OKTokenMode_Integer,    //!< An integral number, without any fractional part.
    OKTokenMode_Float,      //!< A floating point number, with a decimal point.
    OKTokenMode_String,     //!< A quoted string, which may span several lines and contain escape sequences like \n.
    OKTokenMode_Operator,   //!< A single-character operator. To parse things like comments, look for TWO / operators in a row. We do not eliminate comments before parsing to allow for HeaderDoc-like constructs in the future.
    OKTokenMode_LineBreak   //!< A \n, \r or \r\n sequence was encountered and a new line started.
};


struct OKToken
{
    enum OKTokenMode    tokenType;      //!< The type of token.
    int                 indentLevel;    //!< We count the whitespace at the start of each line (in spaces, where a tab == 4 spaces) and apply it to all tokens on that line, so we can tell what nesting level a line is on.
    int                 lineNumber;     //!< The line number at which this token was encountered in the file.
    struct OKToken*     nextToken;      //!< The next token in a token list (singly linked list).
    size_t              stringLen;      //!< The size of the string array.
    char                string[0];      //!< The text for this identifier or string (with escape sequences already substituted), the characters that make up this operator or line break, or a string representation of this integer/floating point number. The struct is allocated so that it exactly fits this string plus a terminating \0 byte, so you can in most cases just use it as a C string. However, there may be \0 bytes embedded in strings.
};


/*! Allocate a new token and append it to the linked list specified by prevToken. If *prevToken is NULL, set
    both *firstToken and *prevToken to the newly created token. Will not create a token if the token type is
    OKTokenMode_Whitespace or identifierLen == 0 and the tokenMode is not OKTokenMode_String (because that
    way we can just call this before starting a new token during parsing to end any previous one without having
    to worry if there *was* a previous one). */
extern struct OKToken *   OKMallocToken( struct OKToken ** firstToken, struct OKToken ** prevToken, enum OKTokenMode tokenMode, int indentLevel, int lineNumber, const char* identifierStr, size_t identifierLen );


/*! Free any memory occupied by this token. */
extern void    OKFreeToken( struct OKToken * inToken );


/*! Print a debug description of a given token. */
extern void    OKPrintToken( struct OKToken * theToken );


/*! Is the given token the operator whose string representation is given
    in operatorName. Returns FALSE if the token is not of type OKTokenMode_Operator
    or if the string representations don't match.
 
    inToken may be NULL. */
extern bool    OKIsOperator( struct OKToken * inToken, const char* operatorName );


/*! Is the given token a line break? Returns FALSE if the token is not of type
    OKTokenMode_LineBreak.
 
    inToken may be NULL. */
extern bool    OKIsLineBreak( struct OKToken * inToken );


/*! Is the given token the identifier whose string representation is given
    in operatorName? Returns FALSE if the token is not of type OKTokenMode_Identifier
    or if the string representations don't match.
    
    Used to verify a certain identifier is present in the token list.
 
    inToken may be NULL. */
extern bool    OKIsIdentifier( struct OKToken * inToken, const char* identifierName );


/*! If the given token is an identifier, return its string, otherwise return NULL.
 
    inToken may be NULL.   */
extern const char*    OKGetIdentifier( struct OKToken * inToken );


/*! If the given token is a string literal, return its string, otherwise return NULL.
 
    inToken may be NULL.   */
extern const char*    OKGetStringLiteral( struct OKToken * inToken );


/*! Free the given linked list of tokens (i.e. call OKFreeToken() on all of the tokens in the linked list from inToken to the end). */
extern void    OKFreeTokenList( struct OKToken * inToken );


/*! Print each token in the given linked list using OKPrintToken(), starting at inToken. */
extern void    OKPrintTokenList( struct OKToken * inToken );



#endif /* defined(__ObjectiveK__OKToken__) */

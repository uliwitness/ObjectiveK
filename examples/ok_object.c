//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "ok_object.h"
#include <stdio.h>
#include <stdarg.h>

struct object_isa     object___isa =
{
	object___init,
	object___dealloc
};

void  object___init( struct object* this )
{
}

void  object___dealloc( struct object* this )
{
}
void  object___init_isa( void )
{
	// Nothing to do for 'object' base class.
	// But we'll init all other built-in classes.
	string___init_isa();
}

struct string_isa     string___isa =
{
	{0}, // set by string___init_isa().
	string___print
};

void  string___init_isa( void )
{
	string___isa.super = object___isa;
}

void string___print( struct string* inFmt, ... )
{
	va_list argl;
	va_start(argl,inFmt);
	vprintf(inFmt->stringBuffer,argl);
	va_end(argl);
}

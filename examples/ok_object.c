//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "ok_object.h"
#include <stdio.h>
#include <stdarg.h>

#include <memory.h>

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
	string___print,
	string___append
};

void  string___init( struct string* this )
{
	this->stringLength = 0;
	this->stringBuffer = "";
	this->isMutable = false;
}

void  string___dealloc( struct string* this )
{
	if( this->isMutable )
		free(this->stringBuffer);
}
void  string___init_isa( void )
{
	string___isa.super = object___isa;
	string___isa.super.init = (void(*)(struct object*))string___init;
	string___isa.super.dealloc = (void(*)(struct object*))string___dealloc;
}

void string___print( struct string* inText )
{
	printf("%s",inText->stringBuffer);
}
void string___append( struct string* this, struct string* other )
{
	char* nuStr = malloc(this->stringLength +other->stringLength +1);
	memcpy(nuStr, this->stringBuffer, this->stringLength);
	memcpy(nuStr +this->stringLength, other->stringBuffer, other->stringLength +1);
	if( this->isMutable )
		free(this->stringBuffer);
	this->stringBuffer = nuStr;
	this->stringLength += other->stringLength;
	this->isMutable = true;
}

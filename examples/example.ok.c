//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "/Users/uli/Programming/ObjectiveK/examples/example.ok.h"

struct string kString___1 = { (struct object_isa*) &string___isa, 5, "Huh.\n", false };
struct string kString___2 = { (struct object_isa*) &string___isa, 13, "Hello World!\n", false };
struct string kString___3 = { (struct object_isa*) &string___isa, 25, "And Greetings, Programs!\n", false };
struct string kString___4 = { (struct object_isa*) &string___isa, 34, "We can call methods on ourselves!\n", false };
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    ulistring___playMusic( struct ulistring* this )
{
	int	__returnValue = 0;
#line 11 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)this)->isa)->print( this );
cleanup:
	return __returnValue;
}

#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct ulistring_isa   ulistring___isa =
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	{0}, // Set to string___isa by ulistring___init_isa().
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	ulistring___playMusic,
};

void ulistring___init_isa( void )
{
	string___init_isa();
	ulistring___isa.super = string___isa;
};

#line 14 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___showtext( struct program* this, struct string * inText )
{
	int	__returnValue = 0;
#line 15 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct ulistring   joe = {(struct object_isa*)&ulistring___isa};
	((struct object*)&joe)->isa->init( (struct object*)&joe );
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)&joe)->isa)->append( &joe, &kString___1 );
#line 17 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct ulistring_isa*)((struct object*)&joe)->isa)->playMusic( &joe );
#line 18 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)inText)->isa)->print( inText );
#line 19 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	__returnValue = 0;
	goto cleanup;
cleanup:
	((struct object*)&joe)->isa->dealloc( (struct object*)&joe );
	return __returnValue;
}

#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    main( int argc, const char** argv )
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
{
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct program this = {(struct object_isa*)&program___isa};
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___init_isa();
	((struct object*)&this)->isa->init( (struct object*)&this );
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int result = ((struct program_isa*) ((struct object*)&this)->isa)->startUp( &this );
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct object*)&this)->isa->dealloc( (struct object*)&this );
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	return result;
}

#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___startUp( struct program* this )
{
	int	__returnValue = 0;
#line 22 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	string___print( &kString___2 );
#line 23 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct program_isa*)((struct object*)this)->isa)->showtext( this, &kString___3 );
#line 24 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct program_isa*)((struct object*)this)->isa)->showtext( this, &kString___4 );
#line 25 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	__returnValue = 0;
	goto cleanup;
cleanup:
	return __returnValue;
}

#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program_isa   program___isa =
{
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	{0}, // Set to object___isa by program___init_isa().
#line 14 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___showtext,
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___startUp,
};

void program___init_isa( void )
{
	object___init_isa();
	program___isa.super = object___isa;
};


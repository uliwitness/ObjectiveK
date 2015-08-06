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
int    program___showtext( struct program* this, struct string * inText )
{
	int	__returnValue = 0;
#line 11 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct string   joe = {(struct object_isa*)&string___isa};
	((struct object*)&joe)->isa->init( (struct object*)&joe );
#line 12 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)&joe)->isa)->append( &joe, &kString___1 );
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)&joe)->isa)->print( &joe );
#line 14 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct string_isa*)((struct object*)inText)->isa)->print( inText );
#line 15 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	__returnValue = 0;
	goto cleanup;
cleanup:
	((struct object*)this)->isa->dealloc( (struct object*)this );
	((struct object*)inText)->isa->dealloc( (struct object*)inText );
	((struct object*)&joe)->isa->dealloc( (struct object*)&joe );
	return __returnValue;
}

#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    main( int argc, const char** argv )
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
{
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct program this = {(struct object_isa*)&program___isa};
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___init_isa();
	((struct object*)&this)->isa->init( (struct object*)&this );
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int result = ((struct program_isa*) ((struct object*)&this)->isa)->startUp( &this );
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct object*)&this)->isa->dealloc( (struct object*)&this );
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	return result;
}

#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___startUp( struct program* this )
{
	int	__returnValue = 0;
#line 17 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	string___print( &kString___2 );
#line 18 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct program_isa*)((struct object*)this)->isa)->showtext( this, &kString___3 );
#line 19 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct program_isa*)((struct object*)this)->isa)->showtext( this, &kString___4 );
#line 20 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	__returnValue = 0;
	goto cleanup;
cleanup:
	((struct object*)this)->isa->dealloc( (struct object*)this );
	return __returnValue;
}

#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program_isa   program___isa =
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	{0}, // Set to object___isa by program___init_isa().
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___showtext,
#line 16 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___startUp,
};

void program___init_isa( void )
{
	object___init_isa();
	program___isa.super = object___isa;
};


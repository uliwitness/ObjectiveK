//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "/Users/uli/Programming/ObjectiveK/examples/example.ok.h"

#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    main( int argc, const char** argv )
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
{
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct program this = {(struct object_isa*)&program___isa};
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___init_isa();
	((struct object*)&this)->isa->init( (struct object*)&this );
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int result = ((struct program_isa*) ((struct object*)&this)->isa)->startUp( &this );
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	((struct object*)&this)->isa->dealloc( (struct object*)&this );
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	return result;
}

#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___startUp( struct program* this )
{
#line 11 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	printf("Hello World!\n");
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	return(0);
}

#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program_isa   program___isa =
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	{0}, // Set to object___isa by program___init_isa().
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	program___startUp,
};

void program___init_isa( void )
{
	object___init_isa();
	program___isa.super = object___isa;
};


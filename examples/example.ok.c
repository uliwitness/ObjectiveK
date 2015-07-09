//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "/Users/uli/Programming/ObjectiveK/examples/example.ok.h"

int    main( int argc, const char** argv )
{
	struct program this = {(struct object_isa*)&program___isa};
	program___init_isa();
	((struct object*)&this)->isa->init( (struct object*)&this );
	int result = ((struct program_isa*) ((struct object*)&this)->isa)->startUp( &this );
	((struct object*)&this)->isa->dealloc( (struct object*)&this );
	return result;
}

int    program___startUp( struct program* this )
{
	printf("Hello World!\n");
	showtext("And Greetings, Programs!\n");
	return 0;
}

struct program_isa   program___isa =
{
	{0}, // Set to object___isa by program___init_isa().
	program___startUp,
};

void program___init_isa( void )
{
	object___init_isa();
	program___isa.super = object___isa;
};


//
//  Header auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//


#include "ok_object.h"

struct program
{
	struct object	super;
};

int    program___startUp( struct program* this );
struct program_isa
{
	struct object_isa	super;
	int (*startUp)( struct program * this );
};

extern struct program_isa   program___isa;
void program___init_isa( void );


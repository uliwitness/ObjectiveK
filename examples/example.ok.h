//
//  Header auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//


#include "ok_object.h"

#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct object	super;
};

#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___showtext( struct program* this );
#line 12 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___startUp( struct program* this );
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program_isa
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct object_isa	super;
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int (*showtext)( struct program * this );
#line 12 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int (*startUp)( struct program * this );
};

#line 18 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
extern struct program_isa   program___isa;
#line 18 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
void program___init_isa( void );


//
//  Header auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//


#include "ok_object.h"

#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct ulistring
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct string	super;
};

#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    ulistring___playMusic( struct ulistring* this );
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct ulistring_isa
{
#line 9 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct string_isa	super;
#line 10 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int (*playMusic)( struct ulistring * this );
};

#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
extern struct ulistring_isa   ulistring___isa;
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
void ulistring___init_isa( void );

#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program
{
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct object	super;
};

#line 14 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___showtext( struct program* this, struct string * inText );
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
int    program___startUp( struct program* this );
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
struct program_isa
{
#line 13 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	struct object_isa	super;
#line 14 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int (*showtext)( struct program * this, struct string * inText );
#line 21 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
	int (*startUp)( struct program * this );
};

#line 27 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
extern struct program_isa   program___isa;
#line 27 "/Users/uli/Programming/ObjectiveK/examples/example.ok"
void program___init_isa( void );


//
//  Source file auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include "/Users/uli/Programming/ObjectiveK/examples/example.ok.h"

struct string kString___1 = { (struct object_isa*) &string___isa, 5, "Huh.\n", false };
struct string kString___2 = { (struct object_isa*) &string___isa, 13, "Hello World!\n", false };
struct string kString___3 = { (struct object_isa*) &string___isa, 25, "And Greetings, Programs!\n", false };
int    program___showtext( struct program* this, struct string * inText )
{
	int	__returnValue = 0;
	struct string   joe = {(struct object_isa*)&string___isa};
	((struct object*)&joe)->isa->init( (struct object*)&joe );
	((struct string_isa*)((struct object*)&joe)->isa)->append( &joe, &kString___1 );
	((struct string_isa*)((struct object*)&joe)->isa)->print( &joe );
	((struct string_isa*)((struct object*)inText)->isa)->print( inText );
	__returnValue = 0;
	goto cleanup;
cleanup:
	((struct object*)this)->isa->dealloc( (struct object*)this );
	((struct object*)inText)->isa->dealloc( (struct object*)inText );
	((struct object*)&joe)->isa->dealloc( (struct object*)&joe );
	return __returnValue;
}

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
	int	__returnValue = 0;
	string___print( &kString___2 );
	((struct program_isa*)((struct object*)this)->isa)->showtext( this, &kString___3 );
	__returnValue = 0;
	goto cleanup;
cleanup:
	((struct object*)this)->isa->dealloc( (struct object*)this );
	return __returnValue;
}

struct program_isa   program___isa =
{
	{0}, // Set to object___isa by program___init_isa().
	program___showtext,
	program___startUp,
};

void program___init_isa( void )
{
	object___init_isa();
	program___isa.super = object___isa;
};


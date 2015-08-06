//
//  Header auto-generated from /Users/uli/Programming/ObjectiveK/examples/example.ok
//  using the objk command line tool. Do not modify, modify the original source file.
//

#include <stdlib.h>
#include <stdbool.h>

struct object
{
	struct object_isa*    isa;
};

struct object_isa
{
	void	(*init)( struct object* this );
	void	(*dealloc)( struct object* this );
};

// Instance methods (clients generally don't care about these):
extern void  object___init( struct object* this );
extern void  object___dealloc( struct object* this );

extern void  object___init_isa( void );

// vtable for plain objects:
extern struct object_isa	object___isa;

// built-in string class:
struct string
{
	struct object		super;
	size_t				stringLength;
	const char*			stringBuffer;
	bool				isMutable;
};

struct string_isa
{
	struct object_isa super;
	void	(*print)( struct string* this );
	void	(*append)( struct string* this, struct string* other );
};

extern void string___print( struct string* inText );
extern void string___append( struct string* this, struct string* other );
extern void string___init_isa( void );

extern struct string_isa	string___isa;


A simple experimental programming language, implemented as a preprocessor that takes
ObjectiveK source code and generates C source code from it.

Design goals for ObjectiveK
---------------------------

1. Provide object-oriented coding mechanisms but be fast by using vtables
2. Use a C-like syntax, but prefer clear identifiers to cryptic symbols
3. Avoid the complexity of C++ and Swift
4. Use indentation to indicate nesting, instead of possibly misleading brackets
5. Be as lightweight and portable as C, by sitting on top of C


License
-------

    Copyright 2014 by Uli Kusterer.

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.

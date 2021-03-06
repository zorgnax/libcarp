Emulates Perl's Carp module
http://search.cpan.org/~dapm/perl-5.10.1/lib/Carp.pm as best as possible to
warn or die of errors in programs written in C.

The following macros are defined for use:

-   warn - prints message with the file and line
-   die - like a warn that exits the program
-   carp - warn of errors (from perspective of caller)
-   croak - die of errors (from perspective of caller)
-   cluck - warn of errors with stack backtrace
-   confess - die of errors with stack backtrace

There are also the macros ewarn, edie, ecarp, ecroak, ecluck, econfess which
work just like the previous macros but add an errno string to your message.

There are also the macros swarn, sdie, scarp, scroak, scluck, sconfess, eswarn,
esdie, escroak, escluck, and esconfess that return the string the original
macro would have outputted. The s* variants will not exit the program. The
returned string will need to be freed.

SYNOPSIS
========

    #include <carp.h>
    #line 1 "libb.c"
    void foo4 () {
        warn("warn");
        carp("carp");
        cluck("cluck");
    }
    void foo3 () {foo4();}
    #line 1 "liba.c"
    void foo2 () {foo3();}
    void foo1 () {foo2();}
    void bar2 () {
        warn("warn");
        carp("carp");
        confess("confess");
        croak("croak");
    }
    void bar1 () {bar2();}
    #line 1 "test1.c"
    int main () {
        foo1();
        bar1();
        return 0;
    }

Results in:

    warn at libb.c line 2
    carp at libb.c line 3
    cluck at libb.c line 4
        foo4() called at libb.c line 6
        foo3() called at liba.c line 1
        foo2() called at liba.c line 2
        foo1() called at test1.c line 2
    warn at liba.c line 4
    carp at liba.c line 5
    confess at liba.c line 6
        bar2() called at liba.c line 9
        bar1() called at test1.c line 3


DESCRIPTION
===========

This library works by reading from a an attached gdb process. If unable to
get info from a stack trace, carp and cluck revert to a warn and croak and
confess revert to a die. However, even with debugging (gcc's -g option) 
disabled requesting a stacktrace may provide some useful information. It uses 
files instead of packages (as Perl does) to determine the trust divide.

On Windows, the stack trace is recieved by walking the stack, and getting
return addresses, which are looked up in the pdbs if they are available.

When you call carp or croak, this library will print the first location it
finds in a different shared library. Static libraries (like libyup.a or
yup.lib) are not capable from a debugging standpoint to be differentiated from
normal object files.

Carp in C differs slightly from the Carp module in Perl: Since it may be common
for a program written in C to link with a static library that uses Carp, I
decided not to default to a full stack trace when a trust boundary isn't found.
Rather a carp will become like a warn.


CONFIGURATION
=============

This library can be configured at runtime either by setting environment
variables or by using the carp_set() function. Each setting uses a string
identifier and an associated data type.

An example use might be:

    carp_set("strip_to", 2, "verbose", 1, "trusted-libs", "pcre,foo", NULL);
    
Here are the settings you can use:

-   "verbose" int

    All macros show the full stack trace. This makes warn and carp the same as a
    cluck, die and croak the same as a confess.
    Defaults to environment var CARP_VERBOSE

-   "muzzled" int

    Stack trace is never performed. This makes carp and cluck the same as warn,
    croak and confess the same as die.
    Defaults to environment var CARP_MUZZLED

-   "dump-stack" int

    Print as much info as you can about the stack.
    Defaults to environment var CARP_DUMP_STACK

-   "output" char*

    May be "default" to set the output back to the builtin output func.

-   "output-func" func

    A CarpOutputFunc to output the error message.

-   "strip" int

    The number of items to remove from the file names.
    Defaults to environment var CARP_STRIP

-   "strip-to" int

    The number of items to keep from the file names. If strip-to is 1,
    foo/bar/baz.c becomes baz.c.
    Defaults to environment var CARP_STRIP_TO

-   "suspected-libs" char*

    Comma separated list of lib names to remove from the list of trusted libs.

-   "trusted-libs" char*

    Comma separated list of lib names to be trusted.
    Defaults to environment var CARP_TRUSTED_LIBS


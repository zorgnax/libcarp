#include "carp.h"
#include "tap.h"
#include "foo.h"
#include "bar.h"

int
main () {
    char *expected;
    setvbuf(stdout, NULL, _IONBF, 0);

#ifdef _WIN32
    carp_set("strip-to", 1, NULL);
#endif
    
    plan(9);

    ok(1, "library compiled and is being used");

#if defined DEBUGGED && defined DYNAMIC
    /* The error is reported by the first one calling into the library that
    called it, here the error is reported in test.c since it called into foo.c
    which reported the error  */
    expected = "carp from fooa at test.c line 55\n";
#line 55
    is(fooa(), expected, "carp one lib away");
    
    expected = "confess from foob at foo.c line 55\n"
               "    foob() called at test.c line 55\n";
#line 55
    is(foob(), expected, "confess one lib away");
    
    expected = "croak from fooc at test.c line 55\n";
#line 55
    is(fooc(), expected, "croak one lib away");
    
    expected = "cluck from food at foo.c line 55\n"
               "    food() called at test.c line 55\n";
#line 55
    is(food(), expected, "cluck one lib away");
    
    expected = "carp from bara at foo.c line 55\n";
#line 55
    is(fooe(), expected, "carp two libs away");
    expected = "confess from barb at bar.c line 55\n"
               "    barb() called at foo.c line 55\n"
               "    foof() called at test.c line 55\n";
#line 55
    is(foof(), expected, "confess two libs away");

    expected = "croak from barc at foo.c line 55\n";
#line 55
    is(foog(), expected, "croak two libs away");

    expected = "cluck from bard at bar.c line 55\n"
               "    bard() called at foo.c line 55\n"
               "    fooh() called at test.c line 55\n";
#line 55
    is(fooh(), expected, "cluck two libs away");

#endif

#if defined DEBUGGED && !defined DYNAMIC
    /* Being that it does not know where the trust boundary is, a carp will
    show the error where it happened, in foo.c.  */
    expected = "carp from fooa at foo.c line 55\n";
#line 55
    is(fooa(), expected, "carp one lib away");

    expected = "confess from foob at foo.c line 55\n"
               "    foob() called at test.c line 55\n";
#line 55
    is(foob(), expected, "confess one lib away");

    expected = "croak from fooc at foo.c line 55\n";
#line 55
    is(fooc(), expected, "croak one lib away");

    expected = "cluck from food at foo.c line 55\n"
               "    food() called at test.c line 55\n";
#line 55
    is(food(), expected, "cluck one lib away");

    expected = "carp from bara at bar.c line 55\n";
#line 55
    is(fooe(), expected, "carp two libs away");

    expected = "confess from barb at bar.c line 55\n"
               "    barb() called at foo.c line 55\n"
               "    foof() called at test.c line 55\n";
#line 55
    is(foof(), expected, "confess two libs away");

    expected = "croak from barc at bar.c line 55\n";
#line 55
    is(foog(), expected, "croak two libs away");

    expected = "cluck from bard at bar.c line 55\n"
               "    bard() called at foo.c line 55\n"
               "    fooh() called at test.c line 55\n";
#line 55
    is(fooh(), expected, "cluck two libs away");

#endif

#if !defined DEBUGGED && defined DYNAMIC
    /* Here, the library knows the error occurs in a different library than the
    main program, but since it can't find a file / line number, it will display
    the place where it occured.  */
    expected = "carp from fooa at foo.c line 55\n";
#line 55
    is(fooa(), expected, "carp one lib away");

    expected = "confess from foob at foo.c line 55\n"
#ifndef _WIN32
               "    foob()\n"
#endif
    ;
#line 55
    is(foob(), expected, "confess one lib away");

    expected = "croak from fooc at foo.c line 55\n";
#line 55
    is(fooc(), expected, "croak one lib away");

    expected = "cluck from food at foo.c line 55\n"
#ifndef _WIN32
               "    food()\n"
#endif
    ;
#line 55
    is(food(), expected, "cluck one lib away");

    expected = "carp from bara at bar.c line 55\n";
#line 55
    is(fooe(), expected, "carp two libs away");

    expected = "confess from barb at bar.c line 55\n"
#ifndef _WIN32
               "    barb() called from t/libnddfoo.so\n"
               "    foof()\n"
#endif
    ;
#line 55
    is(foof(), expected, "confess two libs away");

    expected = "croak from barc at bar.c line 55\n";
#line 55
    is(foog(), expected, "croak two libs away");

    expected = "cluck from bard at bar.c line 55\n"
#ifndef _WIN32
               "    bard() called from t/libnddfoo.so\n"
               "    fooh()\n"
#endif
    ;
#line 55
    is(fooh(), expected, "cluck two libs away");

#endif

#if !defined DEBUGGED && !defined DYNAMIC
    expected = "carp from fooa at foo.c line 55\n";
#line 55
    is(fooa(), expected, "carp one lib away");

    expected = "confess from foob at foo.c line 55\n"
#ifndef _WIN32
               "    foob()\n"
#endif
    ;
#line 55
    is(foob(), expected, "confess one lib away");

    expected = "croak from fooc at foo.c line 55\n";
#line 55
    is(fooc(), expected, "croak one lib away");

    expected = "cluck from food at foo.c line 55\n"
#ifndef _WIN32
               "    food()\n"
#endif
    ;
#line 55
    is(food(), expected, "cluck one lib away");

    expected = "carp from bara at bar.c line 55\n";
#line 55
    is(fooe(), expected, "carp two libs away");

    expected = "confess from barb at bar.c line 55\n"
#ifndef _WIN32
               "    barb()\n"
               "    foof()\n"
#endif
    ;
#line 55
    is(foof(), expected, "confess two libs away");

    expected = "croak from barc at bar.c line 55\n";
#line 55
    is(foog(), expected, "croak two libs away");

    expected = "cluck from bard at bar.c line 55\n"
#ifndef _WIN32
               "    bard()\n"
               "    fooh()\n"
#endif
    ;
#line 55
    is(fooh(), expected, "cluck two libs away");

#endif

    return exit_status();
}

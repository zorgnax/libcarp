#include "carp.h"
#include "tap.h"
#include "foo.h"
#include "bar.h"

int
main () {
    setvbuf(stdout, NULL, _IONBF, 0);

    plan(9);

    ok(1, "library compiled and is being used");

#if defined DEBUGGED && defined DYNAMIC
    /* The error is reported by the first one calling into the library that
    called it, here the error is reported in test.c since it called into foo.c
    which reported the error  */
#line 55
    is(fooa(), "carp from fooa at test.c line 55\n", "carp one lib away");
#line 55
    is(foob(), "confess from foob at foo.c line 55\n"
               "    foob() called at test.c line 55\n",
       "confess one lib away");
#line 55
    is(fooc(), "croak from fooc at test.c line 55\n", "croak one lib away");
#line 55
    is(food(), "cluck from food at foo.c line 55\n"
               "    food() called at test.c line 55\n",
       "cluck one lib away");
#line 55
    is(fooe(), "carp from bara at foo.c line 55\n", "carp two libs away");
#line 55
    is(foof(), "confess from barb at bar.c line 55\n"
               "    barb() called at foo.c line 55\n"
               "    foof() called at test.c line 55\n",
       "confess two libs away");
#line 55
    is(foog(), "croak from barc at foo.c line 55\n", "croak two libs away");
#line 55
    is(fooh(), "cluck from bard at bar.c line 55\n"
               "    bard() called at foo.c line 55\n"
               "    fooh() called at test.c line 55\n",
       "cluck two libs away");
#endif

#if defined DEBUGGED && !defined DYNAMIC
    /* Being that it does not know where the trust boundary is, a carp will
    show the error where it happened, in foo.c.  */
#line 55
    is(fooa(), "carp from fooa at foo.c line 55\n", "carp one lib away");
#line 55
    is(foob(), "confess from foob at foo.c line 55\n"
               "    foob() called at test.c line 55\n",
       "confess one lib away");
#line 55
    is(fooc(), "croak from fooc at foo.c line 55\n", "croak one lib away");
#line 55
    is(food(), "cluck from food at foo.c line 55\n"
               "    food() called at test.c line 55\n",
       "cluck one lib away");
#line 55
    is(fooe(), "carp from bara at bar.c line 55\n", "carp two libs away");
#line 55
    is(foof(), "confess from barb at bar.c line 55\n"
               "    barb() called at foo.c line 55\n"
               "    foof() called at test.c line 55\n",
       "confess two libs away");
#line 55
    is(foog(), "croak from barc at bar.c line 55\n", "croak two libs away");
#line 55
    is(fooh(), "cluck from bard at bar.c line 55\n"
               "    bard() called at foo.c line 55\n"
               "    fooh() called at test.c line 55\n",
       "cluck two libs away");
#endif

#if !defined DEBUGGED && defined DYNAMIC
    /* Here, the library knows the error occurs in a different library than the
    main program, but since it can't find a file / line number, it will display
    the place where it occured.  */
#line 55
    is(fooa(), "carp from fooa at foo.c line 55\n", "carp one lib away");
#line 55
    is(foob(), "confess from foob at foo.c line 55\n"
               "    foob()\n",
       "confess one lib away");
#line 55
    is(fooc(), "croak from fooc at foo.c line 55\n", "croak one lib away");
#line 55
    is(food(), "cluck from food at foo.c line 55\n"
               "    food()\n",
       "cluck one lib away");
#line 55
    is(fooe(), "carp from bara at bar.c line 55\n", "carp two libs away");
#line 55
    is(foof(), "confess from barb at bar.c line 55\n"
               "    barb() called from t/libnddfoo.so\n"
               "    foof()\n",
       "confess two libs away");
#line 55
    is(foog(), "croak from barc at bar.c line 55\n", "croak two libs away");
#line 55
    is(fooh(), "cluck from bard at bar.c line 55\n"
               "    bard() called from t/libnddfoo.so\n"
               "    fooh()\n",
       "cluck two libs away");
#endif

#if !defined DEBUGGED && !defined DYNAMIC
#line 55
    is(fooa(), "carp from fooa at foo.c line 55\n", "carp one lib away");
#line 55
    is(foob(), "confess from foob at foo.c line 55\n"
               "    foob()\n",
       "confess one lib away");
#line 55
    is(fooc(), "croak from fooc at foo.c line 55\n", "croak one lib away");
#line 55
    is(food(), "cluck from food at foo.c line 55\n"
               "    food()\n",
       "cluck one lib away");
#line 55
    is(fooe(), "carp from bara at bar.c line 55\n", "carp two libs away");
#line 55
    is(foof(), "confess from barb at bar.c line 55\n"
               "    barb()\n"
               "    foof()\n",
       "confess two libs away");
#line 55
    is(foog(), "croak from barc at bar.c line 55\n", "croak two libs away");
#line 55
    is(fooh(), "cluck from bard at bar.c line 55\n"
               "    bard()\n"
               "    fooh()\n",
       "cluck two libs away");
#endif

    return exit_status();
}


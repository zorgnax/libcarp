#include "carp.h"
#include "tap.h"
#include "foo.h"
#include "bar.h"

int main () {
    ok(1, "library compiled and is being used");

#ifdef DEBUGGED
    is(fooa(), "carp from fooa at foo.c line 6\n", "carp one lib away");
#line 55
    is(foob(), "confess from foob at foo.c line 10\n"
               "    foob() called at test.c line 55\n",
               "confess one lib away");
    is(fooc(), "croak from fooc at foo.c line 14\n", "croak one lib away");
#line 55
    is(food(), "cluck from food at foo.c line 18\n"
               "    food() called at test.c line 55\n",
               "cluck one lib away");
    is(fooe(), "carp from bara at bar.c line 5\n", "carp two libs away");
#line 55
    is(foof(), "confess from barb at bar.c line 9\n"
               "    barb() called at foo.c line 26\n"
               "    foof() called at test.c line 55\n",
               "confess two libs away");
    is(foog(), "croak from barc at bar.c line 13\n", "croak two libs away");
#line 55
    is(fooh(), "cluck from bard at bar.c line 17\n"
               "    bard() called at foo.c line 34\n"
               "    fooh() called at test.c line 55\n",
               "cluck two libs away");
#endif

#ifdef NOTDEBUGGED
    is(fooa(), "carp from fooa at foo.c line 6\n", "carp one lib away");
    is(foob(), "confess from foob at foo.c line 10\n"
               "    foob() called\n",
               "confess one lib away");
    is(fooc(), "croak from fooc at foo.c line 14\n", "croak one lib away");
    is(food(), "cluck from food at foo.c line 18\n"
               "    food() called\n",
               "cluck one lib away");
    is(fooe(), "carp from bara at bar.c line 5\n", "carp two libs away");
    is(foof(), "confess from barb at bar.c line 9\n"
               "    barb() called\n"
               "    foof() called\n",
               "confess two libs away");
    is(foog(), "croak from barc at bar.c line 13\n", "croak two libs away");
    is(fooh(), "cluck from bard at bar.c line 17\n"
               "    bard() called\n"
               "    fooh() called\n",
               "cluck two libs away");
#endif
    
    return exit_status();
}


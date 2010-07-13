#include "carp.h"
#include "tap.h"
#include "foo.h"
#include "bar.h"

int
main () {
    char *expected;
    setvbuf(stdout, NULL, _IONBF, 0);
    plan(3);

    carp_set("strip-to", 1, NULL);
    
    expected = "carp from bara at foo.c line 55\n";
    is(fooe(), expected, "carp in bar blames foo");
    
    carp_set("trusted-libs", "ddfoo", NULL);
    expected = "carp from bara at trusted-libs.c line 55\n";
#line 55
    is(fooe(), expected, "carp in bar after trusting foo blames main");
    
    carp_set("suspected-libs", "ddfoo", NULL);
    expected = "carp from bara at foo.c line 55\n";
    is(fooe(), expected, "carp in bar blames foo after reverting trust status");
    
    return exit_status();
}


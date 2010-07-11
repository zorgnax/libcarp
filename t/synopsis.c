#include "carp.h"
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


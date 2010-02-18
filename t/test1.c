#include <carp.h>
#line 3 "libb"
void foo4() {
    warn("warn");
    carp("carp");
    cluck("cluck");
}
void foo3() {foo4();}
#line 10 "liba"
void foo2() {foo3();}
void foo1() {foo2();}
void bar2() {
    warn("warn");
    carp("carp");
    cluck("cluck");
    croak("croak");
}
void bar1() {bar2();}
#line 20 "test1.c"
int main() {
    foo1();
    bar1();
    return 0;
}


#include "carp.h"
#include "foo.h"
#include "bar.h"

char *
fooa () {
#line 55
    return scarp("carp from fooa");
}

char *
foob () {
#line 55
    return sconfess("confess from foob");
}

char *
fooc () {
#line 55
    return scroak("croak from fooc");
}

char *
food () {
#line 55
    return scluck("cluck from food");
}

char *
fooe () {
#line 55
    return bara();} /* microsoft's cpp messes up by one line unless I do this */

char *
foof () {
#line 55
    return barb();}

char *
foog () {
#line 55
    return barc();}

char *
fooh () {
#line 55
    return bard();}

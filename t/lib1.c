#include <stdio.h>
#include <carp.h>

void hello(const char *thing_to_greet) {
    if (!thing_to_greet) {
        carp("Attempted to greet a NULL thing");
        return;
    }
    if (it_doesnt_make_any_sense(thing_to_greet))
        cluck("Yet I'll do it anyway!!");
    printf("Hello %s.\n\n", thing_to_greet);
}


#include <stdio.h>
#include <string.h>
#include "carp.h"

int it_doesnt_make_any_sense (const char *thing_to_say_goodnight_to) {
    if (!strcmp(thing_to_say_goodnight_to, "room") ||
        !strcmp(thing_to_say_goodnight_to, "moon") ||
        !strcmp(thing_to_say_goodnight_to, "cow jumping over the moon") ||
        !strcmp(thing_to_say_goodnight_to, "light") ||
        !strcmp(thing_to_say_goodnight_to, "red balloon"))
    {
        carp("It wouldn't make no sense to say goodnight to a %s",
            thing_to_say_goodnight_to);
        return 1;
    }
    return 0;
}

void goodnight (const char *thing_to_say_goodnight_to) {
    if (!thing_to_say_goodnight_to) {
        carp("Attempted to say goodnight to a NULL thing");
        return;
    }
    if (it_doesnt_make_any_sense(thing_to_say_goodnight_to))
        cluck("Yet I'll do it anyway!!");
    printf("Goodnight %s.\n\n", thing_to_say_goodnight_to);
}


#include <stdio.h>
#include "carp.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    carp("Hello World!");
    hello("red balloon");
    hello("light");
    hello("cow jumping over the moon");
    hello("moon");
    hello("room");
    hello(NULL);
    goodnight("room");
    goodnight("moon");
    goodnight("cow jumping over the moon");
    goodnight("light");
    goodnight(NULL);
    goodnight("red balloon");
    return 0;
}


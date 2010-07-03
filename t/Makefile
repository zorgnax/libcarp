-include ../config.mak

CFLAGSND += -I..
LDFLAGS = ../$(CARPLIB) $(TAPLIB)

X = synopsis$(_X) debugged-test$(_X) not-debugged-test$(_X)

TAPLIB = tap$(_O)
O1 = synopsis$(_O)
O2 = debugged-test$(_O) debugged-foo$(_O) debugged-bar$(_O)
O3 = not-debugged-test$(_O) not-debugged-foo$(_O) not-debugged-bar$(_O)
O = $(O1) $(O2) $(O3)

$(X): ../$(CARPLIB) $(TAPLIB)
$(O): ../$(CARPLIB) ../carp.h tap.h

.PHONY: all
all: $(X) ../$(CARPLIB)

# included tap lib
$(TAPLIB): tap.c tap.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) tap.c

# short synopsis
synopsis$(_X): $(O1)
	$(CC) $(CFLAGS) $(CLOUT)$@ $(O1) $(LDFLAGS)

synopsis$(_O): synopsis.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) synopsis.c

# regular object linking with debugging symbols
debugged-test$(_X): $(O2)
	$(CC) $(CFLAGS) $(CLOUT)$@ $(O2) $(LDFLAGS)

debugged-test$(_O): test.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DDEBUGGED $(CFLAGS) test.c

debugged-foo$(_O): foo.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DDEBUGGED $(CFLAGS) foo.c

debugged-bar$(_O): bar.c bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DDEBUGGED $(CFLAGS) bar.c

# regular object linking without debugging symbols
not-debugged-test$(_X): $(O3)
	$(CC) $(CFLAGSND) $(CLOUT)$@ $(O3) $(LDFLAGS)

not-debugged-test$(_O): test.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) test.c

not-debugged-foo$(_O): foo.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) foo.c

not-debugged-bar$(_O): bar.c bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) bar.c

.PHONY: clean
clean:
	$(RM) -rv *.o *.obj *.lib *.pdb *.ilk $(X) _C


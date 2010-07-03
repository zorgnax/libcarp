-include ../config.mak

ifdef GNU
	DSFOOLIB = libdsfoo.a
	DSBARLIB = libdsbar.a
	NDSFOOLIB = libndsfoo.a
	NDSBARLIB = libndsbar.a
else
	DSFOOLIB = dsfoo.lib
	DSBARLIB = dsbar.lib
	NDSFOOLIB = ndsfoo.lib
	NDSBARLIB = ndsbar.lib
endif

CFLAGSND += -I..
LDFLAGS = ../$(CARPLIB) $(TAPLIB)

X = synopsis$(_X) debugged-test$(_X) not-debugged-test$(_X) debugged-staticlib-test$(_X) not-debugged-staticlib-test$(_X)

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

# static libs with debugging symbols
debugged-staticlib-test$(_X): debugged-test$(_O) $(DSFOOLIB) $(DSBARLIB)
	$(CC) $(CFLAGS) $(CLOUT)$@ debugged-test$(_O) $(DSFOOLIB) $(DSBARLIB) $(LDFLAGS)

$(DSFOOLIB): debugged-foo$(_O)
ifdef GNU
	$(AR) rcs $(DSFOOLIB) debugged-foo$(_O)
else
	lib /nologo /out:$(DSFOOLIB) debugged-foo$(_O)
endif

$(DSBARLIB): debugged-bar$(_O)
ifdef GNU
	$(AR) rcs $(DSBARLIB) debugged-bar$(_O)
else
	lib /nologo /out:$(DSBARLIB) debugged-bar$(_O)
endif

# regular object linking without debugging symbols
not-debugged-test$(_X): $(O3)
	$(CC) $(CFLAGSND) $(CLOUT)$@ $(O3) $(LDFLAGS)

not-debugged-test$(_O): test.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) test.c

not-debugged-foo$(_O): foo.c foo.h bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) foo.c

not-debugged-bar$(_O): bar.c bar.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ -DNOTDEBUGGED $(CFLAGSND) bar.c

# static libs without debugging symbols
not-debugged-staticlib-test$(_X): not-debugged-test$(_O) $(NDSFOOLIB) $(NDSBARLIB)
	$(CC) $(CFLAGSND) $(CLOUT)$@ not-debugged-test$(_O) $(NDSFOOLIB) $(NDSBARLIB) $(LDFLAGS)

$(NDSFOOLIB): not-debugged-foo$(_O)
ifdef GNU
	$(AR) rcs $(NDSFOOLIB) not-debugged-foo$(_O)
else
	lib /nologo /out:$(NDSFOOLIB) not-debugged-foo$(_O)
endif

$(NDSBARLIB): not-debugged-bar$(_O)
ifdef GNU
	$(AR) rcs $(NDSBARLIB) not-debugged-bar$(_O)
else
	lib /nologo /out:$(NDSBARLIB) not-debugged-bar$(_O)
endif

.PHONY: clean
clean:
	$(RM) -rv *.o *.obj *.lib *.pdb *.ilk $(X) _C *.a


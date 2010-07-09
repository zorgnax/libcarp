-include ../config.mak

LDFLAGS = $(if GNU, $(subst ,, -L. -Wl,--rpath=.:t))
LDFLAGS += ../$(CARPLIB)
ALLCFLAGS += -I..

DSFOOLIB  = $(if GNU, libdsfoo.a,   dsfoo.lib)
DSBARLIB  = $(if GNU, libdsbar.a,   dsbar.lib)
NDSFOOLIB = $(if GNU, libndsfoo.a,  dsfoo.lib)
NDSBARLIB = $(if GNU, libndsbar.a,  ndsbar.lib)
DDFOOLIB  = $(if GNU, libddfoo.so,  ddfoo.dll)
DDBARLIB  = $(if GNU, libddbar.so,  ddbar.dll)
NDDFOOLIB = $(if GNU, libnddfoo.so, nddfoo.dll)
NDDBARLIB = $(if GNU, libnddbar.so, nddbar.dll)

TAPLIB = tap$(_O)
O1 = synopsis$(_O)
X1 = synopsis$(_X)
O2 = debugged-test$(_O) debugged-foo$(_O) debugged-bar$(_O)
X2-1 = debugged-test$(_X)
X2-2 = debugged-staticlib-test$(_X)
O3 = debugged-dynamic-test$(_O) debugged-dynamic-foo$(_O) debugged-dynamic-bar$(_O)
X3 = debugged-dynamiclib-test$(_X)
O4 = not-debugged-test$(_O) not-debugged-foo$(_O) not-debugged-bar$(_O)
X4-1 = not-debugged-test$(_X)
X4-2 = not-debugged-staticlib-test$(_X)
O5 = not-debugged-dynamic-test$(_O) not-debugged-dynamic-foo$(_O) not-debugged-dynamic-bar$(_O)
X5 = not-debugged-dynamiclib-test$(_X)
O = $(O1) $(O2) $(O3) $(O4) $(O5)
X = $(X1) $(X2-1) $(X2-2) $(X3) $(X4-1) $(X4-4) $(X5)

TEST = $(filter %-test$(_O), $(O))
FOO = $(filter %-foo$(_O), $(O))
BAR = $(filter %-bar$(_O), $(O))

.PHONY: all
all: $(X) ../$(CARPLIB)

$(X): ../$(CARPLIB) $(TAPLIB)
$(O): ../carp.h tap.h
$(TEST): test.c foo.h bar.h
$(FOO): foo.c foo.h bar.h
$(BAR): bar.c bar.h

# included tap lib
$(TAPLIB): CFLAGS = $(DEBUG)
$(TAPLIB): tap.c tap.h

# short synopsis
$(X1): CFLAGS = $(DEBUG)
$(X1): $(O1)
$(O1): synopsis.c

# regular object linking with debugging symbols
$(X2-1): CFLAGS = $(DEBUG) -DDEBUGGED
$(X2-1): $(O2)

# static libs with debugging symbols
$(X2-2): CFLAGS = $(DEBUG) -DDEBUGGED
$(X2-2): debugged-test$(_O) $(DSFOOLIB) $(DSBARLIB)
$(DSFOOLIB): debugged-foo$(_O)
$(DSBARLIB): debugged-bar$(_O)

# dynamic libs with debugging symbols
$(X3): CFLAGS = $(DEBUG) -DDEBUGGED -DDYNAMIC $(PIC)
$(X3): debugged-dynamic-test$(_O) $(DDFOOLIB) $(DDBARLIB)
$(DDFOOLIB): debugged-dynamic-foo$(_O)
$(DDBARLIB): debugged-dynamic-bar$(_O)

# regular object linking without debugging symbols
$(X4-1): CFLAGS = -DNOTDEBUGGED
$(X4-1): $(O4)

# static libs without debugging symbols
$(X4-2): CFLAGS = -DNOTDEBUGGED
$(X4-2): not-debugged-test$(_O) $(NDSFOOLIB) $(NDSBARLIB)
$(NDSFOOLIB): not-debugged-foo$(_O)
$(NDSBARLIB): not-debugged-bar$(_O)

# dynamic libs without debugging symbols
$(X5): CFLAGS = -DNOTDEBUGGED -DDYNAMIC $(PIC)
$(X5): not-debugged-dynamic-test$(_O) $(NDDFOOLIB) $(NDDBARLIB)
$(NDDFOOLIB): not-debugged-dynamic-foo$(_O)
$(NDDBARLIB): not-debugged-dynamic-bar$(_O)

.PHONY: clean
clean:
	$(RM) -rv *.o *.obj *.lib *.pdb *.ilk $(X) _C *.a *.so *.dll


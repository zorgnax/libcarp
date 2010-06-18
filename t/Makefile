-include ../config.mak

CFLAGS += -I..

X = test1$(_X) debugged-main$(_X) not-debugged-main$(_X)

O1 = test1$(_O)
O2 = debugged-main$(_O) debugged-lib1$(_O) debugged-lib2$(_O)
O2 = not-debugged-main$(_O) not-debugged-lib1$(_O) not-debugged-lib2$(_O)
O = $(O1) $(O2) $(O3)

.PHONY: all
all: $(X) ../$(CARPLIB)

test1$(_X): $(O1) ../$(CARPLIB)
	$(CC) $(CLOUT)$@ $(O1) ../$(CARPLIB)

test1$(_O): test1.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) test1.c

.PHONY: clean
clean:
	$(RM) -v *.o *.obj *.lib *.pdb $(X)


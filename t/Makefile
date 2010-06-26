-include ../config.mak

CFLAGSND += -I..

X = test1$(_X) debugged-main$(_X) not-debugged-main$(_X)

O1 = test1$(_O)
O2 = debugged-main$(_O) debugged-lib1$(_O) debugged-lib2$(_O)
O3 = not-debugged-main$(_O) not-debugged-lib1$(_O) not-debugged-lib2$(_O)
O = $(O1) $(O2) $(O3)

.PHONY: all
all: $(X) ../$(CARPLIB)

test1$(_X): $(O1) ../$(CARPLIB)
	$(CC) $(CFLAGS) $(CLOUT)$@ $(O1) ../$(CARPLIB)

test1$(_O): test1.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) test1.c

debugged-main$(_X): $(O2) ../$(CARPLIB)
	$(CC) $(CFLAGS) $(CLOUT)$@ $(O2) ../$(CARPLIB)

debugged-main$(_O): main.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) main.c

debugged-lib1$(_O): lib1.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) lib1.c

debugged-lib2$(_O): lib2.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) lib2.c

not-debugged-main$(_X): $(O3) ../$(CARPLIB)
	$(CC) $(CFLAGSND) $(CLOUT)$@ $(O3) ../$(CARPLIB)

not-debugged-main$(_O): main.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGSND) main.c

not-debugged-lib1$(_O): lib1.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGSND) lib1.c

not-debugged-lib2$(_O): lib2.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGSND) lib2.c

.PHONY: clean
clean:
	$(RM) -v *.o *.obj *.lib *.pdb *.ilk $(X)


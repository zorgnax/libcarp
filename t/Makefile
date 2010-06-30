-include ../config.mak

CFLAGSND += -I..

X = synopsis$(_X) debugged-main$(_X) not-debugged-main$(_X)

O1 = synopsis$(_O)
O2 = debugged-main$(_O) debugged-lib1$(_O) debugged-lib2$(_O)
O3 = not-debugged-main$(_O) not-debugged-lib1$(_O) not-debugged-lib2$(_O)
O = $(O1) $(O2) $(O3)

.PHONY: all
all: $(X) ../$(CARPLIB)

synopsis$(_X): $(O1) ../$(CARPLIB)
	$(CC) $(CFLAGS) $(CLOUT)$@ $(O1) ../$(CARPLIB)

synopsis$(_O): synopsis.c
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) synopsis.c

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
	$(RM) -rv *.o *.obj *.lib *.pdb *.ilk $(X) _C


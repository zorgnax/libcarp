-include config.mak

O = carp$(_O) trace$(_O) funcinfo$(_O) list$(_O)
H = carp.h trace.h funcinfo.h list.h

ifdef GNU
	TRACE = tracegdb.c
else
	TRACE = tracewin.c
endif

.PHONY: all
all: $(CARPLIB) test

$(CARPLIB): $(O)
ifdef GNU
	$(AR) rcs $(CARPLIB) $(O)
else
	lib /nologo /out:$(CARPLIB) $(O)
endif

carp$(_O): carp.c carp.h list.h funcinfo.h trace.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) carp.c

trace$(_O): $(TRACE) trace.h list.h funcinfo.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) $(TRACE)

funcinfo$(_O): funcinfo.c funcinfo.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) funcinfo.c

list$(_O): list.c list.h
	$(CC) $(CCFLAGS) $(CCOUT)$@ $(CFLAGS) list.c

.PHONY: test
test:
	$(MAKE) -C t/ all

.PHONY: clean
clean:
	$(RM) -v $(CARPLIB) *.o *.obj *.lib *.pdb
	$(MAKE) -C t/ clean

.PHONY: dist
dist:
	rm libcarp.zip
	zip -r libcarp *

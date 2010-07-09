-include config.mak

CFLAGS = -g

.PHONY: all
all: $(CARPLIB)
	$(MAKE) -C t/ all

$(CARPLIB): carp$(_O) trace$(_O) funcinfo$(_O) list$(_O)
carp$(_O): carp.c carp.h list.h funcinfo.h trace.h
trace$(_O): $(if GNU, tracegdb.c, tracewin.c) trace.h list.h funcinfo.h
funcinfo$(_O): funcinfo.c funcinfo.h
list$(_O): list.c list.h

.PHONY: clean
clean:
	$(RM) -rv $(CARPLIB) *.o *.obj *.lib *.pdb *.ilk _C
	$(MAKE) -C t/ clean

.PHONY: dist
dist:
	$(RM) -v libcarp.zip
	zip -r libcarp *

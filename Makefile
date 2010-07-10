include config.mk

CFLAGS = $(DEBUG)

.PHONY: all
all: $(CARPLIB)
	$(MAKE) -C t/ all

$(CARPLIB): carp$(_O) trace$(_O) funcinfo$(_O) list$(_O) handy$(_O)
carp$(_O): carp.c carp.h carppriv.h
trace$(_O): $(if GNU, tracegdb.c, tracewin.c) carppriv.h
funcinfo$(_O): funcinfo.c carppriv.h
list$(_O): list.c carppriv.h
handy$(_O): handy.c carppriv.h

.PHONY: clean
clean:
	$(RM) -rv $(CARPLIB) *.o *.obj *.lib *.pdb *.ilk _C
	$(MAKE) -C t/ clean

.PHONY: dist
dist:
	$(RM) -v libcarp.zip
	zip -r libcarp *

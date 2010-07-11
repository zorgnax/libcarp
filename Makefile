include config.mk

CFLAGS = $(DEBUG)
LDFLAGS = $(if $(GNU),, dbghelp.lib)

.PHONY: all
all: $(CARPLIB) t

.PHONY: t
t: $(CARPLIB)
	$(MAKE) -C t/ all

$(CARPLIB): carp$(_O) config$(_O) tracedef$(_O) trace$(_O) funcinfo$(_O) \
			list$(_O) handy$(_O)
carp$(_O): carp.c carp.h carppriv.h
config$(_O): config.c carppriv.h
tracedef$(_O): $(if $(GNU), tracegdb.c, tracewin.c) carppriv.h
trace$(_O): trace.c carppriv.h
funcinfo$(_O): funcinfo.c carppriv.h
list$(_O): list.c carppriv.h
handy$(_O): handy.c carppriv.h

.PHONY: check
check: t
ifdef GNU
	prove -e '' t/*-test
else
	perl -e "system 'prove', '-q', '-m', '-e', '', glob('t/*-test.exe')"
endif

.PHONY: clean
clean:
	$(RM) -rv $(CARPLIB) *.o *.obj *.lib *.pdb *.ilk _C *.dll *.exp *.zip
	$(MAKE) -C t/ clean

.PHONY: dist
dist:
	$(RM) -v libcarp.zip
	zip -r libcarp *

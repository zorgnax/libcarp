GNU = 1
MAKE = make
ifdef GNU
	ALLCFLAGS = -Wall $(CFLAGS)
	DEBUG = -g
	CCFLAGS = -c
	CCOUT = -o
	CLOUT = -o
	PIC = -fPIC
	_O = .o
	_X =
	_A = .a
	_SO = .so
	CARPLIB = libcarp.a
	STATICLIB.o = $(AR) rcs $@ $(filter %$(_O), $^)
	DYNAMICLIB.o = $(CC) -shared -o $@ $(filter %$(_O), $^) $(LDFLAGS)
else
	CC = cl /nologo
	ALLCFLAGS = /Wall /wd4255 /wd4996 /wd4127 /wd4820 \
			    /wd4100 /wd4619 /wd4514 /wd4668 $(CFLAGS)
    DEBUG = /Zi
	CCFLAGS = /c
	CCOUT = /Fo
	CLOUT = /Fe
	PIC = 
	_O = .obj
	_X = .exe
	_A = .lib
	_SO = .dll
	CARPLIB = carp.lib
	STATICLIB.o = lib /nologo /out:$@ $(filter %$(_O), $^) $(LDFLAGS)
	DYNAMICLIB.o = link /nologo /out:$@ /dll $(LINKFLAGS) /machine:x86 \
                   $(patsubst %.dll, %.lib, $(filter %$(_O) %.lib %.dll, $^))
endif
COMPILE.c = $(CC) $(CCFLAGS) $(CPPFLAGS) $(ALLCFLAGS) $(CCOUT)$@ $(filter %.c, $^)
LINK.o = $(CC) $(ALLCFLAGS) $(CLOUT)$@ $(patsubst %.dll, %.lib, \
         $(filter %$(_O) %.a %.so %.lib %.dll, $^)) $(LDFLAGS)

%$(_X):; $(LINK.o)
%$(_A):; $(STATICLIB.o)
%$(_SO):; $(DYNAMICLIB.o)
%$(_O):; $(COMPILE.c)


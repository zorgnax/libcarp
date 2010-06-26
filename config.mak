GNU = 1

MAKE = make

ifdef GNU
	CFLAGSND = -Wall
	CFLAGS = -g $(CFLAGSND)
	CCFLAGS = -c
	CCOUT = -o 
	CLOUT = -o 
	_O = .o
	_X =
	CARPLIB = libcarp.a
else
	CC = cl /nologo
	CFLAGSND = /Wall /wd4255 /wd4996 /wd4127 /wd4820 \
			   /wd4100 /wd4619 /wd4514 /wd4668
	CFLAGS = /Zi $(CFLAGSND)
	CCFLAGS = /c
	CCOUT = /Fo
	CLOUT = /Fe
	_O = .obj
	_X = .exe
	CARPLIB = carp.lib
endif

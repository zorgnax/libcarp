OS := $(shell perl -e 'print $^O')
ifneq ($(OS),MSWin32)
	GNU = 1
endif

ifdef GNU
	CFLAGS = -g -Wall
	CCFLAGS = -c
	CCOUT = -o 
	CLOUT = -o 
	_O = .o
	_X =
	CARPLIB = libcarp.a
else
	CC = cl /nologo
	CFLAGS = /Zi /Wall /wd4255 /wd4996 /wd4127 /wd4820 \
			/wd4100 /wd4619 /wd4514 /wd4668
	CCFLAGS = /c
	CCOUT = /Fo
	CLOUT = /Fe
	_O = .obj
	_X = .exe
	CARPLIB = carp.lib
endif


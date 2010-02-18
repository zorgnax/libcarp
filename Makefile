all: libcarp.a t/dmain t/ndmain t/test1

libcarp.a: carp.c carp.h
	gcc -c -Wall carp.c
	ar rcs libcarp.a carp.o

t/test1: t/test1.c libcarp.a
	gcc -Wall -g -I. -L. t/test1.c -o t/test1 -lcarp

t/dmain: t/main.c t/dlib1.o t/dlib2.o libcarp.a
	gcc -g -I. -L. t/main.c -o t/dmain t/dlib1.o t/dlib2.o -lcarp

t/dlib1.o: t/lib1.c libcarp.a
	gcc -c -g -I. -L. t/lib1.c -o t/dlib1.o -lcarp

t/dlib2.o: t/lib2.c libcarp.a
	gcc -c -g -I. -L. t/lib2.c -o t/dlib2.o -lcarp

t/ndmain: t/main.c t/ndlib1.o t/ndlib2.o libcarp.a
	gcc -I. -L. t/main.c -o t/ndmain t/ndlib1.o t/ndlib2.o -lcarp

t/ndlib1.o: t/lib1.c libcarp.a
	gcc -c -I. -L. t/lib1.c -o t/ndlib1.o -lcarp

t/ndlib2.o: t/lib2.c libcarp.a
	gcc -c -I. -L. t/lib2.c -o t/ndlib2.o -lcarp

clean:
	rm -vf *~ *.a *.o t/dmain t/ndmain t/test1 t/*.o t/*~


all: libmemalloc.a app
memalloc.o: memalloc.c
	gcc -Wall -g -c memalloc.c
libmemalloc.a: memalloc.o
	ar cr libmemalloc.a memalloc.o
	ranlib libmemalloc.a
app.o: app.c
	gcc -Wall -g -c -I. app.c
app: app.o libmemalloc.a
	gcc -Wall -g -o app app.o -I. -L. -lmemalloc
clean:
	rm -fr *.a *.o *~ app libmemalloc.a

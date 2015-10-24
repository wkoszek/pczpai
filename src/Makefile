all: pai

CFLAGS=-g -ggdb -Wall -pedantic -Wno-invalid-source-encoding

pai:	pai.o pai_toklist.o pai_tok.o pai_bc.o pai_msh.o pai_m.o pai_ic.o

pai_toklist.o:
	g++ $(CFLAGS) -c pai_toklist.cc -o pai_toklist.o

pai_tok.o:
	g++ $(CFLAGS) -c pai_tok.cc -o pai_tok.o

pai_bc.o:
	g++ $(CFLAGS) -c pai_bc.cc -o pai_bc.o

pai_m.o:
	g++ $(CFLAGS) -c pai_m.cc -o pai_m.o

pai_msh.o:
	g++ $(CFLAGS) -c pai_msh.cc -o pai_msh.o

pai_ic.o:
	g++ $(CFLAGS) -c pai_ic.cc -o pai_ic.o

pai.o:
	g++ $(CFLAGS) -c pai.cc -o pai.o

pai:
	g++ $(CFLAGS) pai_m.o pai_msh.o pai_bc.o pai_toklist.o pai_tok.o pai_ic.o pai.o -o pai

test:
	./test.sh

clean:
	rm -rf pai *.o

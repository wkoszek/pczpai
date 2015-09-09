all: pai

pai:	pai.o pai_toklist.o pai_tok.o pai_bc.o pai_msh.o pai_m.o pai_ic.o

pai_toklist.o:
	g++ -g -ggdb -Wall -pedantic -c pai_toklist.cc -o pai_toklist.o

pai_tok.o:
	g++ -g -ggdb -Wall -pedantic -c pai_tok.cc -o pai_tok.o

pai_bc.o:
	g++ -g -ggdb -Wall -pedantic -c pai_bc.cc -o pai_bc.o

pai_m.o:
	g++ -g -ggdb -Wall -pedantic -c pai_m.cc -o pai_m.o

pai_msh.o:
	g++ -g -ggdb -Wall -pedantic -c pai_msh.cc -o pai_msh.o

pai_ic.o:
	g++ -g -ggdb -Wall -pedantic -c pai_ic.cc -o pai_ic.o

pai.o:
	g++ -g -ggdb -Wall -pedantic -c pai.cc -o pai.o

pai:
	g++ -g -ggdb -Wall -pedantic pai_m.o pai_msh.o pai_bc.o pai_toklist.o pai_tok.o pai_ic.o pai.o -o pai

test:
	./test.sh

clean:
	rm -rf pai *.o

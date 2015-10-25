all: pai

CXXFLAGS=-g -ggdb -Wall -pedantic -Wno-unused-but-set-variable

pai:	pai.o pai_toklist.o pai_tok.o pai_bc.o pai_msh.o pai_m.o pai_ic.o

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

pai:
	g++ $(CFLAGS) pai_m.o pai_msh.o pai_bc.o pai_toklist.o pai_tok.o pai_ic.o pai.o -o pai

test:
	./test.sh

clean:
	rm -rf pai *.o

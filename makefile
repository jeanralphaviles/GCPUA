all: gcpua

gcpua: instruction.o main.o parser.o token.o
	g++ -std=c++11 instruction.o main.o parser.o token.o -o gcpua -g

instruction.o: instruction.h instruction.cc
	g++ -std=c++11 -c instruction.cc -g

main.o: main.cc
	g++ -std=c++11 -c main.cc -g

parser.o: parser.h parser.cc
	g++ -std=c++11 -c parser.cc -g

token.o: token.h token.cc
	g++ -std=c++11 -c token.cc -g

test: gcpua
	./gcpua exampleAssembly.txt out.mif 1024

clean:
	rm -rf *.o gcpua out.mif

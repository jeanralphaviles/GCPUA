all: gcpua

gcpua: instruction.o main.o parser.o token.o
	g++ -std=c++11 instruction.o main.o parser.o token.o -o gcpua

instruction.o:
	g++ -std=c++11 -c instruction.cc

main.o:
	g++ -std=c++11 -c main.cc

parser.o:
	g++ -std=c++11 -c parser.cc

token.o:
	g++ -std=c++11 -c token.cc

test: gcpua
	./gcpua exampleAssembly.txt out.mif 1024 && xdg-open out.mif

clean:
	rm -rf *.o gcpua out.mif

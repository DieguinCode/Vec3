main:
	g++ -c main.cpp -o Bin/main.o

source:
	cd Sources && g++ -c vectors.cpp -o ../Bin/vectors.o

all: main source
	cd Bin && g++ main.o vectors.o -o VectorsOperations.diego

compile: all
	cd Bin && rm main.o vectors.o

run:
	cd Bin && ./VectorsOperations.diego
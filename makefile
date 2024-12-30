main:
	g++ -c main.cpp -o Bin/main.o

source:
	cd Sources && g++ -c vectors.cpp -o ../Bin/vectors.o
	g++ -c glad/src/glad.c -o Bin/glad.o

all: main source
	cd Bin && g++ main.o vectors.o glad.o -lglfw -o VectorsOperations.diego

compile: all
	cd Bin && rm main.o vectors.o glad.o

run:
	cd Bin && ./VectorsOperations.diego
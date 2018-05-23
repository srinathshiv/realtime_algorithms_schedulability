CC  =gcc
CCC =g++

all: main.c
	$(CC) -g -o qsn1.o main.c -Wall -lm
	$(CCC) -g -std=c++11 -o qsn2.o main.cpp -Wall

qsn1:
	$(CC) -g -o qsn1.o proj-2.c -Wall -lm

qsn2:
	$(CCC) -g -std=c++11 -o qsn2.o proj-2.cpp -Wall

run:
	./qsn1.o ./input.txt
	./qsn2.o 0.5 r

run1:
	./qsn1.o ./input.txt

run2:
	./qsn2.o 0.5 r

clean:
	rm -f *.o

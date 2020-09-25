all: testFetchInstruction

testFetchInstruction: fetchInstruction.o test.cpp
	g++ -g -std=c++11 -o testFetchInstruction test.cpp fetchInstruction.o

fetchInstruction.o: fetchInstruction.c
	gcc -g -o fetchInstruction.o -c fetchInstruction.c

clean:
	rm -rf *.o testFetchInstruction
all: testExecuteStackOperation

testExecuteStackOperation: executeStackOperation.o test.cpp
	g++ -g -o testExecuteStackOperation test.cpp executeStackOperation.o

executeStackOperation.o: executeStackOperation.c
	gcc -g -o executeStackOperation.o -c executeStackOperation.c

clean:
	rm -rf *.o testExecuteStackOperation
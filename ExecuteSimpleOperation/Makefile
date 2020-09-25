all: testExecuteSimpleOperation

testExecuteSimpleOperation: executeSimpleOperation.o test.cpp
	g++ -g -o testExecuteSimpleOperation test.cpp executeSimpleOperation.o

executeSimpleOperation.o: executeSimpleOperation.c
	gcc -g -o executeSimpleOperation.o -c executeSimpleOperation.c

clean:
	rm -rf *.o testExecuteSimpleOperation
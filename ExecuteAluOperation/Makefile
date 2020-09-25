all: testExecuteAluOperation

testExecuteAluOperation: executeAluOperation.o test.cpp
	g++ -g -o testExecuteAluOperation test.cpp executeAluOperation.o

executeAluOperation.o: executeAluOperation.c
	gcc -g -o executeAluOperation.o -c executeAluOperation.c

clean:
	rm -rf *.o testExecuteAluOperation
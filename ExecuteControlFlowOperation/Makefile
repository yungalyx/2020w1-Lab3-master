all: testExecuteControlFlowOperation

testExecuteControlFlowOperation: executeControlFlowOperation.o test.cpp
	g++ -g -o testExecuteControlFlowOperation test.cpp executeControlFlowOperation.o

executeControlFlowOperation.o: executeControlFlowOperation.c
	gcc -g -o executeControlFlowOperation.o -c executeControlFlowOperation.c

clean:
	rm -rf *.o testExecuteControlFlowOperation
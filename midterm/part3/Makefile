exe: utilities.o fileSystemOper.o operation.o
	gcc -o fileSystemOper fileSystemOper.o operation.o utilities.o -Wall -Wextra

fileSystemOper.o: fileSystemOper.c
	gcc -c fileSystemOper.c -Wall -Wextra

utilities.o: utilities.c
	gcc -c utilities.c -Wall -Wextra

operation.o: operation.c
	gcc -c operation.c -Wall -Wextra

clean: 
	rm *.o fileSystemOper

exe: makeFileSystem.o utilities.o operation.o
	gcc -o makeFileSystem makeFileSystem.o utilities.o -Wall -Wextra
	


makeFileSystem.o: makeFileSystem.c
	gcc -c makeFileSystem.c -Wall -Wextra

utilities.o: utilities.c
	gcc -c utilities.c -Wall -Wextra

operation.o: operation.c
	gcc -c operation.c -Wall -Wextra

clean: 
	rm *.o makeFileSystem

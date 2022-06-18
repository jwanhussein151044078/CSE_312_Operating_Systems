exe: sortArrays.o PageTable.o utilities.o statistics.o
	gcc -o sortArrays sortArrays.o PageTable.o utilities.o statistics.o -pthread -lm -Wall -Wextra

sortArrays.o: sortArrays.c
	gcc -c sortArrays.c -pthread -Wall -Wextra

PageTable.o: PageTable.c
	gcc -c PageTable.c -Wall -Wextra

utilities.o: utilities.c
	gcc -c utilities.c -lm -Wall -Wextra 

statistics.o: statistics.c
	gcc -c statistics.c -Wall -Wextra 

clean:
	rm *.o sortArrays



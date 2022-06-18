# include <stdio.h>
# include <string.h>
# include <math.h>
#include "utilities.h"

int parse_command_line(int argc , char* argv[] ,int *framesize ,int *numPhysical ,int *numVirtual ,int *pageTablePrintInt ,char **pageReplacement , char **allocPolicy , char **diskFileName ){
	int f , nPhysical , nVirtual , Print ;
	if(argc < 8){
		printf("usage information :\n\t./sortArrays frameSize numPhysical numVirtual pageReplacement allocPolicy pageTablePrintInt diskFileName.dat");
		return 0 ;
	}
	sscanf(argv[1],"%d",&f);
	sscanf(argv[2],"%d",&nPhysical);
	sscanf(argv[3],"%d",&nVirtual);
	sscanf(argv[6],"%d",&Print);
	if(nVirtual<=nPhysical){
		printf("ERROR :: the virtual memory size is equal to or smaller than the physical memory size !!!! \n");
		return 0 ;
	}
	*framesize = f ;
	*numPhysical = nPhysical;
	*numVirtual = nVirtual;
	*pageTablePrintInt = Print ;
	*pageReplacement = argv[4];
	*allocPolicy = argv[5];
	*diskFileName = argv[7];

	if(strcmp(argv[4],"NRU") != 0 && strcmp(argv[4],"FIFO") != 0 && strcmp(argv[4],"SC") != 0 && strcmp(argv[4],"LRU") != 0 && strcmp(argv[4],"WSClock") != 0){
		printf("ERROR :: unknown page replacement algorithm !!!\n");
		return 0 ;
 	}
 	if(strcmp(argv[5],"global") != 0 && strcmp(argv[5],"local") != 0 ){
 		printf("ERROR :: unknown allocPolicy !!!\n");
 		return 0 ;
 	}
	return 1 ;
}

int power2(int b){
	return pow(2,b);
}
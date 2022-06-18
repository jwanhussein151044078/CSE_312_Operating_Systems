# include <stdio.h>
# include <string.h>
# include "operation.h"

void print_usage_info();
int parse_command_line(int argc,char *argv[] , char** Sfn , char** fn , char** path);

int main(int argc, char *argv[]){
	int op ;
	char *Sfilename;
	char *filename ;
	char *path;	
	if((op = parse_command_line(argc,argv,&Sfilename,&filename,&path))==-1){
		printf("invalid command\n");
		printf("usage_info ::\n");
		print_usage_info();
		return 0 ;
	}
	if(op == 1){
		op_ls(Sfilename,path);
	}else if(op == 2){
		op_mkdir(Sfilename,path);
	}else if(op == 3){
		op_rmdir(Sfilename,path);
	}else if(op == 4){
		op_dumpe2fs(Sfilename);
	}else if(op == 5){
		op_write(Sfilename,path,filename);
	}else if(op == 6){
		op_read(Sfilename,path,filename);
	}else if(op == 7){
		printf("unimplemented !!\n");
	}else{
		printf("error\n");
	}
	return 0 ;
}


int parse_command_line(int argc,char *argv[] , char** Sfn , char** fn , char** path){	
	int op ;

	if(argc < 3){
		return -1 ;
	}
	*Sfn = argv[1];
	if(strcmp(argv[2],"list")== 0){
		if(argc != 4)return -1 ;
		*path = argv[3];
		op = 1 ;
	}else if(strcmp(argv[2],"mkdir")==0){
		if(argc != 4)return -1 ;
		*path = argv[3];
		op = 2 ;
	}else if(strcmp(argv[2],"rmdir")==0){
		if(argc != 4)return -1 ;
		*path = argv[3];
		op = 3 ;
	}else if(strcmp(argv[2],"dumpe2fs")==0){
		if(argc != 3)return -1 ;
		op = 4 ;
	}else if(strcmp(argv[2],"write")==0){
		if(argc != 5)return -1 ;
		*path = argv[3];
		*fn = argv[4];
		op = 5 ;
	}else if(strcmp(argv[2],"read")==0){
		if(argc != 5)return -1 ;
		*path = argv[3];
		*fn = argv[4];
		op = 6 ;
	}else if(strcmp(argv[2],"del")==0){
		if(argc != 4)return -1 ;
		*path = argv[3];
		op = 7 ;
	}else{
		return -1 ;
	}	
	return op ;
}

void print_usage_info(){
	printf("fileSystemOper name_of_the_file_system_file list “path”\n");
	printf("fileSystemOper name_of_the_file_system_file mkdir “path”\n");
	printf("fileSystemOper name_of_the_file_system_file rmdir “path”\n");
	printf("fileSystemOper name_of_the_file_system_file dumpe2fs\n");
	printf("fileSystemOper name_of_the_file_system_file write “path” filename\n");
	printf("fileSystemOper name_of_the_file_system_file read “path” filename\n");
	printf("fileSystemOper name_of_the_file_system_file del  “path and file name”\n");
}
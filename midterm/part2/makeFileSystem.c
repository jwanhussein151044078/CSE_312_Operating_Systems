#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utilities.h"


#include <sys/mman.h>
int is_command_line(int argc, char *argv[]);
int create_file(char* filename);
int check_para(int BLOCK_SIZE ,int FREE_INODE);

int main(int argc, char *argv[]){
	int BLOCK_SIZE ;
	int FREE_INODE ;	
	char filename[32];
	int fd ;
	struct superblock sp;


	if(!is_command_line(argc,argv)){
		printf("usage info :\n\t./makeFileSystem blocksize(int) number_of_free_inode(int) filename\n");
		return -1 ;
	}

	sscanf(argv[1],"%d",&BLOCK_SIZE);
	sscanf(argv[2],"%d",&FREE_INODE);
	sscanf(argv[3],"%s",filename);

	if(check_para(BLOCK_SIZE , FREE_INODE) == -1){
		return -1;
	}
	if((fd = create_file(filename))==-1){
		return -1 ;
	}
	sp = create_superblock(BLOCK_SIZE,FREE_INODE);	
	
	if(write_superblock_info(fd , sp)==-1){
		perror("write superblock ERROR :");
		close(fd);
		return -1 ;
	}
	if(write_fsm_info(fd, sp) == -1){
		perror("write fsm ERROR :");
		close(fd);
		return -1 ;	
	}	
	if(init_root(&sp,fd)==-1){
		perror("write fsm ERROR :");
		close(fd);
		return -1 ;	
	}
	/*
	printf("block size   %d\n",sp.block_size);
	printf("block no     %d\n",sp.block_NO);
	printf("fblock no    %d\n",sp.fblock_NO);
	printf("inode no     %d\n",sp.inode_NO);
	printf("finode no    %d\n",sp.finode_NO);
	printf("addr_fsm     %d\n",sp.addr_fsm);
	printf("addr_inode   %d\n",sp.addr_inode);
	printf("addr_root    %d\n",sp.addr_rootdir);
	printf("addr_fanddir %d\n",sp.addr_fanddir);
	printf("addr_sib     %d\n",sp.addr_SIB);
	printf("addr_dib     %d\n",sp.addr_DIB);
	printf("addr_tib     %d\n",sp.addr_TIB);
	//print_fsm(sp,fd);
	*/
	return 0 ;
}

int is_command_line(int argc, char *argv[]){
	int i = 0 ;
	if(argc != 4)return 0;
	while(argv[1][i]){
		if(argv[1][i] < '0' || argv[1][i] > '9'){
			return 0 ;
		}
		i++;
	}
	i = 0 ;
	while(argv[2][i]){
		if(argv[2][i] < '0' || argv[2][i] > '9'){
			return 0 ;
		}
		i++;
	}
	return 1 ;
}

int create_file(char* filename){
	int fd ;
	//char temp[1024];

	fd = open(filename,O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd == -1){
		perror("open file error :");
		return -1;
	}
	
	if(ftruncate(fd,1048576)==-1){
		perror("ftruncate error :");
		return -1;
	}
	/*
	int i ;
	for ( i = 0; i < 1024 ; ++i){
		temp[i] = ' ';
	}
	for ( i = 0; i < 1024 ; ++i){
		write(fd,temp,1024);
	}
	*/
	/*
	void *np;
    if (ftruncate (fd, 1048576) < 0){
    	perror("ftruncate ERROR :");
    	return -1 ;
    }
       
    np = mmap (NULL,1048576, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (np == MAP_FAILED){
    	perror("mmap ERROR :");
    	return -1 ;
    }
    */
	lseek(fd,0L,SEEK_SET);	
	return fd ;
}

int check_para(int BLOCK_SIZE ,int FREE_INODE){
	if(BLOCK_SIZE > 16 || BLOCK_SIZE < 1){
		printf("block size should be less than or equal to 16\n");
		return -1 ;
	}
	if (BLOCK_SIZE != 1 && BLOCK_SIZE != 2 && BLOCK_SIZE != 4 && BLOCK_SIZE != 8 && BLOCK_SIZE != 16 ){
		printf("block size should be power of 2 \n");
		return -1;
	}
	if(FREE_INODE < 1024/BLOCK_SIZE){
		printf("free inode is to small\n");
		return -1 ;
	}
	return 1 ;
}


/*
	printf("block size   %d\n",sp.block_size);
	printf("block no     %d\n",sp.block_NO);
	printf("fblock no    %d\n",sp.fblock_NO);
	printf("inode no     %d\n",sp.inode_NO);
	printf("finode no    %d\n",sp.finode_NO);
	printf("addr_fsm     %d\n",sp.addr_fsm);
	printf("addr_inode   %d\n",sp.addr_inode);
	printf("addr_root    %d\n",sp.addr_rootdir);
	printf("addr_fanddir %d\n",sp.addr_fanddir);
	printf("addr_sib     %d\n",sp.addr_SIB);
	printf("addr_dib     %d\n",sp.addr_DIB);
	printf("addr_tib     %d\n",sp.addr_TIB);
	*/
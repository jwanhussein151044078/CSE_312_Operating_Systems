# include <stdio.h>
# include <unistd.h>
# include <sys/types.h>
# include <stdlib.h>
# include <time.h>
# include "utilities.h"
# include "operation.h"

struct superblock create_superblock(int blocksize ,int finode ){
	int TsizeofInode = finode*109;
	int sizeofSIB = finode*16;
	int sizeofDIB = finode*48;
	int sizeofTIB = finode*156;
	int total = TsizeofInode + sizeofSIB + sizeofTIB + sizeofDIB ;
	int temp ,temp2;
	struct superblock sp ;

	sp.block_size = 1024*blocksize ;
	sp.block_NO = 1024/blocksize ;
	sp.inode_NO = finode ;
	sp.finode_NO = finode;
	sp.addr_fsm = 1 ;
	temp = sp.block_NO + finode ;
	temp2 = temp/sp.block_size ;
	if(temp%sp.block_size != 0) temp2++ ;
	sp.addr_inode = sp.addr_fsm + temp2 ;  
	temp = TsizeofInode + (sp.addr_inode*sp.block_size) ;
	sp.addr_SIB = temp ;
	sp.addr_DIB = sp.addr_SIB + sizeofSIB;
	sp.addr_TIB = sp.addr_DIB + sizeofDIB;
	temp = total / sp.block_size ;
	if(total % sp.block_size != 0) temp++ ;
	sp.addr_rootdir = sp.addr_inode + temp ;
	sp.addr_fanddir = sp.addr_rootdir + 1;
	sp.fblock_NO = sp.block_NO - sp.addr_fanddir ;  
	return sp ;
}

int write_superblock_info(int fd ,struct superblock sp){
	char temp[10];
	
	sprintf(temp, "%05d",sp.block_size );
	if(write(fd,temp,5) != 5) return -1 ;
	
	sprintf(temp, "%04d",sp.block_NO );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.fblock_NO );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.inode_NO );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.finode_NO );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.addr_fsm );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.addr_inode );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.addr_rootdir );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%04d",sp.addr_fanddir );
	if(write(fd,temp,4) != 4) return -1 ;
	
	sprintf(temp, "%08d",sp.addr_SIB );
	if(write(fd,temp,8) != 8) return -1 ;
	
	sprintf(temp, "%08d",sp.addr_DIB );
	if(write(fd,temp,8) != 8) return -1 ;
	
	sprintf(temp, "%08d",sp.addr_TIB );
	if(write(fd,temp,8) != 8) return -1 ;
	return 1; 
}

int write_fsm_info(int fd ,struct superblock sp){
	int i ;
	int blockinuse = sp.block_NO - sp.fblock_NO ;
	int fsm = get_fsm_addr(sp);
	lseek(fd,fsm,SEEK_SET);
	
	for(i = 0 ; i < sp.block_NO ; i++){
		if(i < blockinuse){
			if(write(fd,"1",1) != 1) return -1 ;
		}
		else{
			if(write(fd,"0",1) != 1) return -1 ;
		}
	}
	for(i = 0 ; i < sp.inode_NO ; i++){
		if(write(fd,"0",1) != 1) return -1 ;		
	}
	return 1 ;	
}


int get_fsm_addr(struct superblock sp){
	return sp.addr_fsm*sp.block_size ;
}
int get_inode_addr(struct superblock sp){
	return sp.addr_inode*sp.block_size ;
}

void print_fsm(struct superblock sp , int fd){
	int fsm = get_fsm_addr(sp);
	lseek(fd,fsm,SEEK_SET);
	char temp ;
	int i ;
	int cout = 0;
	printf("block bit map \n");	
	for(i = 0 ; i < sp.block_NO ; i++){
		read(fd,&temp,1);
		printf("%3c ",temp);
		cout++; 
		if(cout == 16){
			printf("\n");
			cout = 0 ;
		}	
	}
	cout = 0 ;
	printf("\ninode bit map \n");	
	for(i = 0 ; i < sp.inode_NO ; i++){
		read(fd,&temp,1);
		printf("%3c ",temp);
		cout++; 
		if(cout == 16){
			printf("\n");
			cout = 0 ;
		}	
	}
	
	printf("\n" );
}

int init_root(struct superblock *sp , int fd){
	char temp[4];
	char *_time ;
	char *_date ;
	sp->finode_NO -= 1 ;
	lseek(fd,get_fsm_addr(*sp)+sp->block_NO,SEEK_SET);
	if(write(fd,"1",1) != 1) return -1 ;
	lseek(fd,get_inode_addr(*sp),SEEK_SET);
	if(write(fd,"00000000",8) != 8) return -1 ;
	if(write(fd,"0",1) != 1) return -1 ;
	if(write(fd,"                            root",32) != 32) return -1 ;
	if(write(fd,"      ",6) != 6) return -1 ;
	_time = get_time();
	if(write(fd,_time,8) != 8) return -1 ;
	free(_time);
	_date = get_date();
	if(write(fd,_date,10) != 10) return -1 ;
	free(_date);
	if(write(fd,"0001",4) != 4) return -1 ;
	sprintf(temp ,"%04d",sp->addr_rootdir);
	if(write(fd,temp,4) != 4) return -1 ;
	lseek(fd,sp->addr_rootdir*sp->block_size,SEEK_SET);
	if(write(fd,"00000000",8) != 8) return -1 ;
	if(write(fd,"0000",4) != 4) return -1 ;
	if(write(fd,"0000",4) != 4) return -1 ;
	if(write(fd,"0000",4) != 4) return -1 ;
	lseek(fd,17,SEEK_SET);
	sprintf(temp ,"%04d",sp->finode_NO);
	if(write(fd,temp,4) != 4) return -1 ;
	return 0 ;
}

struct superblock get_super_block(int fd){
	struct superblock sp ;
	
	char temp[8];

	lseek(fd , 0 , SEEK_SET);
	read(fd,temp,5);
	sscanf(temp , "%d" ,&sp.block_size);
	temp[4]='\0';
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.block_NO);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.fblock_NO);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.inode_NO);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.finode_NO);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.addr_fsm);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.addr_inode);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.addr_rootdir);
	read(fd,temp,4);
	sscanf(temp , "%d" ,&sp.addr_fanddir);
	read(fd,temp,8);
	sscanf(temp , "%d" ,&sp.addr_SIB);
	read(fd,temp,8);
	sscanf(temp , "%d" ,&sp.addr_DIB);
	read(fd,temp,8);
	sscanf(temp , "%d" ,&sp.addr_TIB);
	
	return sp ;
}

char* get_blocks_map(int fd ,int pos , int blockno){
	char *blocks = (char*)malloc(sizeof(char)*blockno);
	lseek(fd,pos,SEEK_SET);
	read(fd,blocks ,blockno);
	return blocks ;

}

char* get_inode_map(int fd ,int pos , int blockno , int inodeno){
	char *inodes = (char*)malloc(sizeof(char)*inodeno);
	lseek(fd,pos+blockno,SEEK_SET);
	read(fd,inodes ,inodeno);
	return inodes ;	
}

struct directory get_root(int fd , int pos){
	char size[8] ;
	char num[4];
	char parent[4] ;
	char self[4] ;
	struct directory dir;
	char no[4] ;
	char fname[32];
	int i;

	lseek(fd,pos,SEEK_SET);
	read(fd,size,8);
	read(fd,num,4);
	read(fd,parent,4);
	read(fd,self,4);
	sscanf(size,"%d",&dir.size);
	sscanf(num,"%d",&dir.num);
	sscanf(parent,"%d",&dir.parent);
	sscanf(self,"%d",&dir.self);
	dir.entry = (struct directory_entry*)malloc(sizeof(struct directory_entry)*(dir.num+1));
	for(i=0 ; i < dir.num ; i++){
		read(fd,no,4);
		read(fd,fname,32);
		sscanf(no,"%d",&dir.entry[i].inumber);
		sscanf(fname,"%s",dir.entry[i].filename);		
	}
	
	return dir ;
}

char* get_time(){
	time_t rawtime;
    struct tm * timeinfo;
    char *out = (char*)malloc(sizeof(char)*8);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    sprintf(out,"%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return out ;
}





char* get_date(){
	time_t rawtime;
    struct tm * timeinfo;
    char *out = (char*)malloc(sizeof(char)*10);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    sprintf(out,"%02d/%02d/%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
    return out ;
}

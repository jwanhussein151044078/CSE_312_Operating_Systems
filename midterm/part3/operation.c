#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
# include "operation.h"


int op_dumpe2fs(char *Sfilename){
	
	int fd = open(Sfilename , O_RDONLY);
	char *blocks;
	char *inodes;
	char name[32];
	char ext[6];
	int i ;
	int j = 0 ;

	if(fd == -1){
		perror("open system file ERROR ::");
		return -1 ;
	}
	struct superblock sp = get_super_block(fd);
	print_info_superblock(sp);
	blocks = get_blocks_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO);
	printf("occupied blocks { ");
	for (i = 0; i < sp.block_NO ; ++i){
		if(blocks[i] == '1'){
			j++;
			printf("%4d ",i );
		}
		if(j == 12){
			j = 0 ;
			printf("\n\t\t  ");
		}
	}
	free(blocks);
	printf("}\n");
	printf("occupied inode {\n");

	inodes = get_inode_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO,sp.inode_NO);
	for (i = 0; i < sp.inode_NO ; ++i){
		if(inodes[i] == '1'){
			lseek(fd,(sp.addr_inode*sp.block_size)+(i*97)+9,SEEK_SET);
			read(fd,name,32);
			read(fd,ext,6);
			if(ext[5] == ' '){
				printf("\tno.%4d  %s\n",i,name );
			}else{
				printf("\tno.%4d  %s.%s\n",i,name,ext );
			}
			
		}	
	}
	free(inodes);
	printf("}\n");
	close(fd);
	return 1 ;	
}

int op_ls(char* Sfilename,char* path){
	char *token ;
	struct superblock sp ;
	struct I_NODE node ;
	struct directory dir ;
	int self = 0;
	int i ;

	int fd = open(Sfilename , O_RDONLY);
	if(fd == -1 ){
		perror("open system file ERROR :");
	}
	sp = get_super_block(fd);
	token = strtok(path,"/");

	while(token != NULL){
		node = get_node(fd,self,sp);
		if(node.attr.is_file){
			printf("invalid path !!\n");
			close(fd);
			return 0;
		}
		dir = get_dir(fd,node.inodes[0],sp);
		for (i = 0; i < dir.num; ++i){
			if(strcmp(token,dir.entry[i].filename) == 0){
				self = dir.entry[i].inumber ;
				break ;
			}
		}
		if(i >= dir.num){
			printf("invalid path !!\n");
			if(dir.num>0)free(dir.entry);
			close(fd);
			return 0;
		}
		free(dir.entry);
		token = strtok(NULL,"/");
	}
	node = get_node(fd,self,sp);
	if(node.attr.is_file){
		printf("invalid path !!\n");
		close(fd);
		return 0 ;
	}	
	dir = get_dir(fd,node.inodes[0],sp);
	i = 0 ;
	while(i < dir.num && i < 26){
		node = get_node(fd,dir.entry[i].inumber,sp);
		printf("%8d\t\t",node.attr.size);
		printf("%s\t",node.attr.date );
		printf("%s\t",node.attr.time );
		printf("%s\n",node.attr.name );			
		i++;
	}

	if(dir.num > 0)free(dir.entry);
	close(fd);
	return 1 ;
}

int op_mkdir(char* Sfilename,char* path){
	
	int fd ;
	struct superblock sp ;
	struct directory dir ;
	struct I_NODE node ;
	struct directory newdir ;
	struct I_NODE newnode ;
	char *token ;
	char temp[32];
	int  parent=0 ;
	int counter = 0 ;

	char *_time , *_date ;
	char *inodemap , *blockmap ;

	int block=0 , inode=0 ;

	fd = open(Sfilename,O_RDWR);	
	if(fd == -1 ){
		perror("open System file ERROR :");
		return -1;
	}
	sp = get_super_block(fd);
	if(sp.finode_NO <= 0 || sp.fblock_NO <= 0 ){
		printf("no space to create a directory !!\n");
		close(fd);
		return 0 ;
	}

	token = strtok(path,"/");
	while(token){
		counter++;
		strcpy(temp,token);
		token = strtok(NULL ,"/");
		if(token != NULL){
			parent = search_for_parent(parent , temp , fd , sp );
			if(parent ==-1){
				printf("invalid path !!\n");
				close(fd);
				return 0 ;
			}
		}
	}
	if(counter == 0 ){
		printf("invalid path !11!\n");
		close(fd);
		//free(dir.entry);
		return -1 ;
	}

	node = get_node(fd,parent,sp);
	dir = get_dir(fd ,node.inodes[0],sp );
	if(dir.num > 0){
		int i ;
		for (i = 0; i < dir.num; ++i){
			if(strcmp(dir.entry[i].filename,temp)==0){
				printf("directory already exist !!\n");
				close(fd);
				free(dir.entry);
				return 0 ;
			}
		}
	}

	inodemap = get_inode_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO,sp.inode_NO);		
	blockmap = get_blocks_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO);
	while(inodemap[inode] == '1'){
		inode++;
	}
	while(blockmap[block] == '1'){
		block++;
	}
	inodemap[inode]='1';
	blockmap[block]='1';
	newnode.attr.size = 0 ;
	newnode.attr.is_file = 0 ;
	sprintf(newnode.attr.name,"%32s",temp);
	sprintf(newnode.attr.ext,"      ");
	_time = get_time();
	sprintf(newnode.attr.time,"%s",_time);
	free(_time);
	_date = get_date();
	sprintf(newnode.attr.date,"%s",_date);
	free(_date);
	newnode.attr.inodeused = 1 ;
	newnode.inodes[0] = block;

	newdir.size = 0 ;
	newdir.num = 0 ;
	newdir.parent = parent ;
	newdir.self = inode ;
	newdir.entry = NULL ;

	sp.finode_NO--;
	sp.fblock_NO--;
	
	write_block_map(fd ,sp ,blockmap );
	write_inode_map(fd ,sp ,inodemap );
	write_inode_at(fd , inode , newnode , sp);
	write_directory_at(fd,block,newdir,sp);	
	dir.num++;
	if(dir.num == 1)
		dir.entry = (struct directory_entry*)malloc(sizeof(struct directory_entry));
	else
		dir.entry = (struct directory_entry*)realloc(dir.entry,sizeof(struct directory_entry)*dir.num);
	dir.size = 0 ;	
	dir.entry[dir.num-1].inumber = inode;
	strcpy(dir.entry[dir.num-1].filename , temp);
	write_directory_at(fd,node.inodes[0],dir,sp);
	lseek(fd,9,SEEK_SET);
	sprintf(temp,"%04d",sp.fblock_NO);
	write(fd,temp,4);
	lseek(fd,17,SEEK_SET);
	sprintf(temp,"%04d",sp.finode_NO);
	write(fd,temp,4);
	
	free(dir.entry);
	free(inodemap);
	free(blockmap);
	close(fd);
	return 1;
}

int op_rmdir(char* Sfilename,char* path){
	char *token ;
	struct superblock sp ;
	struct I_NODE node ;
	struct directory dir ;
	struct directory newdir ;
	
	int self = 0;
	int parent = 0 ;
	int i , j =0 ;

	char *inodemap ;
	char *blockmap ;
	char temp[5];



	int fd = open(Sfilename , O_RDWR);
	if(fd == -1 ){
		perror("open system file ERROR :");
		return -1;
	}
	sp = get_super_block(fd);
	token = strtok(path,"/");

	while(token != NULL){
		node = get_node(fd,self,sp);
		if(node.attr.is_file){
			printf("invalid path !	!\n");
			close(fd);
			return 0;
		}
		dir = get_dir(fd,node.inodes[0],sp);
		for (i = 0; i < dir.num; ++i){
			if(strcmp(token,dir.entry[i].filename) == 0){
				parent = self ;
				self = dir.entry[i].inumber ;
				break ;
			}

		}
		if(i >= dir.num){
			printf("invalid path !!\n");
			if(dir.num>0)free(dir.entry);
			close(fd);
			return 0;
		}else{
			node = get_node(fd,self,sp);
			if(node.attr.is_file){
				printf("invalid path !!\n");
				if(dir.num>0)free(dir.entry);
				close(fd);
				return 0;		
			}
		}
		free(dir.entry);
		token = strtok(NULL,"/");
	}
	if(self == 0 ){
		printf("can not remove root directory !!\n");
		close(fd);
		return 0 ;
	}

	node = get_node(fd,self,sp);

	dir = get_dir(fd,node.inodes[0],sp);
	if(dir.num > 0){
		printf("only empty directory can be removed !!\n");
		close(fd);
		return 0 ;
	} 
//	printf("self %d , parent %d\n",self, parent );
	inodemap = get_inode_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO,sp.inode_NO);		
	blockmap = get_blocks_map(fd ,sp.addr_fsm*sp.block_size,sp.block_NO);
	inodemap[self] = '0';
	blockmap[node.inodes[0]] = '0';

	write_block_map(fd ,sp ,blockmap );
	write_inode_map(fd ,sp ,inodemap );
	
	sp.fblock_NO++;
	sp.finode_NO++;
	sprintf(temp ,"%04d",sp.fblock_NO);
	lseek(fd,9,SEEK_SET);
	write(fd,temp,4);

	sprintf(temp ,"%04d",sp.finode_NO);
	lseek(fd,17,SEEK_SET);
	write(fd,temp,4);

	node = get_node(fd,parent,sp);
	dir = get_dir(fd,node.inodes[0],sp);

	newdir.size = dir.size ;
	newdir.num = dir.num-1 ;
	newdir.self = dir.self ;
	newdir.parent = dir.parent ;
	if(newdir.num > 0){
		newdir.entry = (struct directory_entry*)malloc(sizeof(struct directory_entry)*newdir.num);
		for(i = 0 ; i < dir.num ; i++){
			if(dir.entry[i].inumber != self){
				newdir.entry[j].inumber = dir.entry[i].inumber ;
				strncpy(newdir.entry[j].filename ,dir.entry[i].filename ,32);
				j++;
			}
		}
	}
	write_directory_at(fd,node.inodes[0],newdir,sp);
	if(newdir.num)free(newdir.entry);
	free(dir.entry);
	free(inodemap);
	free(blockmap);

	close(fd);
	return 0 ;
}

int op_write(char *Sfilename,char *path,char *filename){
	int sfd , fd ;
	int fsize ;
	int file_blocks = 0 , counter = 0 ;
	
	struct superblock sp ;
	struct I_NODE node ;
	struct I_NODE newnode ;	
	struct directory dir ;

	int parent = 0 ;
	int nodepos = 0 ;
	char *token ;
	char temp[34];
	char *inodemap ;
	char *blockmap ;
	
	fd = open(filename,O_RDONLY);
	if(fd == -1 ){
		perror("open input file ERROR :");
		return -1 ;
	}
	fsize = lseek(fd,0,SEEK_END);
	sfd = open(Sfilename , O_RDWR);
	if(sfd == -1){
		perror("open system file ERROR :");
		close(fd);
		return -1 ;	
	}
	sp = get_super_block(sfd);
	
	file_blocks = fsize / sp.block_size ;
	if(fsize % sp.block_size != 0)file_blocks++;
	if(file_blocks > 43){
		printf("the input file size is (%d)bytes which need (%d)blocks \n",fsize,file_blocks);
		printf("the file is too big to be copied to the system file due to inodes implementation\nIt is explained in detail in the report\n");
		close(sfd);
		close(fd);
		return 0 ;
	}
	if(sp.fblock_NO < file_blocks){
		printf("There is not enough space to store \"%s\" \n ",filename);
		printf("free block in the system (%d)\n\"%s\" requires (%d)", sp.fblock_NO,filename,file_blocks);
		close(sfd);
		close(fd);
		return 0 ;
	}


	token = strtok(path,"/");
	while(token){
		counter++;
		strcpy(temp,token);
		token = strtok(NULL ,"/");
		if(token != NULL){
			parent = search_for_parent(parent , temp , sfd , sp );
			if(parent ==-1){
				printf("invalid path !!\n");
				close(fd);
				return 0 ;
			}
		}
	}
	if(counter == 0 ){
		printf("invalid path !!\n");
		close(fd);
		return -1 ;
	}

	node = get_node(sfd,parent,sp);
	dir = get_dir(sfd,node.inodes[0],sp);

	if(dir.num > 0){
		int q ;
		for (q = 0; q < dir.num; ++q){
			if(strcmp(dir.entry[q].filename,temp)==0){
				printf("file is already exist !!\n");
				close(fd);
				free(dir.entry);
				return 0 ;
			}
		}
	}
	newnode.attr.size = fsize ;
	newnode.attr.is_file = 1 ;
	sprintf(newnode.attr.name,"%32s",temp);
	sprintf(newnode.attr.ext,"      ");
	char*_time , *_date ;
	_time = get_time();
	_date = get_date();
	sprintf(newnode.attr.time,"%8s",_time);
	sprintf(newnode.attr.date,"%10s",_date);
	newnode.attr.inodeused = file_blocks ;

	inodemap = get_inode_map(sfd ,sp.addr_fsm*sp.block_size,sp.block_NO,sp.inode_NO);		
	blockmap = get_blocks_map(sfd ,sp.addr_fsm*sp.block_size,sp.block_NO);
//	printf(">>%s\n",blockmap );
	while(inodemap[nodepos] == '1'){
		nodepos++;
	}
	inodemap[nodepos] = '1';
	counter = 0 ;
	int bytewriten ;
	while(counter < 4 && counter < file_blocks){
		int j = 0 ;
		while(blockmap[j]=='1')j++;
		newnode.inodes[counter] = j ;
		blockmap[j] = '1';
		bytewriten = cpyfiletoSYS(sfd,fd,sp,j,counter*sp.block_size,-1);
		if(bytewriten == -1){
			perror("copy file ERROR :");
			close(sfd);
			close(fd);
			return -1 ;
		}
		counter++;	
	}

	if(counter < file_blocks && counter == 4){
		int j = 0 ;
		int q = 0 ;
 		char block[10];

		newnode.single_ind = sp.addr_SIB + (nodepos*16) ;
		while(q < 3 && counter < file_blocks){
//			printf("im here\n");
			while(blockmap[j]=='1')j++;

			blockmap[j] = '1';			
			lseek(sfd,newnode.single_ind+(q*4)+8,SEEK_SET);
			sprintf(block,"%04d",j);
			write(sfd,block,4);
			bytewriten = cpyfiletoSYS(sfd,fd,sp,j,counter*sp.block_size,-1);
			if(bytewriten == -1){
				perror("copy file ERROR :");
				close(sfd);
				close(fd);
				return -1 ;
			}
			q++;
			counter++;
		}			
	}

	if(counter < file_blocks){
		printf("the first 7'th block of the input file was copied to the system file \n");
		printf("the rest of the file was ignored due to double indirect inode problem \n");
		printf("it is explained in detail in the report\n");
		newnode.attr.size = 7*sp.block_size;
	}
	
	write_inode_at(sfd,nodepos,newnode,sp);
	//////////////////////////////////////////////////  change parent node and dir////////////////
//	node.attr.size += newnode.attr.size ;
	sprintf(node.attr.time,"%8s",_time);
	sprintf(node.attr.date,"%10s",_date);
	write_inode_at(sfd,parent,node,sp);
	dir.num++;
	if(dir.num == 1)
		dir.entry = (struct directory_entry*)malloc(sizeof(struct directory_entry));
	else
		dir.entry = (struct directory_entry*)realloc(dir.entry,sizeof(struct directory_entry)*dir.num);
	
	dir.entry[dir.num-1].inumber = nodepos;
	strncpy(dir.entry[dir.num-1].filename , temp ,32);
	write_directory_at(sfd,node.inodes[0],dir,sp);
	//////////////////////////////////////////////////////////////////////////////////////
	if(dir.num > 0)free(dir.entry);
	node = get_node(sfd,parent,sp);
	dir = get_dir(sfd,node.inodes[0],sp);	
	while(dir.self != dir.parent){
		node.attr.size += newnode.attr.size ;
		write_inode_at(sfd,dir.self,node,sp);	
		node = get_node(sfd,dir.parent,sp);
		if(dir.num > 0)free(dir.entry);
		dir = get_dir(sfd,node.inodes[0],sp);
	}
	//if(dir.num > 0)free(dir.entry);
	//////////////////////////////////////////////////////////////////////////////////////
	sp.fblock_NO -= file_blocks ;
	sp.finode_NO--;
	lseek(sfd,9,SEEK_SET);
	sprintf(temp,"%04d",sp.fblock_NO);
	write(sfd,temp,4);
	lseek(sfd,17,SEEK_SET);
	sprintf(temp,"%04d",sp.finode_NO);
	write(sfd,temp,4);

	write_block_map(sfd ,sp ,blockmap );
	write_inode_map(sfd ,sp ,inodemap );
	free(dir.entry);
	free(_date);
	free(_time);
	free(inodemap);
	free(blockmap);
	close(sfd);
	close(fd);
	return 1 ;
}
int op_read(char *Sfilename,char *path,char *filename){
	int sfd , fd ;
	struct superblock sp ;
	struct I_NODE node ;
	struct directory dir ;

	char* token ;


	int self = 0 ;
	int i = 0 ;
	int bytewriten ;
	int filesize ;

	if((sfd = open(Sfilename,O_RDONLY))==-1){
		perror("open system file ERROR :");
		return -1 ;
	}
	sp = get_super_block(sfd);

	token = strtok(path,"/");

	while(token != NULL){
		node = get_node(sfd,self,sp);
		
		if(node.attr.is_file){
			printf("invalid path !3!\n");
			close(sfd);
			return 0;
		}
		dir = get_dir(sfd,node.inodes[0],sp);
		for (i = 0; i < dir.num; ++i){
			if(strcmp(token,dir.entry[i].filename) == 0){
				self = dir.entry[i].inumber ;
				break ;
			}
		}
		if(i >= dir.num){
			printf("invalid path !1!\n");
			if(dir.num>0)free(dir.entry);
			close(sfd);
			return 0;
		}
		free(dir.entry);
		token = strtok(NULL,"/");		
	}

	node = get_node(sfd,self,sp);
	if(!node.attr.is_file){
		printf("invalid path !!\n");
		if(dir.num>0)free(dir.entry);
		close(sfd);
		return 0;		
	}
	
	if((fd = open(filename,O_RDWR | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR))==-1){
		perror("open output file ERROR :");
		close(sfd);
		return -1 ;
	}
	i = 0 ;
	filesize = node.attr.size ;
	while(i<node.attr.inodeused && i < 4 ){
		bytewriten = cpyfiletoSYS(fd,sfd,sp,i,node.inodes[i]*sp.block_size,sp.block_size-filesize);
		if(bytewriten == -1){
			perror("copy file ERROR :");
			close(sfd);
			close(fd);
			return -1 ;
		}
		filesize -= bytewriten ;
		i++;
	}
	if(i<node.attr.inodeused){
		int j = 0 , blockno;
		char temp[4];
		while(j < 3 && i< node.attr.inodeused){			
			lseek(sfd,node.single_ind+(j*4)+8,SEEK_SET);
			read(sfd,temp,4);
			sscanf(temp,"%d",&blockno);
			bytewriten = cpyfiletoSYS(fd,sfd,sp,i,blockno*sp.block_size,sp.block_size-filesize);
			if(bytewriten == -1){
				perror("copy file ERROR :");
				close(sfd);
				close(fd);
				return -1 ;
			}
			filesize -= bytewriten ;
			i++;
			j++;	
		}
	}


	close(fd);
	close(sfd);
	return 1 ;
}


int cpyfiletoSYS(int dest,int src,struct superblock sp, int blockno ,int start,int size){
	char *buf ;
	int readsize ;
	int byteread = 0;
	int bytewriten = 0 ;

	buf = (char*)malloc(sizeof(char)*sp.block_size+1);
	if(buf == NULL)return -1 ;
	lseek(dest, blockno*sp.block_size ,SEEK_SET);
	lseek(src , start , SEEK_SET);
	if(size<0){
		readsize = sp.block_size;
	}else{
		readsize = sp.block_size-size ;
	}
	byteread = read(src,buf,readsize);
	bytewriten = write(dest,buf,byteread);		
	free(buf);
	return bytewriten ;
}
int search_for_parent(int parent ,char* temp ,int fd ,struct superblock sp ){
	struct I_NODE node = get_node(fd , parent , sp);
	struct directory dir ;
	int ret ;
	if(!node.attr.is_file && node.attr.inodeused > 0){
		dir = get_dir(fd ,node.inodes[0],sp );
		if(dir.num <= 0){
			if(dir.num>0)free(dir.entry);
			return -1 ;
		}else{
			int i ;
			for ( i = 0; i < dir.num; ++i){
				if(strcmp(dir.entry[i].filename,temp)==0){
					ret = dir.entry[i].inumber ;
					free(dir.entry);
					return ret ;
				}
			}
			free(dir.entry);
			return -1 ;
		}
	}
	else{
		return -1 ;
	}
}

struct I_NODE get_node(int fd , int pos , struct superblock sp){
	struct I_NODE node ;
	int i ,counter = 0 ;
	char info[75];
	char size[10];
	char temp[4];

	lseek(fd,(sp.addr_inode*sp.block_size)+(pos*97),SEEK_SET);

	read(fd,info,69);
	for(i =0 ; i < 8 ; i++){
		size[i] = info[counter++];
	}
	sscanf(size,"%d",&node.attr.size);
	if(info[8] == '1'){
		node.attr.is_file =1;
	}else {
		node.attr.is_file =0;
	}
	counter++;
	for(i =0 ; i < 32 ; i++){
		node.attr.name[i] = info[counter++];
	}
	node.attr.name[32] = '\0';
	for(i =0 ; i < 6 ; i++){
		node.attr.ext[i] = info[counter++];
	}
	node.attr.ext[6] = '\0';
	for(i =0 ; i < 8 ; i++){
		node.attr.time[i] = info[counter++];
	}
	node.attr.time[8] = '\0';
	for(i =0 ; i < 10 ; i++){
		node.attr.date[i] = info[counter++];
	}
	node.attr.date[10] = '\0';
	for(i =0 ; i < 4 ; i++){
		size[i] = info[counter++];
	}
	size[4] = '\0';
	sscanf(size,"%d",&node.attr.inodeused);
	for(i = 0 ; i < 4 ; i++){		
		read(fd,temp,4);
		temp[4] = '\0';
		sscanf(temp,"%d",&node.inodes[i]);	
	}
	read(fd,size,8);
	size[8] = '\0';
	sscanf(size,"%d",&node.single_ind);	
	read(fd,size,8);
	size[8] = '\0';
	sscanf(size,"%d",&node.double_ind);
	read(fd,size,8);
	size[8] = '\0';
	sscanf(size,"%d",&node.triple_ind);
	return node ;
}

void print_info_superblock(struct superblock sp){
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
}

void write_block_map(int fd ,struct superblock sp , char* map ){
	lseek(fd,get_fsm_addr(sp),SEEK_SET);
	write(fd,map,sp.block_NO);	
}

void write_inode_map(int fd ,struct superblock sp , char* map ){	
	
	lseek(fd,get_fsm_addr(sp)+sp.block_NO,SEEK_SET);
	write(fd,map,sp.inode_NO);
}

void write_inode_at(int fd ,int inode ,struct I_NODE newnode ,struct superblock sp){
	
	char temp[10];
	char temp1[6];
	int i = 0 ;
	lseek(fd,get_inode_addr(sp)+(inode*97),SEEK_SET);

	sprintf(temp ,"%08d",newnode.attr.size);
	write(fd,temp,8);
	if(newnode.attr.is_file){
		write(fd,"1",1);
	}else{
		write(fd,"0",1);
	}
	write(fd,newnode.attr.name,32);
	write(fd,newnode.attr.ext,6);
	write(fd,newnode.attr.time,8);
	write(fd,newnode.attr.date,10);
	sprintf(temp1 ,"%04d",newnode.attr.inodeused);
	write(fd,temp1,4);
	
	while(i < newnode.attr.inodeused && i < 4){
		sprintf(temp1 ,"%04d",newnode.inodes[i]);
		write(fd,temp1,4);
		i++;
	}
	if(i < newnode.attr.inodeused){
		sprintf(temp ,"%08d",newnode.single_ind);
		write(fd,temp,8);	
		i += 3;
	}
	if(i < newnode.attr.inodeused){
		sprintf(temp ,"%08d",newnode.double_ind);
		write(fd,temp,8);	
		i += 9;
	}
	if(i < newnode.attr.inodeused){
		sprintf(temp ,"%08d",newnode.triple_ind);
		write(fd,temp,8);	
	}
}

void write_directory_at(int fd,int block,struct directory dir,struct superblock sp){
	char temp[10];
	char temp1[9];
	int i = 0;
	lseek(fd , (block*sp.block_size),SEEK_SET);
	sprintf(temp1 ,"%08d",dir.size);
	write(fd,temp1,8);
	sprintf(temp ,"%04d",dir.num);
	write(fd,temp,4);	
	sprintf(temp ,"%04d",dir.self);
	write(fd,temp,4);
	sprintf(temp ,"%04d",dir.parent);
	write(fd,temp,4);
	while(i < dir.num){
		sprintf(temp ,"%04d",dir.entry[i].inumber);
		write(fd,temp,4);
		write(fd,dir.entry[i].filename,32);
		i++;
	}
}

struct directory get_dir(int fd , int pos , struct superblock sp){
	struct directory dir;
	char temp[16];
	char temp1[10];
	if(lseek(fd , pos*sp.block_size , SEEK_SET) == -1){
		perror("lseek ERROR : ");
		dir.num = 0 ;
		return dir;
	}
	read(fd,temp,8);
	temp[8] = '\0';
	sscanf(temp ,"%d",&dir.size);
	read(fd,temp1,4);
	temp1[4] = '\0';
	sscanf(temp1 ,"%d",&dir.num);

	read(fd,temp,4);
	temp[4] = '\0';
	sscanf(temp ,"%d",&dir.self);

	read(fd,temp,4);
	temp[4] = '\0';
	sscanf(temp ,"%d",&dir.parent);


	if(dir.num > 0){
		int i ;
		dir.entry = (struct directory_entry*)malloc(sizeof(struct directory_entry)*dir.num);
		for( i = 0 ; i < dir.num ; i++){
			read(fd,temp,4);
			temp[4] = '\0';
			sscanf(temp ,"%d",&dir.entry[i].inumber);

			read(fd,dir.entry[i].filename,32);
			dir.entry[i].filename[32] = '\0';
		}
	}
	return dir ;
}
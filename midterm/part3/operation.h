#ifndef operation_H_
#define operation_H_
# include "utilities.h"

int op_dumpe2fs(char *Sfilename);
int op_mkdir(char* Sfilename,char* path);
int op_rmdir(char* Sfilename,char* path);
int op_ls(char* Sfilename,char* path);
int op_write(char *Sfilename,char *path,char *filename);
int op_read(char *Sfilename,char *path,char *filename);

void print_info_superblock(struct superblock sp);
struct I_NODE get_node(int fd , int pos , struct superblock sp);
struct directory get_dir(int fd , int pos , struct superblock sp);
void write_block_map(int fd ,struct superblock sp , char* map );
void write_inode_map(int fd ,struct superblock sp , char* map );
void write_inode_at(int fd ,int inode ,struct I_NODE newnode ,struct superblock sp);
void write_directory_at(int fd,int block,struct directory dir,struct superblock sp);
int search_for_parent(int parent ,char* temp ,int fd ,struct superblock sp );
int cpyfiletoSYS(int dest,int src,struct superblock sp, int blockno ,int start,int size);


#endif
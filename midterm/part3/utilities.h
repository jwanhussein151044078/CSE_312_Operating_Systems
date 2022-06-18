#ifndef utilities_H_
#define utilities_H_

struct superblock{
	int block_size; 		// 5byte
	int block_NO ;  		// 4byte
	int fblock_NO ; 		// 4byte
	int inode_NO ;			// 4byte
	int finode_NO ;			// 4byte
	int addr_fsm ;			// 4byte
	int addr_inode ;		// 4byte
	int addr_rootdir ;		// 4byte
	int addr_fanddir ;		// 4byte
	int addr_SIB ;			// 8byte
	int addr_DIB ;			// 8byte
	int addr_TIB ;			// 8byte
};

struct attributes{
	int size; 				// 8byte
	int is_file;			// 1byte
	char name[33];			// 32byte
	char ext[7]; 			// 6byte
	char time[9];			// 8byte
	char date[11];			// 10byte
	int inodeused;			// 4byte
};

struct I_NODE{
	struct attributes attr ;  // 69byte
	int inodes[4] ;			  // 16byte
	int single_ind ;		  // 8byte
	int double_ind ;		  // 8byte
	int triple_ind ; 		  // 8byte
};

struct directory_entry{
	int inumber ;			  // 4byte
	char filename[33];		  // 32byte
};

struct directory{
	int size ;                      // 8byte
	int num;                        // 4byte
	int self ;				  		// 4byte
	int parent ;              		// 4byte
	struct directory_entry *entry; 	
};


struct superblock create_superblock(int blocksize ,int finode );
struct superblock get_super_block(int finode);
int write_superblock_info(int fd ,struct superblock sp);
int write_fsm_info(int fd ,struct superblock sp);
int init_root(struct superblock *sp , int fd);
int get_fsm_addr(struct superblock sp);
int get_inode_addr(struct superblock sp);
void print_fsm(struct superblock sp , int fd);
char* get_blocks_map(int fd ,int pos , int blockno);
char* get_inode_map(int fd ,int pos , int blockno , int inodeno);
struct directory get_root(int fd , int pos);
char* get_time();
char* get_date();
#endif 
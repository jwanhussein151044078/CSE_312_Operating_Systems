#ifndef PageTable_H_
#define PageTable_H_
#include <stdint.h>
struct LL{
	int data ;
	struct LL *next;
};


struct PAGE_ENTRY{
	int page_frame_number;
	int PA ;
	int M ;
	int R ;
	int counter ;
	char* belongto ;
	
};

struct PAGE_TABLE{
	struct PAGE_ENTRY *pt ;
	int *pf ;
	int (*pagereplacementalgo)(struct PAGE_TABLE* ,int* , int , int* , char*);
	char *page_replacement_algorithm;
	int size ;
	int frame_size ;
	int diskfd ;
	int phy_size ;
	int pageTablePrintInt;
	int allocpo ;
	struct LL* head  ;	
};


struct PAGE_TABLE create_table(int Vsize ,int physize, int fs , char* pagereplacement , char* filename , int pageTablePrintInt , char* allocpo);
struct PAGE_ENTRY PT_get_at(struct PAGE_TABLE pt , int index);
void PT_set_at(struct PAGE_TABLE *pt , int index , int pfn , int PA );
void reset_R(struct PAGE_TABLE *pt);
int is_page_in_table(struct PAGE_TABLE pt , int pn);
void free_page_table(struct PAGE_TABLE* pt);
void _free(struct PAGE_TABLE* pt ,int *mem, char* );


int NRU(struct PAGE_TABLE* ,int*, int , int*, char*);
int FIFO(struct PAGE_TABLE* ,int*, int ,int*, char*);
int SC(struct PAGE_TABLE* ,int*, int ,int*, char*);
int LRU(struct PAGE_TABLE* ,int*, int ,int*, char*);
//int WSClock(struct PAGE_TABLE* ,int*, int ,int*, char*);


void write_to_vertual(int fd , int* mem ,int to,int from,int fs);
void write_to_mem(int fd ,int *mem,int from , int to , int fs);
void print_t(struct PAGE_TABLE* pt);

void print_l(struct LL *l);
#endif
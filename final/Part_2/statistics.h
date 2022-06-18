#ifndef statistics_H_
#define statistics_H_

struct statistics{
	int reads ;
	int writes ;
	int page_misses ;
	int page_replacements;
	int disk_page_writes ;
	int disk_page_reads ;
};

void print_s(struct statistics s,char* name);
void addtostatistics(struct statistics*,int , int , int , int ,int ,int);

#endif



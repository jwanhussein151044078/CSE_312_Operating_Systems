# include <stdio.h>
#include "statistics.h"



void print_s(struct statistics s,char* name){
	printf("-----------------------------------\n");
	printf("statistics for  '%s'\n",name);
	printf("\treads               %10d\n",s.reads );
	printf("\twrites              %10d\n",s.writes );
	printf("\tpage_misses         %10d\n",s.page_misses );
	printf("\tpage_replacements   %10d\n",s.page_replacements );
	printf("\tdisk_page_writes    %10d\n",s.disk_page_writes );
	printf("\tdisk_page_reads     %10d\n",s.disk_page_reads );
	printf("-----------------------------------\n");
}

void addtostatistics(struct statistics *s, int r, int w, int pm, int pr,int dpw,int dpr){
	s->reads += r ;
	s->writes += w ;
	s->page_misses += pm ;
	s->page_replacements += pr ;
	s->disk_page_writes += dpw ;
	s->disk_page_reads += dpr ;
}

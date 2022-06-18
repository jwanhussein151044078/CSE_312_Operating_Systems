# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

# include "PageTable.h"


struct PAGE_TABLE create_table(int Vsize ,int physize, int fs , char* pagereplacement , char* filename , int pageTablePrintInt , char* allocpo){
	struct PAGE_TABLE pt ;
	int i ;

	pt.pt = (struct PAGE_ENTRY*)malloc(sizeof(struct PAGE_ENTRY)*Vsize);
	pt.pf = (int*)malloc(sizeof(int)*physize);
	pt.size = Vsize ;
	pt.frame_size = fs ;
	pt.phy_size = physize ;
	pt.head = NULL;
	pt.pageTablePrintInt = pageTablePrintInt ;
	for(i = 0 ; i < Vsize ; i++){
		pt.pt[i].page_frame_number = 0 ;
		pt.pt[i].PA = 0 ;
		pt.pt[i].M = 0 ;
		pt.pt[i].R = 0 ;
		pt.pt[i].counter = 0 ;
	}
	for(i = 0 ; i < physize ; i++){
		pt.pf[i] = 0 ;
	}
	pt.diskfd = open(filename, O_RDWR | O_CREAT |  O_TRUNC , S_IRWXU | S_IRWXG | S_IRWXO );
	if(pt.diskfd == -1){
		perror("open disk file ERROR ::");
		exit(0);
	}
	for(i = 0 ; i < Vsize * fs ; i++){
		write(pt.diskfd, "         0\n" , 11);
	}

	pt.page_replacement_algorithm = pagereplacement ;
	if(strcmp(pagereplacement,"NRU") == 0){
		pt.pagereplacementalgo = &NRU ;
	}else if(strcmp(pagereplacement,"FIFO") == 0){
		pt.pagereplacementalgo = &FIFO ;
	}else if(strcmp(pagereplacement,"SC") == 0){
		pt.pagereplacementalgo = &SC ;
	}else if(strcmp(pagereplacement,"LRU") == 0){
		pt.pagereplacementalgo = &LRU ;
	}else if(strcmp(pagereplacement,"WSClock") == 0){
		//pt.pagereplacementalgo = &WSClock ;
	}else{
		printf("ERROR :: unknown page replacemaent algorithm !!!\n");
	}
	if(strcmp(allocpo,"local")==0){
		pt.allocpo = 0 ;
	}else if(strcmp(allocpo,"global")==0){
		pt.allocpo = 1 ;
	}else{
		pt.allocpo = -1 ;
	}
	return pt ;
}

struct PAGE_ENTRY get_at(struct PAGE_TABLE pt , int index){
	if(index >= pt.size){
		struct PAGE_ENTRY pe;
		pe.page_frame_number = -1 ;
		pe.PA = -1 ;
		pe.M = -1 ;
		pe.R = -1 ;
		return pe ;
	}else{
		return pt.pt[index];
	}
}

void PT_set_at(struct PAGE_TABLE *pt , int index , int pfn , int PA ){
	if(index < pt->size){
		pt->pt[index].page_frame_number = pfn ;
		pt->pt[index].PA = PA ;
		pt->pt[index].R = 0 ;
		pt->pt[index].M = 0 ;
	}
}

int is_page_in_table(struct PAGE_TABLE pt , int pn){
	if(pn<pt.size && pn >=0 ){
		return (pt.pt[pn].PA == 1)?pt.pt[pn].page_frame_number:-1 ;
	}else if(pn < 0){
		return -2 ;
	}
	return -1 ;
}

void free_page_table(struct PAGE_TABLE* pt){
	struct LL *l ;
	if(pt->pt){
		free(pt->pt);
	}
	if(pt->pf){
		free(pt->pf);
	}
	while(pt->head){
		l = pt->head ;
		pt->head = pt->head->next ;
		free(l);
	}
	close(pt->diskfd );
}


int NRU(struct PAGE_TABLE* pt,int* phy_mem, int pn ,int* is_written, char* tName){
	int where_to = -1;
	int from_where = -1 ;
	int i ;
	
 	for(i = 0 ; i < pt->phy_size ; i++ ){
		if(pt->pf[i] == 0){
			where_to = i ;
			from_where = pn ;
			break;
		}
	}
	while(where_to == -1){
		////////////////////////////////  serch for the lowest class ///////////////////
		
		int flag = 0;
		for(i = 0 ; i < pt->size ; i++){
			if(pt->pt[i].PA == 1){
				if(pt->pt[i].M == 0 && pt->pt[i].R == 0){
					where_to = pt->pt[i].page_frame_number ;
					from_where = i ;
					flag = 1 ;
					break;
				}
			}
		}
		
		if(flag)break;
		for(i = 0 ; i < pt->size ; i++){
			if(pt->pt[i].PA == 1){
				if(pt->pt[i].M == 1 && pt->pt[i].R == 0){
					where_to = pt->pt[i].page_frame_number ;
					from_where = i ;
					flag = 1 ;
					break;
				}
			}
		}
		
		if(flag)break;
		for(i = 0 ; i < pt->size ; i++){
			if(pt->pt[i].PA == 1){
				if(pt->pt[i].M == 0 && pt->pt[i].R == 1){
					where_to = pt->pt[i].page_frame_number ;
					from_where = i ;
					flag = 1 ;
					break;
				}
			}
		}
		if(flag)break;
		for(i = 0 ; i < pt->size ; i++){
			if(pt->pt[i].PA == 1){
				if(pt->pt[i].M == 1 && pt->pt[i].R == 1){
					where_to = pt->pt[i].page_frame_number ;
					from_where = i ;
					flag = 1 ;
					break;
				}
			}
		}
						
		////////////////////////////////////////////////////////////////////////////////
	}
	//printf("pn %d\n",pn);
	//printf("where_to %d\n",where_to );
	//printf("from_where %d\n",from_where);
	pt->pt[from_where].page_frame_number = 0 ;
	pt->pt[from_where].PA = 0 ;
	pt->pt[from_where].R = 0 ;
	*is_written = 0;
	if(pt->pt[from_where].M == 1){
	//	printf("writing to the file \n");
		write_to_vertual(pt->diskfd,phy_mem,from_where,where_to,pt->frame_size);
		*is_written = 1;
	}
	pt->pt[from_where].M = 0 ;
	pt->pt[from_where].belongto = NULL ;
	
	write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
	pt->pt[pn].page_frame_number = where_to ;
	//pt->pt[pn].M = 1 ;
	pt->pt[pn].PA = 1 ;
	pt->pt[pn].belongto = tName ;
	
	//pt->pt[pn].R = 0 ;
	pt->pf[where_to] = 1 ;

	

	//print_t(pt);
	return where_to ;
}

int FIFO(struct PAGE_TABLE* pt,int* phy_mem, int pn ,int* is_written, char* tName){
	int where_to = -1 ;
	int i ;
	*is_written = 0;
	if(pt->head == NULL){
		pt->head =  (struct LL*)malloc(sizeof(struct LL));
		pt->head->data = pn ;
		pt->head->next = NULL ;
		for(i=0;i<pt->phy_size;i++){
			if(pt->pf[i] == 0){
				break;
			}
		}
		pt->pt[pn].page_frame_number = i;
		pt->pt[pn].PA = 1;
		pt->pt[pn].belongto = tName ;
		pt->pf[i] = 1 ;		
		write_to_mem(pt->diskfd ,phy_mem , pn , i , pt->frame_size);
		return i;
	}else{

		for(i=0 ; i < pt->phy_size ;i++ ){
			if(pt->pf[i] == 0){
				where_to = i ;
				pt->pf[i] = 1 ;
				break ;
			}
		}
		if(where_to == -1){    // no empty space in physical mem 
			int from_where = pt->head->data;
			struct LL *temp = pt->head->next;
			where_to = pt->pt[pt->head->data].page_frame_number ;
			free(pt->head);
			pt->head = temp ;
			while(temp->next){
				temp = temp->next;
			}
			temp->next = (struct LL*)malloc(sizeof(struct LL));
			temp = temp->next ;
			temp->data = pn ;
			temp->next = NULL ;

			pt->pt[from_where].page_frame_number = 0 ;
			pt->pt[from_where].PA = 0 ;
			pt->pt[from_where].R = 0 ;
			pt->pt[from_where].belongto = NULL ;
			
			if(pt->pt[from_where].M == 1){
				write_to_vertual(pt->diskfd,phy_mem,from_where,where_to,pt->frame_size);
				*is_written = 1;
			}
			pt->pt[from_where].M = 0 ;

			write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
			pt->pt[pn].page_frame_number = where_to ;
			pt->pt[pn].PA = 1 ;
			pt->pt[pn].belongto = tName ;
			 
			return where_to ;
		}else{                 // empty spot was found in the mem
			struct LL* temp = pt->head ;
			while(temp->next){
				temp = temp->next ;				
			}
			temp->next =  (struct LL*)malloc(sizeof(struct LL));
			temp = temp->next ;
			temp->data = pn ;
			temp->next = NULL ;
			pt->pt[pn].page_frame_number = where_to ;
			pt->pt[pn].PA = 1 ;
			pt->pt[pn].belongto = tName ;			
			write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
			return where_to;
		}
	}
	return 0;
}
int SC(struct PAGE_TABLE* pt,int* phy_mem, int pn ,int* is_written, char* tName){
	int where_to = -1 ;
	int i ;
	*is_written = 0;
	if(pt->head == NULL){
		pt->head =  (struct LL*)malloc(sizeof(struct LL));
		pt->head->data = pn ;
		pt->head->next = NULL ;
		for(i=0;i<pt->phy_size;i++){
			if(pt->pf[i] == 0){
				break;
			}
		}
		pt->pt[pn].page_frame_number = i;
		pt->pt[pn].PA = 1;
		pt->pt[pn].belongto = tName ;
		pt->pf[i] = 1 ;		
		write_to_mem(pt->diskfd ,phy_mem , pn , i , pt->frame_size);

		return i ;
	}else{

		for(i=0 ; i < pt->phy_size ;i++ ){
			if(pt->pf[i] == 0){
				where_to = i ;
				pt->pf[i] = 1 ;
				break ;
			}
		}
		if(where_to == -1){    // no empty space in physical mem 
			struct LL *temp , *temp2 ;
			while(pt->pt[pt->head->data].R == 1){   // while the R bit is 1 
				temp = pt->head ;
				temp2 = pt->head->next ;
				while(temp->next){
					temp = temp->next ;
				}
				temp->next = pt->head ;
				pt->pt[temp->next->data].R = 0 ;
				pt->head->next = NULL ;
				pt->head = temp2 ;
			}
			int from_where = pt->head->data;
			temp = pt->head->next;
			where_to = pt->pt[pt->head->data].page_frame_number ;
			free(pt->head);
			pt->head = temp ;
			while(temp->next){
				temp = temp->next;
			}
			temp->next = (struct LL*)malloc(sizeof(struct LL));
			temp = temp->next ;
			temp->data = pn ;
			temp->next = NULL ;

			pt->pt[from_where].page_frame_number = 0 ;
			pt->pt[from_where].PA = 0 ;
			pt->pt[from_where].R = 0 ;
			pt->pt[from_where].belongto = NULL ;
			if(pt->pt[from_where].M == 1){
				write_to_vertual(pt->diskfd,phy_mem,from_where,where_to,pt->frame_size);
				*is_written = 1;
			}
			pt->pt[from_where].M = 0 ;

			write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
			pt->pt[pn].page_frame_number = where_to ;
			pt->pt[pn].PA = 1 ;
			pt->pt[pn].belongto = tName ;

			return where_to ;
		}else{                 // empty spot was found in the mem
			struct LL* temp = pt->head ;
			while(temp->next){
				temp = temp->next ;				
			}
			temp->next =  (struct LL*)malloc(sizeof(struct LL));
			temp = temp->next ;
			temp->data = pn ;
			temp->next = NULL ;
			pt->pt[pn].page_frame_number = where_to ;
			pt->pt[pn].PA = 1 ;	
			pt->pt[pn].belongto = tName ;		
			write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
			return where_to;
		}
	}
	return 0;	
}
int LRU(struct PAGE_TABLE* pt,int* phy_mem, int pn ,int* is_written, char* tName){
	int where_to = -1;
	int from_where = -1 ;
	int i ;
	int min = 0 ;
	int flag = 0; 
	*is_written = 0 ;
 	for(i = 0 ; i < pt->phy_size ; i++ ){
		if(pt->pf[i] == 0){
			pt->pf[i] = 1 ;
			where_to = i ;
			from_where = pn ;
			break;
		}
	}
	while(where_to == -1){
		if(pt->allocpo == 0){
			min = -1 ;
			for(i = 0 ; i < pt->size ; i++){
				if( pt->pt[i].PA == 1 ){
					if(strcmp(pt->pt[i].belongto,tName)==0){
						min = i ;
						break;
					}
				}
			}
			for(;i<pt->size;i++){
				if((pt->pt[i].counter < pt->pt[min].counter) && pt->pt[i].PA == 1){					
					if(strcmp(pt->pt[i].belongto,tName)==0){
						min = i;
					}
				}
			}
			if(min == -1){
				flag = 1 ;
			}else{
				from_where = min ;
				where_to = pt->pt[from_where].page_frame_number ;
			}
		}
		if (pt->allocpo == 1 || flag){
			for(i = 0 ; i < pt->size ; i++){
				if( pt->pt[i].PA == 1){
					min = i ;
					break;
				}
			}
			i++;
			for(;i<pt->size;i++){
				if((pt->pt[i].counter < pt->pt[min].counter) && pt->pt[i].PA == 1){
					min = i;
				}
			}
			from_where = min ;
			where_to = pt->pt[from_where].page_frame_number ;
		}
//		printf("where_to %d from_where %d\n",where_to , from_where );
	}
	pt->pt[from_where].page_frame_number = 0 ;
	pt->pt[from_where].PA = 0 ;
	pt->pt[from_where].R = 0 ;
	pt->pt[from_where].counter = 0 ;
	pt->pt[from_where].belongto = NULL ;
	
	if(pt->pt[from_where].M == 1){
		write_to_vertual(pt->diskfd,phy_mem,from_where,where_to,pt->frame_size);
		*is_written = 1;
	}
	pt->pt[from_where].M = 0;
	

	write_to_mem(pt->diskfd ,phy_mem , pn , where_to , pt->frame_size);
	pt->pt[pn].page_frame_number = where_to ;
	pt->pt[pn].PA = 1 ;
	pt->pt[pn].counter = 1 ;
	pt->pt[pn].belongto = tName ;
	
	
	return where_to;
}

/*
int WSClock(struct PAGE_TABLE* pt,int* phy_mem, int pn ,int* is_written, char* tName){
	printf("unimplemented algorithm (WSClock) %d \n",pt->size );
	return -1;
}
*/

void write_to_vertual(int fd , int* mem ,int to,int from,int fs){
	int i ;
	char temp[12];
	int f = from * fs ;
	lseek(fd, to*fs*11 ,SEEK_SET);
	for(i = 0 ; i < fs ; i++){
		sprintf(temp , "%10d\n" , mem[f+i]);
		write(fd , temp , 11);
	}


}

void write_to_mem(int fd ,int *mem,int from , int to , int fs){
	int i =0 , d;
	char temp[13];
	int t = to *fs ;
	lseek(fd, from*fs*11 ,SEEK_SET);
	for(i = 0 ; i < fs ; i++){
		temp[0]='\0';
		read(fd,temp,11);
		temp[11] = '\0'; 
		sscanf(temp,"%d",&d);
		mem[t+i] = d ;
		
	}		
}


void print_t(struct PAGE_TABLE* pt){
	printf("///////////////////////////////////////////////////////////////////////////////\n");
	printf("    | belongto |   counter     |   R   |   M   |   PA   |  Page frame number  | \n");
	int i ;
	for(i = 0 ; i < pt->size ; i++){
		printf("%4d|%10s|%12d   |   %d   |   %d   |    %d   |     %10d      | \n",i,pt->pt[i].belongto,pt->pt[i].counter , pt->pt[i].R , pt->pt[i].M , pt->pt[i].PA , pt->pt[i].page_frame_number);
	}
	printf("///////////////////////////////////////////////////////////////////////////////\n");
}

void reset_R(struct PAGE_TABLE *pt){
	int i ;
	for(i=0;i<pt->size;i++){
		pt->pt[i].R = 0 ;
	}
}

void print_l(struct LL *l){
	
	printf("list :: ");
	while(l){
		printf(" %d ",l->data );
		if(l->next){
			printf("->");
		}
		l = l->next;
	}
	printf("    .\n");
}

void _free(struct PAGE_TABLE* pt ,int *mem, char* tName){
	int i ;
	if(strcmp(pt->page_replacement_algorithm,"FIFO")== 0 || strcmp(pt->page_replacement_algorithm,"SC")== 0){
		while(pt->head != NULL){
			
			if(strcmp(pt->pt[pt->head->data].belongto,tName)==0){
				struct LL *temp = pt->head ;
				pt->head = pt->head->next ;
				free(temp);
			}else{
				break;
			}

		}
		if(pt->head){

			struct LL *l = pt->head->next;
			struct LL *l2 = pt->head ;
			while(l){
			
				if(strcmp(pt->pt[l->data].belongto,tName) == 0){
					l2->next = l->next ;
					free(l);
					l = l2->next;
				}else{
					l = l->next ;
					l2 = l2->next;
				}
			}
		}

	}
	for(i = 0 ; i < pt->size ; i++){
		if(pt->pt[i].PA == 1){
			if(strcmp(pt->pt[i].belongto , tName)==0){
				if(pt->pt[i].M == 1){
					write_to_vertual(pt->diskfd,mem,i,pt->pt[i].page_frame_number,pt->frame_size);
				}
				pt->pt[i].R = 0;
				pt->pt[i].M = 0;
				pt->pf[pt->pt[i].page_frame_number] = 0 ;
				pt->pt[i].page_frame_number = 0 ;
				pt->pt[i].belongto = NULL ;
				pt->pt[i].PA = 0 ;
				pt->pt[i].counter = 0 ;
			}
		}
	}
}
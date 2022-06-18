#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
# include "PageTable.h"
# include "utilities.h"
# include "statistics.h"


void set(unsigned int index, int value, char * tName); 
int get(unsigned int index, char * tName);
int get_offset(int index);
int get_Vaddr(int index);
void quickSort(int low, int high);
int partition (int low, int high);
void mergesort(int i,int j);
void merge(int i1,int j1,int i2,int j2);

struct PAGE_TABLE pt ;
void* fill(void* arg);
void* bubble_sort(void* arg);
void* quick_sort(void* arg);
void* merge_sort(void* arg);
void* index_sort(void* arg);

void* check(void* arg);

//void* bubble_sort2(void* arg);

struct statistics s[6] ;
int *physical_mem;
int mem_access = 0 ;



pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER ;


int main(int argc , char* argv[]){
	int framesize , numPhysical , numVirtual , pageTablePrintInt ;
	char *pageReplacement , *allocPolicy , *diskFileName ;
	pthread_t fill_thread ;
	pthread_t check_thread ;
	
	pthread_t bs , qs , ms ,is;
	int i ;

	if(!parse_command_line(argc , argv , &framesize , &numPhysical , &numVirtual , &pageTablePrintInt , &pageReplacement , &allocPolicy , &diskFileName )){
		return 0 ;
 	}
 	if(strcmp(pageReplacement,"WSClock")== 0){
 		printf("the WSClock algorithm was not implemented !!!\nterminating the program !!\n");
 		return 0 ;
  	}
 	pt = create_table(power2(numVirtual),power2(numPhysical),power2(framesize),pageReplacement,diskFileName,pageTablePrintInt,allocPolicy);
 	physical_mem = (int*)malloc(sizeof(int)*(power2(framesize)*power2(numPhysical)));

 	/////////////////////////////////    fill thread      ///////////////////////////////////
 	
 	if(pthread_create(&fill_thread , NULL , &fill ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
 	pthread_join(fill_thread , NULL);
	
	
 	////////////////////////////////////////////////////////////////////////////////////////
 	
 	if(pthread_create(&bs , NULL , &bubble_sort ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
 	
 	if(pthread_create(&qs , NULL , &quick_sort ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
 	
 	if(pthread_create(&ms , NULL , &merge_sort ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
 	
 	if(pthread_create(&is , NULL , &index_sort ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
	
 	pthread_join(bs , NULL);
	pthread_join(qs , NULL);
	pthread_join(ms , NULL);
	pthread_join(is , NULL);
	

	if(pthread_create(&check_thread , NULL , &check ,(void*)&pt ) != 0){
 		printf("ERROR creating fill thread !!!\n");
 		exit(0);
 	}
 	pthread_join(check_thread , NULL);
 	
	print_s(s[0],"fill");
	print_s(s[1],"bubbble");
	print_s(s[2],"quick");
	print_s(s[3],"merge");
	print_s(s[4],"index");
	print_s(s[5],"check");
	for(i = 0 ; i < pt.size ; i++){
 		if(pt.pt[i].PA == 1){
 			write_to_vertual(pt.diskfd , physical_mem , i , pt.pt[i].page_frame_number , pt.frame_size );
 			pt.pt[i].PA = 0 ;
 		}
 	}

 	free(physical_mem);
 	free_page_table(&pt);
	return 0 ;
}



int get(unsigned int index, char * tName){
	int V_ADDR = get_Vaddr(index);
	int offset = get_offset(index);
	int physicaladdr = -1;
	int is_written = 0 ;
	int page_miss = 0 ;
	int ret ;
	if(V_ADDR == -2 ){
		printf("ERROR :: set() function !!\n");
		return -1;
	}
	mem_access++;
	if((physicaladdr = is_page_in_table(pt,V_ADDR)) != -1){
	//	printf("get the page is in the table \n");
		physicaladdr *= pt.frame_size ;
		physicaladdr += offset ;
		pt.pt[V_ADDR].R = 1 ; 
		if(strcmp(pt.page_replacement_algorithm,"LRU")==0 ){
			pt.pt[V_ADDR].counter += 1 ;
		}
		ret = physical_mem[physicaladdr];
	}else{
	//	printf("get the page is not in the table :: page fault !!\n");
		physicaladdr = pt.pagereplacementalgo(&pt,physical_mem,V_ADDR , &is_written , tName );
		physicaladdr *= pt.frame_size ;
		physicaladdr += offset ;
		pt.pt[V_ADDR].R = 1 ; 
		ret = physical_mem[physicaladdr] ;
		
		page_miss = 1 ;
	}
	if(mem_access >= pt.pageTablePrintInt){
		print_t(&pt);
		if(strcmp(pt.page_replacement_algorithm,"NRU")==0 ){
			reset_R(&pt);
		}
		mem_access = 0 ;
	}
	if(strcmp(tName,"fill")==0){
		addtostatistics(&s[0] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"bubble")==0){
		addtostatistics(&s[1] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"quick")==0){
		addtostatistics(&s[2] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"merge")==0){
		addtostatistics(&s[3] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"index")==0){
		addtostatistics(&s[4] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"check")==0){
		addtostatistics(&s[5] , 1 , 0 , page_miss , page_miss , is_written , page_miss);
	}
	return ret ;
}



void set(unsigned int index, int value, char * tName){
	int V_ADDR = get_Vaddr(index);
	int offset = get_offset(index);
	int physicaladdr = -1;
	int page_miss = 0 ;
	int is_written = 0 ;
	if(V_ADDR == -2 ){
		printf("ERROR :: set() function !!\n");
		return ;
	}
	mem_access++;
	if((physicaladdr = is_page_in_table(pt,V_ADDR)) != -1){
		//printf("the page is in the table \n");
		physicaladdr *= pt.frame_size ;
		physicaladdr += offset ;
		physical_mem[physicaladdr] = value ;
		if(strcmp(pt.page_replacement_algorithm,"LRU")==0 ){
			pt.pt[V_ADDR].counter += 1 ;
		}

		pt.pt[V_ADDR].M = 1 ; 
		
	}else{
	//	printf("the page is not in the table :: page fault !!\n");
		physicaladdr = pt.pagereplacementalgo(&pt,physical_mem,V_ADDR ,&is_written , tName);
		physicaladdr *= pt.frame_size ;
		physicaladdr += offset ;
		physical_mem[physicaladdr] = value ;
		pt.pt[V_ADDR].M = 1 ;
		
		
		page_miss = 1 ; 
	}
	if(mem_access >= pt.pageTablePrintInt){
		print_t(&pt);
		if(strcmp(pt.page_replacement_algorithm,"NRU")==0 ){
			reset_R(&pt);
		}
		mem_access = 0 ;
	}
	if(strcmp(tName,"fill")==0){
		addtostatistics(&s[0] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"bubble")==0){
		addtostatistics(&s[1] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"quick")==0){
		addtostatistics(&s[2] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"merge")==0){
		addtostatistics(&s[3] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"index")==0){
		addtostatistics(&s[4] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}else if(strcmp(tName,"check")==0){
		addtostatistics(&s[5] , 0 , 1 , page_miss , page_miss , is_written , page_miss);
	}
	
}


int get_offset(int index){
	return index%pt.frame_size ;
}

int get_Vaddr(int index){
	int ret ;
	ret = index/pt.frame_size;
	if(ret >= pt.size){
		return -1 ;
	}
	return ret ;
}


void* fill(void* arg){
	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int i ;
	int s = t->size * t->frame_size ;
	srand(1000);

	for(i = 0 ; i < s ; i++ ){
		/*
		if(i%2 == 0)
			set(i , (i+1), "fill");
		else 
			set(i , (i-1) , "fill");
		*/
		set(i , rand() , "fill");		
	}
	_free(&pt,physical_mem,"fill");
	return NULL ;
}

void* check(void* arg){
	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int i , flag = 1 ;
	int interval = (t->size*t->frame_size/4) ;

	for(i = 0 ; i< interval -1; i++){
		if(get(i,"check") > get(i+1,"check")){
			printf(" un sorted element was detected   i = %d\n",i);
			flag = 0;
		}
	}
	i++;
	for( ; i< (interval*2)-1 ; i++){
		if(get(i,"check") > get(i+1,"check")){
			printf(" un sorted element was detected   i = %d\n",i);
			flag = 0;
		}
	}
	i++;
	for( ; i< (interval*3 )-1; i++){
		if(get(i,"check") > get(i+1,"check")){
			printf(" un sorted element was detected   i = %d\n",i);
			flag = 0;
		}
	}
	i++;
	for( ; i< (interval*4) -1; i++){
		if(get(i,"check") > get(i+1,"check")){
			printf(" un sorted element was detected   i = %d\n",i);
			flag = 0;
		}
	}
	if(flag){
		printf("checked and the elements are sorted .\n");
	}
	return NULL ;
}

void* bubble_sort(void* arg){
	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int i = 0 ;
	int j = 0 ;

	int l = (t->size*t->frame_size/4) ;

	int temp1 , temp2 ;
	for(i = 0 ; i < l ; i++){
//		printf("im here      bubble %d   %d    %d\n",mem_access , i , l);
		for(j = 0 ; j < l-i-1 ; j++){
			pthread_mutex_lock(&m);
			temp1 = get(j,"bubble");
			temp2 = get(j+1,"bubble");
			pthread_mutex_unlock(&m);
			if(temp1 > temp2){
				pthread_mutex_lock(&m);
				set(j,temp2,"bubble");
				set(j+1 , temp1 ,"bubble");
				pthread_mutex_unlock(&m);			
			}			
		}
	}
	/*
	pthread_mutex_lock(&m);
	//_free(&pt,physical_mem,"bubble");
	pthread_mutex_unlock(&m);
	*/
	return NULL ;
}

void* merge_sort(void* arg){
	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int start = (t->size*t->frame_size/2);
	int end = (3*t->size*t->frame_size/4);

	mergesort(start,end-1);
	pthread_mutex_lock(&m);
	//_free(&pt,physical_mem,"merge");
	pthread_mutex_unlock(&m);
	return NULL;
}

void* quick_sort(void* arg){

	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int start = (t->size*t->frame_size/4) ;
	int end = (t->size*t->frame_size/2) ;

	quickSort(start,end-1);
	/*
	pthread_mutex_lock(&m);
	//_free(&pt,physical_mem,"quick");
	pthread_mutex_unlock(&m);
	*/
	return NULL ;
}

void* index_sort(void* arg){
	struct PAGE_TABLE* t = (struct PAGE_TABLE*)arg;
	int start = 0 ;
	int end = (t->size*t->frame_size/4) ;
	
	int i , j;
	int *index = (int*)malloc(sizeof(int)*(end-start+1));
	for(i = 0;i<end-start;i++){
		index[i] = i + (3*t->size*t->frame_size/4) ;
	} 
	for(i = 0 ; i < end - 1 ; i++){
		for(j = i+1 ; j < end ; j++){
			int temp ;
			pthread_mutex_lock(&m);
			if(get(index[i],"index")>get(index[j],"index")){
				temp = index[i];
    			index[i] = index[j];
    			index[j] = temp;
			}
			pthread_mutex_unlock(&m);
		}
	}
	
	
	for(i = 0 ; i < end-start ; i++){
		pthread_mutex_lock(&m);

		int temp = get(index[i],"index");
		//int temp1 = get(i+(3*t->size*t->frame_size/4),"index");
		
		set(i+(3*t->size*t->frame_size/4),temp,"index");
		//set(index[i],temp1,"index");
		
		//temp = index[i];



		pthread_mutex_unlock(&m);
		 
	}
	
	free(index);	
	return NULL ;
}

void quickSort(int low, int high){ 
    if (low < high) 
    { 
        int pi = partition( low, high); 
  
        quickSort(low, pi - 1); 
        quickSort(pi + 1, high); 
    } 
} 

int partition (int low, int high){
	int temp1 , temp2 ; 
	int j ;
	pthread_mutex_lock(&m);
    int pivot = get(high,"quick"); 
    pthread_mutex_unlock(&m);
    
    int i = (low - 1); 
  
    for (j = low; j <= high- 1; j++) 
    { 
    	pthread_mutex_lock(&m);
    	temp2 = get(j,"quick");
        if (temp2 < pivot){ 
            i++; 
            temp1 = get(i,"quick");
            set(i,temp2,"quick");
            set(j,temp1,"quick");
        }
        pthread_mutex_unlock(&m);    	 
    } 
    pthread_mutex_lock(&m);
    temp1 = get(i+1,"quick");
    temp2 = get(high,"quick");
    set(i+1 , temp2 ,"quick");
    set(high, temp1 ,"quick");    
    pthread_mutex_unlock(&m);     
    return (i + 1); 
} 


void mergesort(int i,int j){
	int mid;
		
	if(i<j)
	{
		mid=(i+j)/2;
		mergesort(i,mid);
		mergesort(mid+1,j);
		merge(i,mid,mid+1,j);
	}
}
 
void merge(int i1,int j1,int i2,int j2){
	int *temp;	
	int temp1,temp2;
	int i,j,k;
	i=i1;	
	j=i2;	
	k=0;
	temp = (int*)malloc(sizeof(int)*(j2-i1+1));

	while(i<=j1 && j<=j2){
		pthread_mutex_lock(&m);
    	temp1 = get(i,"merge");
    	temp2 = get(j,"merge");
    	pthread_mutex_unlock(&m);
    	
		if(temp1<temp2){
			temp[k++] = temp1;
			i++;
		}
		else{
			temp[k++]=temp2;
			j++;
		}
	}	
	while(i<=j1){	//copy remaining elements of the first list
		pthread_mutex_lock(&m);
		temp1 = get(i++,"merge");
		pthread_mutex_unlock(&m);
		temp[k++]=temp1;
	}	
	while(j<=j2){	//copy remaining elements of the second list
		pthread_mutex_lock(&m);
		temp2 = get(j++,"merge");
		pthread_mutex_unlock(&m);
		temp[k++]= temp2;
	}
	
	for(i=i1,j=0;i<=j2;i++,j++){
		pthread_mutex_lock(&m);
		set(i,temp[j],"merge");
		pthread_mutex_unlock(&m);
	}
	free(temp);
}


/*
 	printf("framesize         = %d\n",framesize );
 	printf("numPhysical       = %d\n",numPhysical );
 	printf("numVirtual        = %d\n",numVirtual );
 	printf("pageTablePrintInt = %d\n",pageTablePrintInt );
 	printf("pageReplacement   = %s\n",pageReplacement );
 	printf("allocPolicy       = %s\n",allocPolicy );
 	printf("diskFileName      = %s\n",diskFileName );
*/
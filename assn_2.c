/*
 * Name: Ravi Pratap Singh
 * UnityId: rpsingh3
 * Assignment_2
 * CSC 541
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	int key;   // Record's key
	long offset;  //Record's offset in file
} index_S;

index_S prim_key[10000];
int index_size = 0;

typedef struct{
	int size;     // Hole's size 
	long offset;    // Hole's offset in file 
} avail_S;

avail_S avail_holes[10000];
int avail_size = 0;

/* comparison for quick sort to sort */
int compare_sort(const void *p, const void *q) {
	avail_S a = *(avail_S *)p;
	avail_S b = *(avail_S *)q;	
	return (a.size - b.size);
}

/* comparison for quick sort to reverse sort */
int compare_reverse_sort(const void *p, const void *q) {
	avail_S a = *(avail_S *)p;
	avail_S b = *(avail_S *)q;
	return (b.size - a.size);
}

int compare_index(const void *p, const void *q) {
	index_S a = *(index_S *)p;
	index_S b = *(index_S *)q;
	return (a.key - b.key);
}

/* Function to search key using binary search */
int bin_search(int key) {
	int i=0, beg=0, end=index_size-1, mid=0;
	for(i=0;beg <= end;i++){
		mid = (beg + end)/2;
		if(prim_key[mid].key == key){
			return mid;
		} else if(prim_key[mid].key < key) {
			beg = mid + 1;
		} else {
			end = mid - 1;
		}
	}
	return -1;	
}

/* Function to insert the record in hole based on order type */
void insert(int rec_size, long offset,int order_type) {
	avail_holes[avail_size].size= rec_size;
	avail_holes[avail_size].offset= offset;
	avail_size++;
	if(order_type == 20)
		qsort (avail_holes, avail_size, sizeof(avail_S), compare_sort);
	else if(order_type == 30)
		qsort (avail_holes, avail_size, sizeof(avail_S), compare_reverse_sort);		
}

/* Function to add the record */
void add(int key, char* record, char* record_file,int order_type) { 
	long offset,hole_offset; 
	int rem_holes,i, j,check = -1,size,len;
	FILE *ptr;
	check = bin_search(key);
	if(check != -1) {
		printf("Record with SID=%d exists\n",key);
		return;
	}
	ptr = fopen(record_file, "r+b");
	if (ptr == NULL) {
		ptr = fopen( record_file, "w+b" );
		offset = 0;
	} else {
		len = strlen(record);
		len += sizeof(int);
		j = -1;
		for(i=0;i<avail_size;i++)
			if(avail_holes[i].size >= len) {
				j = i;
				break;
			}
	  	if(j == -1) {
			fseek(ptr, 0, SEEK_END);
			offset = ftell(ptr);
    		} else {
			offset = avail_holes[j].offset;
			size = strlen(record);
			size += sizeof(int);
			rem_holes = avail_holes[j].size - size;
			hole_offset = avail_holes[j].offset + size;

			for(i=j;i<avail_size-1;i++) {
				avail_holes[i].size = avail_holes[i+1].size;
				avail_holes[i].offset = avail_holes[i+1].offset;
			}
			avail_size = avail_size-1;
			if(rem_holes > 0) 
				insert(rem_holes, hole_offset,order_type);
		}
	}
	prim_key[index_size].offset = offset;
	prim_key[index_size].key = key;	
	index_size ++;
	
	qsort(prim_key,index_size,sizeof(index_S),compare_index);
	
	size = strlen(record);
	//printf("Record = %s\n",record);	
	fseek(ptr,offset,SEEK_SET);
	fwrite(&size, sizeof(int), 1, ptr);
	fwrite( record, 1, size, ptr );
	fclose(ptr);
}

/* Function to find the record */
void find(int key,char* record_file) {
	int size,pos;
	long j;
	char* buffer;
	FILE* ptr;
		 
	pos = bin_search(key);
	j = prim_key[pos].offset;
	if(pos!= -1) {
		ptr = fopen( record_file, "r+b" );
		fseek( ptr, j, SEEK_SET );
		fread( &size, sizeof(int), 1, ptr );
		buffer = (char*)malloc(size);
		fread( buffer, 1, size, ptr );
		printf("%s\n",buffer);
		fclose(ptr);			
	} else {
	     	printf("No record with SID=%d exists\n",key);
	}
}

/* Function to delete the record */
void delete(int key, char* record_file,int order_type) {
	int i,pos;
	long offset;
	FILE *ptr;
        int size;		 

	pos = bin_search(key);
	offset = prim_key[pos].offset;
	if(pos != -1) {
	 	for(i= pos;i<index_size-1;i++) {
			prim_key[i].key = prim_key[i+1].key;
		 	prim_key[i].offset = prim_key[i+1].offset;
		}
		index_size--;
		ptr = fopen( record_file, "r+b" );
		fseek( ptr, offset, SEEK_SET );
		fread( &size, sizeof(int), 1, ptr );
		size += sizeof(int);
		fclose(ptr);
		insert(size,offset,order_type);
	} else {
		printf("No record with SID=%d exists\n",key);
	}	
}

/* Function to check if the file exists */	
int file_exist(const char *fname) {
	FILE *file;
	if ((file = fopen(fname, "r")) != NULL) {
		fclose(file);
		return 1;
	}
	return 0;
}

/* Main function */
int main(int argc, char *argv[]) {
	int key,i=0,order_type,flag=0;
	int hole_spaces=0;
	char *token = NULL, *word[3] ;
	char rec_line[1000];
	FILE *ptr1;
	FILE *ptr2;
	if(strcmp(argv[1],"--first-fit")==0) {
		order_type = 10;
	} else if(strcmp(argv[1],"--best-fit")==0) {
		order_type = 20;
	} else if(strcmp(argv[1],"--worst-fit")==0) {
		order_type = 30;
	} else {
		printf("Wrong Arguments!!, Please provide first-fit or worst-fit or best-fit\n");
		exit(1);
	}
	if(!file_exist(argv[2])) {
		flag = 1;
	} else {	 
		if ((ptr1 = fopen( "index.bin", "r+b")) != NULL) {
			while(fread(&prim_key[index_size].key, sizeof(int), 1, ptr1)>0) {
				fread( &prim_key[index_size].offset, sizeof(long), 1, ptr1 );
				index_size++;
			}
			fclose(ptr1);
		}
		if ((ptr2 = fopen("availability.bin", "r+b")) != NULL) {
			while(fread(&avail_holes[avail_size].size, sizeof(int), 1, ptr2)>0) {
				fread(&avail_holes[avail_size].offset, sizeof(long), 1, ptr2 );
				avail_size++;
			}
			fclose(ptr2);
		}
	}	
	while(scanf("%[^\n]%*c",&rec_line)==1) {
		word[0]=NULL;	
       		char *s = rec_line;
		i=0;
		while((token = strtok(s," ")) != NULL) {
			s = NULL;
			word[i++] = token;
        }
	if(strcmp(word[0],"add")==0) {
		if(word[1] != NULL) {
			key = atoi(word[1]);
			add(key,word[2],argv[2],order_type);
		} else {
			printf("Wrong key!");
			break;
		}
        } else if(strcmp(word[0],"find")==0) {
		if(word[1] != NULL) {
			key = atoi(word[1]);
			find(key,argv[2]);
		} else {
			printf("Wrong key!");
			break;
		}		     	
        } else if(strcmp(word[0],"del")==0) {
		if(word[1] != NULL) {
			key = atoi(word[1]);
			delete(key,argv[2],order_type);
		} else {
			printf("Wrong key!");
			break;
		}		       	
        } else if(strcmp(word[0],"end")==0) {
        	break;
        } else{
		printf("Wrong command Input\n");
        }
    }

	printf("Index:\n");
	for(i=0;i<index_size;i++) {
		printf("key=%d: offset=%ld\n",prim_key[i].key,prim_key[i].offset);
	}
	printf("Availability:\n");
	for(i=0;i<avail_size;i++) {
		printf("size=%d: offset=%ld\n",avail_holes[i].size,avail_holes[i].offset);
    	}
	printf("Number of holes: %d\n",avail_size);
	for(i=0;i<avail_size;i++) {
		hole_spaces = avail_holes[i].size+hole_spaces;
	}	
	printf("Hole space: %d\n",hole_spaces);
	if( flag == 1)
		ptr1 = fopen( "index.bin", "w+b" );
	else
		ptr1 = fopen( "index.bin", "wb" );
	if (ptr1 != NULL) {
		for(i=0;i<index_size;i++) {
			fwrite( &prim_key[i].key, sizeof(int), 1, ptr1 );
			fwrite( &prim_key[i].offset, sizeof(long), 1, ptr1 );
		}
		fclose(ptr1);
	}
	if( flag == 1)
		ptr2 = fopen( "availability.bin", "w+b" );
	else
		ptr2 = fopen( "availability.bin", "wb" );
	if (ptr2 != NULL) {
		for(i=0;i<avail_size;i++) {
			fwrite( &avail_holes[i].size, sizeof(int), 1, ptr2 );
			fwrite( &avail_holes[i].offset, sizeof(long), 1, ptr2 );
		}
		fclose(ptr2);
	}
	return 0;
}



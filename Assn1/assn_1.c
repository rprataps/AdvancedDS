#include<stdio.h>
#include <sys/time.h> 
#include<string.h>
#include<stdlib.h>

int main(int argc, char *argv[]) {
FILE *ptr1,*ptr2;
int i=0,j=0;
int size1 = 0,size2 = 0;
ptr1 = fopen(argv[2],"rb");
if(!ptr1){
	printf("File does not exist\n");
	exit(1);
}
	
ptr2 = fopen(argv[3],"rb");
if(!ptr2){
	printf("File does not exist\n");
	exit(1);
}

fseek(ptr1, 0, SEEK_END); 
size1 = ftell(ptr1);
size1 = size1/sizeof(int);

fseek(ptr2, 0, SEEK_END); 
size2 = ftell(ptr2);
size2 = size2/sizeof(int);


int K[size1+1],S[size2+1],H[size2+1];
int beg=0,last=0,mid = 0;

if(argc != 4){
	printf("Need to pass correct parameters\n");
	exit(0);
}
	
for(i=0;i<size2;i++){
	H[i] = 0;
}
struct timeval tm1,tm2,tdiff; 

i = 0;
while(i<size2) {
	fseek( ptr2, i * sizeof( int ), SEEK_SET );
	fread( &S[i], sizeof(int ), 1, ptr2 ) ;
	i++;
}

 if(strcmp(argv[1],"--mem-lin") == 0) {
	i = 0;
	gettimeofday( &tm1, NULL ); 
	while(i<size1) {
		fseek( ptr1, i * sizeof( int ), SEEK_SET );
		fread( &K[i], sizeof( int ), 1, ptr1 ) ;
		i++;
	}
	i = 0;
	while(i<size2) {
		for(j=0;j<size1;j++)
			if(S[i] == K[j]) {
				H[i] = 1;
				break;
			}
		i++;
	}
} else if(strcmp(argv[1],"--mem-bin") == 0) {
	i = 0;
	gettimeofday( &tm1, NULL ); 
	while(i<size1) {
		fseek( ptr1, i * sizeof( int ), SEEK_SET );
		fread( &K[i], sizeof( int ), 1, ptr1 ) ;
		i++;
	}

	i = 0;
	while(i<size2) {
		for(beg=0,last=size1-1;beg<=last;){
			mid = (beg+last)/2;
			if(K[mid]==S[i]){
				H[i] = 1;
				beg = last + 1;
			} else if(K[mid] < S[i]) {
				beg = mid + 1;
			} else {
				last = mid - 1;
			}
		}
		i++;
	}
} else if(strcmp(argv[1],"--disk-lin") == 0) {	
	gettimeofday( &tm1, NULL ); 
	i = 0;
	int temp = 0;
	while(i<size2) {
		for(j=0;j<size1;j++){
			fseek( ptr1, j * sizeof( int ), SEEK_SET );
			fread( &temp, sizeof( int ), 1, ptr1 ) ;
			if(S[i] == temp) {
				H[i] = 1;
				break;
			}
		}
		i++;
	}	
} else if(strcmp(argv[1],"--disk-bin") == 0) {
	gettimeofday( &tm1, NULL );	
	i = 0;
	int temp = 0;
	while(i<size2) {
		for(beg=0,last=size1-1;beg<=last;){
			mid = (beg+last)/2;
			fseek( ptr1, mid * sizeof( int ), SEEK_SET );
			fread( &temp, sizeof( int ), 1, ptr1 ) ;
			if(temp==S[i]){
				H[i] = 1;
				beg = last + 1;
			} else if(temp < S[i]) {
				beg = mid + 1;
			} else {
				last = mid - 1;
			}
		}
		i++;
	}
} else {
	printf("Wrong options! Please pass valid option as second parameter\n");
	printf("--mem-lin\n--mem-bin\n--disk-lin\n--disk-bin\n");
	exit(1);
}

gettimeofday( &tm2, NULL ); 
float diff;
tdiff.tv_sec = tm2.tv_sec - tm1.tv_sec;
tdiff.tv_usec = tm2.tv_usec - tm1.tv_usec;

diff = tdiff.tv_sec + (float)(tdiff.tv_usec)/1000000.0;
tdiff.tv_sec = diff;
tdiff.tv_usec  = diff*1000000.0 - tdiff.tv_sec*1000000;

for(i=0;i<size2;i++){
	if(H[i] == 1)
		printf("%12d: Yes", S[i]); 
	else
		printf( "%12d: No", S[i] ); 
	printf("\n");
}

printf( "Time: %ld.%06ld\n", tdiff.tv_sec, tdiff.tv_usec );

fclose(ptr1);
fclose(ptr2);

return 0;
}

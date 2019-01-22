
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>



int main(void){
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec + tv.tv_usec);
    printf("%d\n",tv.tv_usec);
    return 0;
}
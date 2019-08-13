#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <iostream>
/*
//https://www.cnblogs.com/matthew-2013/p/4675366.html  S_IRWXU
// http://joe.is-programmer.com/posts/17463.html O_RDWR|O_CREAT
*/
int main(){
    int fd=0;
    if((fd=open("0_decode.yuv",O_RDONLY,S_IRGRP))<0){
        printf("open failed \n");
        return 0;
    }
    struct stat file_stat;
    if((fstat(fd,&file_stat))<0){
        printf("fstat failed \n");
        return 0;        
    }
    void *start_fp;
    if((start_fp=mmap(NULL,file_stat.st_size,PROT_READ,MAP_SHARED,fd,0))==MAP_FAILED){
        printf("mmap failed \n");
        return 0;         
    }
    std::cout<<"size "<<file_stat.st_size<<std::endl;
    return 0;
}

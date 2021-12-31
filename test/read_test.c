#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
    int block_size = 4096;//4K
    int len = 4096;//1G

    int raw_nvm_fd = open("/dev/pmem0",O_RDWR);

    if(raw_nvm_fd < 0){
        perror("open");
        return -1;
    }

    int offset = atoi(argv[1]);


    // char* buf1 = "hello world";
    // int len = strlen(buf1);
    // printf("%d\n",len);
    // len = pwrite(raw_ssd_fd,buf1,strlen(buf1),0);
    // printf("%d\n",len);

    // if(len <= 0){
    //     perror("pwrite");
    // }


    // char* buf1 = "hello2022";
    // // char buf1[10000];
    // len = pwrite(raw_nvm_fd,buf1,9,0);
    // printf("%d\n",len);
    // if(len <= 0){
    //     perror("pread");
    // }

    //fsync(raw_nvm_fd);

    char buf2[4096];

    char buf1[4096];
    memset(buf1,'o',4096);

    size_t offs = (size_t)offset*4096;

    printf("off is %ld\n",offs);
    
    len = pread(raw_nvm_fd,buf2,4096,offs);
    for(int i = 0; i < len; i++){
        putchar(buf2[i]);
    }
    printf("\nlen is %d\n",len);
    putchar(buf2[0]);
    return 0;
}


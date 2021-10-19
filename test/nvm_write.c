#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <emmintrin.h>
#include "mix_queue.h"

typedef struct task{
    size_t len;
    size_t offset;
    char* buffer;
}task_t;

int task_num = 0;
#define THREAD_NUM 4

int block_size = 4096;//4K
size_t len = (size_t)4096 * 1024 * 1024 * 32;// 128G

void* nvm_buffer;

char ch;

char buffer[4096];

#define uintptr_t unsigned long long
#define FLUSH_ALIGN 64

mix_queue_t queue[4];

int empty = 0;

static inline void flushopt(const void* addr, size_t len){
    uintptr_t uptr;

    for(uptr = (uintptr_t)addr & ~(FLUSH_ALIGN -1); uptr < (uintptr_t)addr + len; uptr += FLUSH_ALIGN){
        _mm_clflush((char*)uptr);
    }
}

static inline void ntstore32(const void* dst, const void* src, size_t len){
    for(size_t i = 0; i < len; i = i + 4){
        _mm_stream_si32((int*)(dst + i),*(int*)(src+i));
    }
}

void* pthread_write(void* arg){
    int idx = *(int*)arg;
    printf("thread %d start\n",idx);
    int l = 0;
    task_t* task = malloc(sizeof(task_t));
    while(1){
        //printf("[%d]:task %d\n",idx,i);
        l = mix_dequeue(queue+idx,task,1);

        if(l > 0){
            //printf("[task]: offset is %d\n",task->offset);
            ntstore32(nvm_buffer + task->offset,task->buffer,task->len);
        }else{
            if(empty) return;
        }


        //_mm_sfence();
        //memcpy(nvm_buffer + (size_t)i * block_size,buffer,block_size);
        //ntstore32(nvm_buffer + task->offset,task->buffer,task->len);
        //flushopt(nvm_buffer,block_size);
    }
}


int main(int argc, char** argv){
    if(argc < 2){
        printf("error argurements\n");
        return 0;
    }

    //thread_num = atoi(argv[1]);
    task_num = atoi(argv[1]);
    ch = argv[2][0];

    //printf("thread num is %d\n",thread_num);
    printf("task num is %d\n",task_num);
    printf("len is %llu\n",len);

    int nvm_fd = open("/dev/pmem0",O_RDWR);

    if(nvm_fd < 0){
        perror("open");
        return -1;
    }

    nvm_buffer = mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,nvm_fd,0);

    if(nvm_buffer == MAP_FAILED){
        perror("mmap");
        return -1;
    }

    memset(buffer,ch,4096);

    time_t start  = time(NULL);

    pthread_t pids[THREAD_NUM];
    int idx[THREAD_NUM];
    for(int i = 0; i < THREAD_NUM; i++){
        idx[i] = i;
        pthread_create(&pids[i],NULL,pthread_write,(void*)(idx+i));
    }

    for(int i = 0; i < 4; i++){
        queue[i] = *mix_queue_init(1024*sizeof(task_t),sizeof(task_t));
    }
    
    int pre_queue_idx = 0;
    int l = 0;
    task_t* task = malloc(sizeof(task_t));
    for(size_t i = 0; i < (size_t)task_num; i++){
        task->len = block_size;
        task->offset = i * block_size;
        task->buffer = buffer;
        while(l == 0){
            //printf("enqueue task %llu\n",i);
            l = mix_enqueue(queue + pre_queue_idx, task, 1);
            //if(l == 0) printf("retry task %d\n",i);
            pre_queue_idx = (pre_queue_idx + 1)%4;
        }
        l = 0;
    }

    free(task);
    empty = 1;

    printf("write success\n");

    for(int i = 0; i < THREAD_NUM; i++){
        pthread_join(pids[i],NULL);
    }

    time_t end = time(NULL);
    printf("time is %ld\n",end-start);
    return 0;
}


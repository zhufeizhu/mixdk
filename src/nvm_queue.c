#include "nvm_queue.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#include "nvm.h"


static mix_queue_t* nvm_queue;

/**
 * @param dst:读取的目标地址
 * @param len:读取的长度
 * @param offset:读取的偏移
**/
static inline size_t mix_read_from_nvm(void* dst, size_t len, size_t offset){
    return mix_nvm_read(dst,len,offset);
}

/**
 * @param src:写入的内容
 * @param len:写入的长度
 * @param offset:写入的偏移
**/
static inline size_t mix_write_to_nvm(void* src, size_t len, size_t offset){
    return mix_nvm_write(src,len,offset);
}

static void mix_submit_to_nvm(void* arg){
    int len = 0;
    int ret = 0;
    while(1){
        io_task_t* task = malloc(TASK_SIZE);
        len = mix_dequeue(nvm_queue,task,1);
        if (len < 1) {
            free(task);
            continue;
        }

        switch (task->opcode) {
            case MIX_READ:
            {
                ret = mix_read_from_nvm(task->buf,task->len,task->offset);
                break;
            };
            case MIX_WRITE:
            {
                ret = mix_write_to_nvm(task->buf,task->len,task->offset);
                break;
            };
            default:
                break;
        }
        task->ret = ret;
        task->on_task_completed(task);
        free(task);
    }
    return;
}

int mix_init_nvm_queue(unsigned int size, unsigned int esize){
    nvm_queue = mix_queue_init(size,esize);
    if(mix_nvm_init()){
        return -1;
    }
    pthread_t pid;
    if(pthread_create(&pid,NULL,(void*)mix_submit_to_nvm,NULL)){
        printf("create ssd queue failed\n");
        return -1;
    }
    return 0;
}

int mix_post_task_to_nvm(io_task_t* task){
    assert(task != NULL);
    int l = mix_enqueue(nvm_queue,task,1);
    return l;
}
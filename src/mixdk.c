#include "mixdk.h"

#ifndef _WIN32
#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>

#include "mix_task.h"
#include "scheduler.h"


int a = 1;

int mixdk_init(){
    mix_init_scheduler(1024*TASK_SIZE,TASK_SIZE,1024);
    return 0;
}

size_t mixdk_write(void* src, size_t len, size_t offset,int idx){
    assert(src != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = src;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_WRITE;
    task->task_index = idx;
    task->original_task = task;
    
    int ind = mix_post_task_to_io(task);
    if(ind < 0){
        return 0;
    }
    mix_wait_for_task_completed(ind);
    int ret =  task->ret;
    free(task);
    return ret; 
}


size_t mixdk_read(void* dst,size_t len, size_t offset,int idx){
    assert(dst != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = dst;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_READ;
    task->task_index = idx;
    
    int ind = mix_post_task_to_io(task);
    if(ind < 0){
        return 0;
    }
    mix_wait_for_task_completed(ind);
    int ret =  task->ret;

    free(task);
    return ret; 
}
#include "mixdk.h"

#ifndef _WIN32
#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "mix_task.h"
#include "scheduler.h"

int mixdk_init(){
    mix_init_scheduler(4096*TASK_SIZE,TASK_SIZE,4096);
    return 0;
}

size_t mixdk_write(void* src, size_t len, size_t offset, size_t flags, int idx){
    assert(src != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = src;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_WRITE|flags;
    task->task_index = idx;
    task->flag = NULL;

    //printf("offset is %llu\n",offset);
    
    int ind = mix_post_task_to_io(task);
    if(ind < 0){
        return 1;
    }
    //mix_wait_for_task_completed(ind);
    //int ret =  task->ret;
    free(task);
    return 0; 
    //return 0;
}

size_t mixdk_read(void* dst, size_t len, size_t offset, size_t flags, int idx){
    assert(dst != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = dst;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_READ|flags;
    task->task_index = idx;
    atomic_bool read_finish_flag = false;
    task->flag = &read_finish_flag;
    
    int ind = mix_post_task_to_io(task);
    if(ind < 0){
        return 1;
    }

    mix_wait_for_task_completed(&read_finish_flag);

    free(task);
    return 0; 
}
size_t mix_completed_task_num(){
    return get_completed_task_num();
}
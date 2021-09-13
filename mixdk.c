#include "mixdk.h"

#ifndef _WIN32
#include <unistd.h>
#endif

#include "mix_task.h"
#include "scheduler.h"


int mix_init(){
    mix_init_scheduler(1024*TASK_SIZE,TASK_SIZE,1024);
}

int mix_write(void* src,unsigned int len, unsigned int offset){
    assert(src != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = src;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_WRITE;
    
    int ind = mix_post_task_to_io(task);
    if(!ind){
        return 0;
    }
    mix_wait_for_task_completed(ind);
    return 0; 
}


int mix_read(void* dst,unsigned int len, unsigned int offset){
    assert(dst != NULL);
    assert(len > 0 && offset >=0);

    io_task_t* task = malloc(sizeof(io_task_t));
    task->buf = dst;
    task->len = len;
    task->offset = offset;
    task->opcode = MIX_WRITE;
    
    int ind = mix_post_task_to_io(task);
    if(!ind){
        return 0;
    }
    mix_wait_for_task_completed(ind);
    return 0; 
}
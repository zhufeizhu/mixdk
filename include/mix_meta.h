#ifndef MIX_META_H
#define MIX_META_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "mix_bitmap.h"
#include "mix_bloom_filter.h"
#include "mix_hash.h"
#include "mix_task.h"

#define SEGMENT_NUM 4

typedef struct free_segment {
    size_t size;                         //当前segment的总体的大小
    uint32_t block_num;                  //当前segment的block的个数
    atomic_bool migration;   //是否在执行迁移动作
    atomic_int_fast32_t used_block_num;  //当前segment被使用的block的个数
    // atomic_bool migration;               //是否正在迁移数据
    mix_bitmap_t* bitmap;  //描述当前segment的bitmap信息
} free_segment_t;

typedef struct mix_metadata {
    uint32_t offset;         //元数据区的偏移
    uint32_t size;           //元数据区的大小
    uint32_t block_num;      //元数据区block的个数
    uint32_t per_block_num;  //元数据区每个segment的block的个数
    mix_hash_t* hash[SEGMENT_NUM];        //全局的hash
    atomic_bool migration;   //是否在执行迁移动作
    mix_counting_bloom_filter_t* bloom_filter[SEGMENT_NUM];  //全局的bloom_filter
    free_segment_t segments[SEGMENT_NUM];       //四个free_segment
} mix_metadata_t;

bool mix_init_metadata(uint32_t block_num);

void mix_free_meatdata();

bool mix_init_free_segment(free_segment_t* segment, uint32_t block_num);

int mix_get_next_free_block(int idx);

void mix_clear_blocks(io_task_t* task);

void mix_free_free_segment(free_segment_t* segments);

int mix_buffer_block_test(uint32_t offset);

bool mix_write_redirect_block(int idx, uint32_t offset, int bit);

bool mix_has_free_block(int idx);

int mix_post_task_to_meta(io_task_t* task);

void mix_migrate();

#endif
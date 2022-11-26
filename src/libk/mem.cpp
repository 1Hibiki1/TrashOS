#include <libk/mem.h>
#include <kernel/debug.h>
#include <libk/string.h>
#include "../kernel/umm_malloc/umm_malloc.h"
#include <kernel/kernel.h>

extern int _ebss;

#define USE_UMM_MALLOC

#define HEAP_START ((((kernel::u32)&_ebss) & (~0b11)) + 4)  // end of bss section
#define MAX_MEM_SIZE 32768                          // 32k
#define KERNEL_STACK_SIZE 0xc00                     // 3k
#define MEM_END_ADR 0x20008000
#define MEM_START_ADR 0x20000000

// TODO: add check for kernel stack overflow
// TODO: replace void* in code with block_t
// TODO: make func arg naming convention (block/start) uniform
// TODO: Use HEADER_T and sizeof(HEADER_T) instead of HEADER_SIZE and
//  HEADER_SIZE_T. Same for other quantities */


/*
 * block structure for malloc:
 * 
 * malloc'd block:
 *  - base: 15 bits size + 1 bit allocated
 *  - remaining: payload
 * 
 * free block:
 *  - base: HEADER_SIZE bytes size of block + 1 bit allocated
 *  - base + HEADER_SIZE bytes: PTR_SIZE bytes pointer to next free block
 *  - (base + HEADER_SIZE + size - PTR_SIZE bytes) - PTR_SIZE bytes pointer to previous
 *                                     free block
 * Total min size: 2*PTR_SIZE + HEADER_SIZE
 */


/************* MALLOC macros, functions, and variables *************/
#define HEADER_SIZE 4
#define HEADER_SIZE_T kernel::u32
#define PTR_SIZE 4
#define PTR_SIZE_T kernel::u32
#define MIN_BLOCK_SIZE (4*PTR_SIZE)
#define BLOCK_SIZE_T kernel::u16
#define MIN_SPLIT_THRES 16 + HEADER_SIZE

#define block_t void*

bool is_block(void* block);
void malloc_init_block(void* start, kernel::u32 size);

void malloc_set_block_free(void* start);
void malloc_set_block_allocated(void* start);
bool malloc_is_free(void* start);
bool malloc_is_allocated(void* start);

block_t malloc_get_next_block(void* block);
block_t malloc_get_prev_block(void* block);
void malloc_set_next_block(void* start, void* block);
void malloc_set_prev_block(void* start, void* block);

HEADER_SIZE_T malloc_get_header(block_t block);

BLOCK_SIZE_T malloc_get_size(void* block);
void malloc_set_size(void* start, BLOCK_SIZE_T sz);

void malloc_split_block(block_t block, kernel::u32 size);
void malloc_add_to_free_list(block_t block);
void malloc_remove_from_free_list(block_t block);

void malloc_coalesce(block_t blk);
bool malloc_is_left_blk_free(block_t blk);
bool malloc_is_right_blk_free(block_t blk);
void malloc_set_prev_free_bit(block_t blk, bool isfree);


// kernel::u32 available_mem = 0;
extern "C" uint32_t available_mem;
kernel::u32 max_available_mem = 0;
block_t last_free_block = nullptr;
block_t first_free_block = nullptr;
/******************************************************************/


/******************************************************************/
// debug functions
bool pointer_in_range(void* ptr);
bool is_in_free_list(block_t b);
/******************************************************************/


void kernel::mem_init(){
    kernel::u32 heap_end = MEM_START_ADR + (MEM_END_ADR - HEAP_START - KERNEL_STACK_SIZE);
    kernel::u32* i;

    for(i = (kernel::u32*)(HEAP_START); (kernel::u32)i <= heap_end; i++){
        *i = 0;
        if((*i) != 0)
            break;
    }

    available_mem = heap_end - HEAP_START;
    max_available_mem = available_mem;

    malloc_init_block((block_t)HEAP_START, (available_mem & ~(0b11)));
    last_free_block = (block_t)HEAP_START;
    first_free_block = last_free_block;
}


kernel::u32 kernel::get_available_mem(){
    return available_mem;
}

void* kernel::malloc(kernel::u32 size){
    #ifdef USE_UMM_MALLOC

    if(kernel::sched_started()){
        kernel::get_scheduler()->pause();
    }

    void* res = umm_malloc(size);

    if(kernel::sched_started()){
        kernel::get_scheduler()->run();
    }

    ASSERT(res != nullptr);

    return res;

    #else

    REQUIRES((MEM_START_ADR + size) < MEM_END_ADR);
    available_mem -= size;

    if(size == 0)
        return nullptr;

    block_t allocated_blk = nullptr;

    size = (
        (size%4 == 0) ? size : ((size & (~(0b11))) + 4) // align size to 4 byte boundary
    );

    if(size < MIN_BLOCK_SIZE)
        size = MIN_BLOCK_SIZE;

    for(block_t b = first_free_block; b != nullptr; b = malloc_get_next_block(b)){
        kernel::u32 blk_siz = malloc_get_size(b);

        if(blk_siz >= (size)){
            allocated_blk = b;

            if((blk_siz - size) >= MIN_SPLIT_THRES)
                malloc_split_block(allocated_blk, size);

            malloc_remove_from_free_list(allocated_blk);

            // return the address after header
            allocated_blk = (block_t)((kernel::u8*)allocated_blk + HEADER_SIZE);
            break;
        }
    }

    if(allocated_blk == nullptr)
        kernel::panic();

    ENSURES(allocated_blk == nullptr || pointer_in_range(allocated_blk));
    return allocated_blk;

    #endif
}

void kernel::free(void* ptr){
    #ifdef USE_UMM_MALLOC
    if(kernel::sched_started()){
        kernel::get_scheduler()->pause();
    }

    umm_free(ptr);

    if(kernel::sched_started()){
        kernel::get_scheduler()->run();
    }

    #else
    REQUIRES(ptr != nullptr && pointer_in_range(ptr));

    block_t start = (kernel::u8*)ptr - HEADER_SIZE;

    // if(is_in_free_list(start))
    // //! TODO: FIX THIS lmao
    //     return;

    malloc_add_to_free_list(start);
    // available_mem += malloc_get_size(start);
    malloc_coalesce(start);

    #endif
}


void kernel::memcpy(void *dest, void *src, kernel::u32 n){
    kernel::u8* csrc = (kernel::u8*)src;
    kernel::u8* cdest = (kernel::u8*)dest;

    for (kernel::u32 i=0; i<n; i++){
        cdest[i] = csrc[i];
    }
}

// from http://www.sunshine2k.de/articles/coding/cmemalloc/cmemory.html
void* kernel::realloc(void* ptr, kernel::size_t size){
    return umm_realloc(ptr, size);
    void* p;

    /* allocate new block of requested size */
    p = kernel::malloc(size);
    if (p == nullptr){
        return p;
    }

    /* get original size of memory block */
    kernel::size_t sizeToCopy;
    kernel::size_t originalSize = malloc_get_size(((kernel::u8*)ptr) - HEADER_SIZE);

    /* copy data of original memory block to new one - only valid data is copied */
    if (originalSize < size){
        sizeToCopy = originalSize;
    }
    else{
        sizeToCopy = size;
    }
    kernel::memcpy(p, ptr, sizeToCopy);

    /* free original block */
    kernel::free(ptr);

    return p;
}


void malloc_init_block(void* start, kernel::u32 size){
    REQUIRES(start != nullptr && pointer_in_range(start));

    *(HEADER_SIZE_T*)(start) = 0;
    malloc_set_size(start, size);
    malloc_set_next_block(start, nullptr);
    malloc_set_prev_block(start, nullptr);

    ENSURES(malloc_get_size(start) == size);
}

void malloc_set_block_free(void* start){
    REQUIRES(start != nullptr && pointer_in_range(start));

    *(HEADER_SIZE_T*)start &= ~(0x1);

    ENSURES(malloc_is_free(start));
}

void malloc_set_block_allocated(void* start){
    REQUIRES(start != nullptr && pointer_in_range(start));

    *(HEADER_SIZE_T*)start |= (0x1);

    ENSURES(!malloc_is_free(start));
}

bool malloc_is_free(void* start){
    REQUIRES(start != nullptr && pointer_in_range(start));

    bool res = !((malloc_get_header(start)) & 1);

    ENSURES(!res || is_in_free_list(start));
    return res;
}

bool malloc_is_allocated(void* start){
    REQUIRES(start != nullptr && pointer_in_range(start));

    bool res = (malloc_get_header(start)) & 1;

    ENSURES(res || !is_in_free_list(start));
    return res;
}


block_t malloc_get_next_block(void* block){
    REQUIRES(block != nullptr && pointer_in_range(block));
    // REQUIRES(is_in_free_list(block));

    kernel::u8* base = (kernel::u8*)block;
    block_t res = (block_t)(*(PTR_SIZE_T*)(base + HEADER_SIZE));

    ENSURES(res != block);
    ENSURES(res == nullptr || pointer_in_range(res));
    return res;
}

block_t malloc_get_prev_block(void* block){
    REQUIRES(block != nullptr && pointer_in_range(block));
    // REQUIRES(is_in_free_list(block));

    kernel::u8* base = (kernel::u8*)block;
    block_t res = (block_t)(*(PTR_SIZE_T*)(base + HEADER_SIZE +malloc_get_size(block) - 2*PTR_SIZE));

    ENSURES(res != block);
    ENSURES(res == nullptr || pointer_in_range(res));
    return res;
}

void malloc_set_next_block(void* start, void* block){
    REQUIRES(start != nullptr && pointer_in_range(start));
    REQUIRES(block == nullptr || pointer_in_range(block));
    REQUIRES(start != block);

    kernel::u8* base = (kernel::u8*)start;
    *(PTR_SIZE_T*)(base + HEADER_SIZE) = (PTR_SIZE_T)block;

    ENSURES(malloc_get_next_block(start) == block);
}

void malloc_set_prev_block(void* start, void* block){
    REQUIRES(start != nullptr && pointer_in_range(start));
    REQUIRES(block == nullptr || pointer_in_range(block));
    REQUIRES(start != block);

    kernel::u8* base = (kernel::u8*)start;
    *(PTR_SIZE_T*)(base + HEADER_SIZE + malloc_get_size(start) - 2*PTR_SIZE) = (PTR_SIZE_T)block;

    ENSURES(malloc_get_prev_block(start) == block);
}


BLOCK_SIZE_T malloc_get_size(void* block){
    REQUIRES(block != nullptr && pointer_in_range(block));

    BLOCK_SIZE_T res = ((malloc_get_header(block)) & (~0b11));

    ENSURES(res <= max_available_mem);
    ENSURES(res >= MIN_BLOCK_SIZE);
    return res;
}

void malloc_set_size(void* start, BLOCK_SIZE_T sz){
    REQUIRES((sz & 0b11) == 0);
    REQUIRES(start != nullptr && pointer_in_range(start));
    REQUIRES(sz <= max_available_mem);
    REQUIRES(sz >= MIN_BLOCK_SIZE);

    kernel::u8 allocated = malloc_get_header(start) & 1;
    kernel::u8 prev_allocated = (malloc_get_header(start) & 0b10);
    *(HEADER_SIZE_T*)start = (sz) | prev_allocated | allocated;
    *(BLOCK_SIZE_T*)((kernel::u8*)start + HEADER_SIZE + malloc_get_size(start) - sizeof(BLOCK_SIZE_T)) = (BLOCK_SIZE_T)sz;

    ENSURES(malloc_get_size(start) == sz);
}

HEADER_SIZE_T malloc_get_header(block_t block){
    REQUIRES(block != nullptr && pointer_in_range(block));

    HEADER_SIZE_T res = *(HEADER_SIZE_T*)block;

    ENSURES((kernel::u32)(res & ~0b11) <= max_available_mem);
    return res;
}

void malloc_split_block(block_t block, kernel::u32 size){
    REQUIRES(block != nullptr && pointer_in_range(block));
    REQUIRES(size < max_available_mem);
    REQUIRES(size < malloc_get_size(block));
    REQUIRES(is_in_free_list(block));

    kernel::u32 cur_size = malloc_get_size(block);
    kernel::u32 new_size = cur_size - size - HEADER_SIZE;
    block_t prev = malloc_get_prev_block(block);
    block_t next = malloc_get_next_block(block);

    malloc_set_size(block, size);
    malloc_set_prev_block(block, prev);
    malloc_set_next_block(block, next); //TODO: do we really need this??


    kernel::u8* base = (kernel::u8*)block;
    block_t new_block = (block_t)(base + HEADER_SIZE + size);
    malloc_init_block(new_block, new_size);
    malloc_add_to_free_list(new_block);


    ENSURES(is_in_free_list(new_block));
    ENSURES(is_in_free_list(block));
    ENSURES(malloc_get_next_block(new_block) == nullptr ||
            is_in_free_list(malloc_get_next_block(new_block)));
    ENSURES(malloc_get_size(block) + malloc_get_size(new_block) == cur_size - HEADER_SIZE);
}

void malloc_add_to_free_list(block_t block){
    REQUIRES(block != nullptr && pointer_in_range(block));
    REQUIRES(is_block(block));
    REQUIRES(!is_in_free_list(block));

    
    if(last_free_block != nullptr)
        malloc_set_next_block(last_free_block, block);

    malloc_set_prev_block(block, last_free_block);
    malloc_set_next_block(block, nullptr);

    if(first_free_block == nullptr) first_free_block = block;
    last_free_block = block;

    malloc_set_block_free(block);
    malloc_set_size(block, malloc_get_size(block));

    kernel::u8* next_blk = ((kernel::u8*)block + malloc_get_size(block) + HEADER_SIZE);
    if(pointer_in_range(next_blk) && (((*(HEADER_SIZE_T*)(next_blk)) & ~0b11) <= max_available_mem))
        malloc_set_prev_free_bit(next_blk, true);

    ENSURES(malloc_is_free(block));
    ENSURES(is_in_free_list(block));
}

void malloc_remove_from_free_list(block_t block){
    REQUIRES(block != nullptr && pointer_in_range(block));
    REQUIRES(is_in_free_list(block));

    block_t prev = malloc_get_prev_block(block);
    block_t next = malloc_get_next_block(block);

    if(prev != nullptr)
        malloc_set_next_block(prev, next);
    else
        first_free_block = next;  // it was the first block in list
    
    if(next != nullptr)
        malloc_set_prev_block(next, prev);
    else
        last_free_block = prev;  // it was the last block in list


    kernel::u8* next_blk = ((kernel::u8*)block + malloc_get_size(block) + HEADER_SIZE);
    if(pointer_in_range(next_blk) && (((*(HEADER_SIZE_T*)(next_blk)) & ~0b11) <= max_available_mem))
        malloc_set_prev_free_bit(next_blk, false);
    
    malloc_set_block_allocated(block);

    ENSURES(!malloc_is_free(block));
    ENSURES(!is_in_free_list(block));
}

void malloc_set_prev_free_bit(block_t blk, bool isfree){
    HEADER_SIZE_T head = malloc_get_header(blk);

    if(isfree)
        head |= 0b10;
    else
        head &= ~0b10;

    *(HEADER_SIZE_T*)blk = head;
}

bool malloc_is_left_blk_free(block_t blk){
    return (bool)((*(HEADER_SIZE_T*)blk & 0b10) >> 1);
}

bool malloc_is_right_blk_free(block_t blk){
    kernel::size_t sz = malloc_get_size(blk);
    block_t next_blk = (block_t)((kernel::u8*)blk + sz + HEADER_SIZE);
    return malloc_is_free(next_blk);
}

void malloc_coalesce(block_t blk){
    bool left_free = malloc_is_left_blk_free(blk);
    bool right_free = malloc_is_right_blk_free(blk);

    kernel::u8* base = (kernel::u8*)blk;
    kernel::size_t blk_size = malloc_get_size(blk);

    if(left_free && right_free){
        kernel::size_t left_size = (*(BLOCK_SIZE_T*)(base-sizeof(BLOCK_SIZE_T)));
        kernel::u8* left_blk = (base - left_size - HEADER_SIZE);

        kernel::u8* right_blk = (base + malloc_get_size(blk) + HEADER_SIZE);
        kernel::size_t right_size = malloc_get_size(right_blk);

        malloc_remove_from_free_list(left_blk);
        malloc_remove_from_free_list(right_blk);
        malloc_remove_from_free_list(blk);
        
        kernel::size_t new_sz = left_size + blk_size + right_size + 2*HEADER_SIZE;

        ASSERT(new_sz <= max_available_mem);

        malloc_init_block(left_blk, new_sz);
        malloc_add_to_free_list(left_blk);
    }

    else if(right_free){
        kernel::u8* right_blk = (base + malloc_get_size(blk) + HEADER_SIZE);
        kernel::size_t right_size = malloc_get_size(right_blk);

        malloc_remove_from_free_list(right_blk);
        malloc_remove_from_free_list(blk);
        
        kernel::size_t new_sz = blk_size + right_size + HEADER_SIZE;
        ASSERT(new_sz <= max_available_mem);

        malloc_init_block(blk, new_sz);
        malloc_add_to_free_list(blk);
    }

    else if(left_free){
        kernel::size_t left_size = (*(BLOCK_SIZE_T*)(base-sizeof(BLOCK_SIZE_T)));
        kernel::u8* left_blk = (base - left_size - HEADER_SIZE);

        malloc_remove_from_free_list(left_blk);
        malloc_remove_from_free_list(blk);
        
        kernel::size_t new_sz = left_size + blk_size + HEADER_SIZE;
        ASSERT(new_sz <= max_available_mem);

        malloc_init_block(left_blk, new_sz);
        malloc_add_to_free_list(left_blk);
    }

    // ASSERT(false);  // O_O
}


/******************************************************************/
/* debug functions definitions */

bool is_block(void* block){
    if(is_in_free_list(block)){
        bool sz_test = (
            malloc_get_size(block) ==
            *(BLOCK_SIZE_T*)((kernel::u8*)block + HEADER_SIZE + malloc_get_size(block) - sizeof(BLOCK_SIZE_T))
        );
        return  pointer_in_range(malloc_get_next_block(block)) &&
                pointer_in_range(malloc_get_prev_block(block)) &&
                sz_test;
    }

    return true;
}


bool pointer_in_range(void* ptr){
    return (((kernel::u32)ptr > MEM_START_ADR) && ((kernel::u32)ptr < MEM_END_ADR));
}

bool is_in_free_list(block_t b){
    for(block_t s = first_free_block; s != nullptr; s = malloc_get_next_block(s)){
        if(s == b) return true;
    }

    return false;
}


/******************************************************************/

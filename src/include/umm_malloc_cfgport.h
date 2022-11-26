#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int _ebss;

// ! This can be unaligned
// !! needs to be fixed
void* UMM_MALLOC_CFG_HEAP_ADDR = &_ebss;

uint32_t UMM_MALLOC_CFG_HEAP_SIZE = (28000);

#ifdef __cplusplus
}
#endif

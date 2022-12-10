//
// Created by ubuntu on 2022/12/10.
//

#ifndef MALLOC_DUMP_ANDROID_MALLOC_H
#define MALLOC_DUMP_ANDROID_MALLOC_H
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

// Structures for android_mallopt.

typedef struct {
    // Pointer to the buffer allocated by a call to M_GET_MALLOC_LEAK_INFO.
    uint8_t* buffer;
    // The size of the "info" buffer.
    size_t overall_size;
    // The size of a single entry.
    size_t info_size;
    // The sum of all allocations that have been tracked. Does not include
    // any heap overhead.
    size_t total_memory;
    // The maximum number of backtrace entries.
    size_t backtrace_size;
} android_mallopt_leak_info_t;

// Opcodes for android_mallopt.

enum {
    // Marks the calling process as a profileable zygote child, possibly
    // initializing profiling infrastructure.
    M_INIT_ZYGOTE_CHILD_PROFILING = 1,
#define M_INIT_ZYGOTE_CHILD_PROFILING M_INIT_ZYGOTE_CHILD_PROFILING
    M_RESET_HOOKS = 2,
#define M_RESET_HOOKS M_RESET_HOOKS
    // Set an upper bound on the total size in bytes of all allocations made
    // using the memory allocation APIs.
    //   arg = size_t*
    //   arg_size = sizeof(size_t)
    M_SET_ALLOCATION_LIMIT_BYTES = 3,
#define M_SET_ALLOCATION_LIMIT_BYTES M_SET_ALLOCATION_LIMIT_BYTES
    // Called after the zygote forks to indicate this is a child.
    M_SET_ZYGOTE_CHILD = 4,
#define M_SET_ZYGOTE_CHILD M_SET_ZYGOTE_CHILD

    // Options to dump backtraces of allocations. These options only
    // work when malloc debug has been enabled.

    // Writes the backtrace information of all current allocations to a file.
    // NOTE: arg_size has to be sizeof(FILE*) because FILE is an opaque type.
    //   arg = FILE*
    //   arg_size = sizeof(FILE*)
    M_WRITE_MALLOC_LEAK_INFO_TO_FILE = 5,
#define M_WRITE_MALLOC_LEAK_INFO_TO_FILE M_WRITE_MALLOC_LEAK_INFO_TO_FILE
    // Get information about the backtraces of all
    //   arg = android_mallopt_leak_info_t*
    //   arg_size = sizeof(android_mallopt_leak_info_t)
    M_GET_MALLOC_LEAK_INFO = 6,
#define M_GET_MALLOC_LEAK_INFO M_GET_MALLOC_LEAK_INFO
    // Free the memory allocated and returned by M_GET_MALLOC_LEAK_INFO.
    //   arg = android_mallopt_leak_info_t*
    //   arg_size = sizeof(android_mallopt_leak_info_t)
    M_FREE_MALLOC_LEAK_INFO = 7,
#define M_FREE_MALLOC_LEAK_INFO M_FREE_MALLOC_LEAK_INFO
    // Query whether the current process is considered to be profileable by the
    // Android platform. Result is assigned to the arg pointer's destination.
    //   arg = bool*
    //   arg_size = sizeof(bool)
    M_GET_PROCESS_PROFILEABLE = 9,
#define M_GET_PROCESS_PROFILEABLE M_GET_PROCESS_PROFILEABLE
    // Maybe enable GWP-ASan. Set *arg to force GWP-ASan to be turned on,
    // otherwise this mallopt() will internally decide whether to sample the
    // process. The program must be single threaded at the point when the
    // android_mallopt function is called.
    //   arg = android_mallopt_gwp_asan_options_t*
    //   arg_size = sizeof(android_mallopt_gwp_asan_options_t)
    M_INITIALIZE_GWP_ASAN = 10,
#define M_INITIALIZE_GWP_ASAN M_INITIALIZE_GWP_ASAN
    // Query whether memtag stack is enabled for this process.
    M_MEMTAG_STACK_IS_ON = 11,
#define M_MEMTAG_STACK_IS_ON M_MEMTAG_STACK_IS_ON
};
#endif //MALLOC_DUMP_ANDROID_MALLOC_H

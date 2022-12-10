#include <iostream>
#include <string>
#include <malloc.h>
#include <dlfcn.h>
#include <android_malloc.h>

using namespace  std;

static void *findSymbol(const char *path, const char *symbol) {
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        cout << "Handler is null" <<endl;
        return nullptr;
    }
    void *target = dlsym(handle, symbol);
    if (!target) {
        cout << "symbol is null" <<endl;
    }
    return target;
}
typedef bool (*android_mallopt_func_t)(int,void*,size_t);
int main() {
    android_mallopt_leak_info_t leak_info;
    void* func_ptr = findSymbol("libc.so", "android_mallopt");
    bool result = reinterpret_cast<android_mallopt_func_t>(func_ptr)(M_GET_MALLOC_LEAK_INFO,&leak_info,sizeof(leak_info));

    if (!result){
        cout << "MALLOC_LEAK FAILED!!!" << result << endl;
    }
    if (leak_info.buffer == nullptr || leak_info.overall_size == 0 || leak_info.info_size == 0
        || (leak_info.overall_size / leak_info.info_size) == 0) {
        cout << "MALLOC_LEAK NULL!!!" << endl;
    }

#define PRINT(fild) std::cout << #fild << leak_info.fild <<std::endl;
    PRINT(backtrace_size);
    PRINT(total_memory)
    PRINT(overall_size)
    return 0;
}


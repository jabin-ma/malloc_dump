#include <android_malloc.h>
#include <dirent.h>
#include <dlfcn.h>
#include <iostream>
#include <malloc.h>
#include <string>

using namespace std;

static void *findSymbol(const char *path, const char *symbol) {
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        cout << "Handler is null" << endl;
        return nullptr;
    }
    void *target = dlsym(handle, symbol);
    if (!target) {
        cout << "symbol is null" << endl;
    }
    return target;
}

int get_target_process_pid(const char *pkg_name) {
    int id;
    pid_t pid = -1;
    DIR *dir;
    FILE *fp;
    char cmdline[256];
    char filename[32];
    struct dirent *entry;

    if (pkg_name == NULL) {
        return -1;
    }

    dir = opendir("/proc");
    if (dir == NULL) {
        return -1;
    }
    while ((entry = readdir(dir)) != NULL) {
        id = atoi(entry->d_name);
        if (id != 0) {
            sprintf(filename, "/proc/%d/cmdline", id);
            fp = fopen(filename, "r");
            if (fp) {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);
                if (strcmp(cmdline, pkg_name) == 0) {
                    pid = id;
                    break;
                }
            }
        }
    }
    closedir(dir);
    return pid;
}

void *get_libs_addr(pid_t pid, const char *lib_name) {
    char maps_path[32];
    long addr = 0;
    if (pid < 0) {
        sprintf(maps_path, "/proc/self/maps");
    } else {
        sprintf(maps_path, "/proc/%d/maps", pid);
    }
    FILE *maps = fopen(maps_path, "r");
    char str_line[1024];
    cout << "get_libs_addr:" << maps_path << " lib:" << lib_name << endl;
    while (!feof(maps)) {
        fgets(str_line, 1024, maps);
        if (strstr(str_line, lib_name) != nullptr) {
            cout << "match:" << str_line << endl;
            fclose(maps);
            addr = strtoul(strtok(str_line, "-"), nullptr, 16);
            cout << "str addr" << addr <<endl;
            cout << " ptr " << (void *)addr << endl;
            if (addr == 0x8000)
                addr = 0;
            break;
        }
    }
    fclose(maps);
    cout << " ENDL " << endl;
    return (void *) addr;
}


void *get_remote_func_addr(void *local_lib_addr, void *local_func_addr, void *remote_func_addr) {
    return (void *) ((long) remote_func_addr + (long) local_func_addr - (long) local_lib_addr);
}

typedef bool (*android_mallopt_func_t)(int, void *, size_t);

const char *LIBC_NAME = "lib64/bionic/libc.so";
const char *LINKER_PATH = "bin/linker64";
int main() {
    pid_t my_pid = get_target_process_pid(nullptr);
    pid_t target_pid = get_target_process_pid("com.android.settings");

    void *remote_libc_addr = get_libs_addr(target_pid, LIBC_NAME);
    void *remote_linker_addr = get_libs_addr(target_pid, LINKER_PATH);
    void *local_libc_addr = get_libs_addr(my_pid, LIBC_NAME);
    void *local_linker_addr = get_libs_addr(my_pid, LINKER_PATH);

    cout << remote_libc_addr << endl;


    android_mallopt_leak_info_t leak_info;
    void *func_ptr = findSymbol("libc.so", "android_mallopt");

    cout << "android_mallopt ptr:" << func_ptr << endl;

    bool result = reinterpret_cast<android_mallopt_func_t>(func_ptr)(M_GET_MALLOC_LEAK_INFO, &leak_info, sizeof(leak_info));

    if (!result) {
        cout << "MALLOC_LEAK FAILED!!!" << result << endl;
    }
    if (leak_info.buffer == nullptr || leak_info.overall_size == 0 || leak_info.info_size == 0 || (leak_info.overall_size / leak_info.info_size) == 0) {
        cout << "MALLOC_LEAK NULL!!!" << endl;
    }

#define PRINT(fild) std::cout << #fild << leak_info.fild << std::endl;
    PRINT(backtrace_size);
    PRINT(total_memory)
    PRINT(overall_size)
    return 0;
}

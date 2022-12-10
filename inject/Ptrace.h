//
// Created by ubuntu on 2022/12/10.
//

#ifndef MALLOC_DUMP_PTRACE_H
#define MALLOC_DUMP_PTRACE_H
#include <sys/ptrace.h>
#include <iostream>
#include <wait.h>
class Ptrace {

private:
    const pid_t mPid;
public:
    Ptrace(int pid):mPid(pid){}
    bool attach() const;
    bool readAsm();
    bool call();
};
#endif //MALLOC_DUMP_PTRACE_H

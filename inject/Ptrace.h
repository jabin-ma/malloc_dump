//
// Created by ubuntu on 2022/12/10.
//

#ifndef MALLOC_DUMP_PTRACE_H
#define MALLOC_DUMP_PTRACE_H
#include <cstdio>
#include <cstdlib>
#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <elf.h>
#include <android/log.h>
#include <sys/uio.h>
#include <iostream>

#if defined(__aarch64__)
#define pt_regs         user_pt_regs
#define uregs	regs
#define ARM_pc	pc
#define ARM_sp	sp
#define ARM_cpsr	pstate
#define ARM_lr		regs[30]
#define ARM_r0		regs[0]
#define PTRACE_GETREGS PTRACE_GETREGSET
#define PTRACE_SETREGS PTRACE_SETREGSET
#endif
#define CPSR_T_MASK     ( 1u << 5 )
#define FUNCTION_NAME_ADDR_OFFSET       0x100
#define FUNCTION_PARAM_ADDR_OFFSET      0x200

class Ptrace {

private:
    const pid_t mPid;
    user_pt_regs ss;
    pt_regs mSavedReg,mCurrentReg;
    bool _continue();
public:
    Ptrace(int pid):mPid(pid){}
    bool attach() const;

    bool save();
    bool restore();

    bool writeReg(user_pt_regs *regs) const;
    bool readReg(user_pt_regs *regs) const;
    bool call(void *target_func_addr, const long *params, long params_length);
    bool writeData(const uint8_t *dest, const uint8_t *data, size_t size);
};
#endif //MALLOC_DUMP_PTRACE_H

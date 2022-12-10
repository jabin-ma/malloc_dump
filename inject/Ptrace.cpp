//
// Created by ubuntu on 2022/12/10.
//
#include "Ptrace.h"
using namespace std;


bool Ptrace::readReg(pt_regs *regs) const {
    iovec ioVec{};
    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_GETREGSET, mPid, /*NT_PRSTATUS*/ 1, &ioVec) < 0) {
        std::cerr << "Read reg failed!!" << std::endl;
        return false;
    }
    return true;
}

bool Ptrace::writeReg(pt_regs *regs) const {
    iovec ioVec{};
    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_SETREGSET, mPid, /*NT_PRSTATUS*/ 1, &ioVec) < 0) {
        std::cerr << "Set reg failed!!" << std::endl;
        return false;
    }
    return true;
}


bool Ptrace::writeData(const uint8_t *dest, const uint8_t *data, size_t size) {
#if defined(__arm__)
    uint32_t i, j, remain;
#elif defined(__aarch64__)
    uint64_t i, j, remain;
#endif

    const uint8_t *laddr;

    union u {
        long val;
        char chars[sizeof(long)];
    } d{};

    j = size / 4;
    remain = size % 4;

    laddr = data;

    //往内存中写入数据
    for (i = 0; i < j; i++) {
        memcpy(d.chars, laddr, 4);
        ptrace(PTRACE_POKETEXT, mPid, dest, d.val);
        dest += 4;
        laddr += 4;
    }

    //多出来的一小段
    if (remain > 0) {
        //d.val = ptrace(PTRACE_PEEKTEXT, pid, dest, 0);
        for (i = 0; i < remain; i++) {
            d.chars[i] = *laddr++;
        }
        ptrace(PTRACE_POKETEXT, mPid, dest, d.val);
    }
    return true;
}

bool Ptrace::attach() const {
    cout << "Attaching..." << endl;
    if (ptrace(PTRACE_ATTACH, mPid, NULL, 0) < 0) {
        std::cerr << "attach to " << mPid << " failed.";
        return false;
    }
    int status = 0;
    waitpid(mPid, &status, WUNTRACED);
    return true;
}


bool Ptrace::call(void *target_func_addr, const long *params, long params_length) {
    int i = 0;
#if defined(__arm__)
    int num_param_registers = 4;
#elif defined(__aarch64__)
    int num_param_registers = 8;
#endif

    for (i = 0; i < params_length && i < num_param_registers; i++) {
        mCurrentReg.uregs[i] = params[i];
    }
    if (i < params_length) {
        mCurrentReg.ARM_sp -= (params_length - i) * sizeof(long);// 分配栈空间，栈的方向是从高地址到低地址
        if (!writeData((uint8_t *) mCurrentReg.ARM_sp, (uint8_t *) &params[i], (params_length - i) * sizeof(long)))
            return false;
    }

#if defined(__arm__)
    regs->ARM_pc = (uint32_t) target_func_addr;
#elif defined(__aarch64__)
    mCurrentReg.ARM_pc = (uint64_t) target_func_addr;
#endif
    //  printf("pc point is %x\n", target_func_addr);
    if (mCurrentReg.ARM_pc & 1) {
        /* thumb */
        mCurrentReg.ARM_pc &= (~1u);
        mCurrentReg.ARM_cpsr |= CPSR_T_MASK;
    } else {
        /* arm */
        mCurrentReg.ARM_cpsr &= ~CPSR_T_MASK;
    }

    mCurrentReg.ARM_lr = 0;

    if (!writeReg(&mCurrentReg) || !_continue()) {
        printf("error\n");
        return -1;
    }

    int stat = 0;
    waitpid(mPid, &stat, WUNTRACED);
    while (stat != 0xb7f) {
        if (!_continue()) {
            printf("error\n");
            return -1;
        }
        waitpid(mPid, &stat, WUNTRACED);
    }
    return true;
}


bool Ptrace::_continue() {
    if (ptrace(PTRACE_CONT, mPid, NULL, 0) < 0) {
        printf("pid is %d\n", mPid);
        perror("ptrace_cont");
        return false;
    }
    return true;
}


bool Ptrace::save() {
    if (readReg(&mCurrentReg)) {
        // 保存原始值
        memcpy(&mSavedReg, &mCurrentReg, sizeof(mCurrentReg));
        return true;
    }
    return false;
}


bool Ptrace::restore() {
    // 回复寄存器
    if (writeReg(&mSavedReg)) {
        return true;
    }
    return false;
}

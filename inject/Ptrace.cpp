//
// Created by ubuntu on 2022/12/10.
//
#include "Ptrace.h"
using namespace std;
bool Ptrace::attach() const {
    cout << "Attaching..." <<endl;
    if (ptrace(PTRACE_ATTACH,mPid, NULL, 0) < 0) {
        std::cerr << "attach to " << mPid << " failed.";
        return false;
    }
    int status = 0;
    waitpid(mPid, &status, WUNTRACED);
    return true;
}

bool Ptrace::readAsm() {

    return false;
}

bool Ptrace::call() {
    return false;
}

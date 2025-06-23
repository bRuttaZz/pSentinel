#include "../include/trace.h"
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/user.h>

int trace_exec_calls(long long unsigned int syscall, struct user_regs_struct *regs) {
    switch (syscall) {
    case SYS_execve:
        printf("[SYS_EXECVE] called %llu\n", regs->orig_rax);
        return 0;

    case SYS_execveat:
        printf("[SYS_EXECVET] called\n");
        return 0;

    default:
        return 1;
    }
}

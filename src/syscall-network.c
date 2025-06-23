#include "../include/trace.h"
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/user.h>

int trace_network_calls(long long unsigned int syscall, struct user_regs_struct *regs) {
    switch (syscall) {
    case SYS_connect:
        printf("[SYS_CONNECT] called %llu\n", regs->orig_rax);
        return 0;

    case SYS_sendto:
        printf("[SYS_SENDTO] called\n");
        return 0;

    case SYS_recvfrom: // incomplete
        printf("[SYS_RECVFROM] recvfrom called\n");
        return 0;

    default:
        return 1;
    }
}

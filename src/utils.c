#include "../include/trace.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int _extract_str_arg(pid_t pid, unsigned long long int addr, char **dst) {
    size_t i = 0;
    size_t word_s = sizeof(long); // sizeof(long) -> size of a word
    size_t dst_size = word_s;
    long tmp;
    *dst = malloc(dst_size);
    if (dst == NULL) {
        perror("Insufficient Mem (alloc)");
        return errno;
    }

    while (1) {
        errno = 0;
        tmp = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
        if (errno != 0) {
            perror("PTRACE_PEEKDATA");
            free(*dst);
            return errno;
        }
        memcpy(*dst + i, &tmp, word_s);
        if (memchr(&tmp, 0, word_s))
            break;

        i += word_s;
        dst_size += word_s;
        *dst = realloc(*dst, dst_size);
        if (dst == NULL) {
            perror("Insufficient Mem");
            return errno;
        }
    }
    return 0;
}

int empty_trace_syscall_exit(long long unsigned int syscall, pid_t pid,
                             struct user_regs_struct *regs) {
    (void)syscall;
    (void)regs;
    int status;
    ptrace(PTRACE_SYSCALL, pid, 0, 0); // for the syscall end
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return abs(status);
    return -1;
}

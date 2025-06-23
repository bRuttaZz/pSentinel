#include "../include/conf.h"
#include "../include/trace.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>

int trace_exec_calls(long long unsigned int syscall, pid_t pid, struct user_regs_struct *regs) {
    int status;
    long return_;
    char *filename = NULL;

    switch (syscall) {
    case SYS_execve:
        // int execve(const char *filename, char *const argv[], char *const envp[]);
        fprintf(g_conf.log_file, "<exec data-cmd=");
        if (_extract_str_arg(pid, regs->rdi, &filename)) {
            fprintf(g_conf.log_file, "\"\"></exec>");
            return empty_trace_syscall_exit(syscall, pid, regs);
        }
        fprintf(g_conf.log_file, "\"%s\"", filename);
        free(filename);
        status = empty_trace_syscall_exit(syscall, pid, regs);
        if (status != -1)
            return status;

        // get sys call return code
        ptrace(PTRACE_GETREGS, pid, 0, regs);
        return_ = (long)regs->rax;
        if (return_ == 0) {
            fprintf(g_conf.log_file, "data-status=\"success\" data-code=\"%ld\"></exec>\n",
                    return_);
        } else {
            fprintf(g_conf.log_file, "data-status=\"failed\" data-code=\"%ld\"></exec>\n", return_);
        }
        return -1;

    case SYS_execveat:
        // int execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int
        // flags);
        fprintf(g_conf.log_file, "<exec data-cmd=");
        if (_extract_str_arg(pid, regs->rsi, &filename)) {
            fprintf(g_conf.log_file, "\"\"></exec>");
            return empty_trace_syscall_exit(syscall, pid, regs);
        }
        fprintf(g_conf.log_file, "\"%s\"", filename);
        free(filename);
        status = empty_trace_syscall_exit(syscall, pid, regs);
        if (status != -1)
            return status;

        // get sys call return code
        ptrace(PTRACE_GETREGS, pid, 0, regs);
        return_ = (long)regs->rax;
        if (return_ == 0) {
            fprintf(g_conf.log_file, "data-status=\"success\" data-code=\"%ld\"></exec>\n",
                    return_);
        } else {
            fprintf(g_conf.log_file, "data-status=\"failed\" data-code=\"%ld\"></exec>\n", return_);
        }
        return -1;

    default:
        return -2;
    }
}

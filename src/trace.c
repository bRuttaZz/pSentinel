#include "../include/trace.h"
#include "../include/conf.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

int (*syscall_tracers[])(long long unsigned int, pid_t, struct user_regs_struct *) = {
    trace_exec_calls,
    trace_network_calls,
    empty_trace_syscall_exit,
};
int syscall_tracers_count = 3;

int trace() {
    pid_t child = fork();
    if (child == 0) {
        // child
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGSTOP);
        execvp(g_conf.exec, g_conf.args);
        fprintf(stderr, "Error executing: '%s'. ", g_conf.exec);
        perror("");
        exit(errno);
    } else {
        // parent
        int status;
        int return_ = 0;

        waitpid(child, &status, 0);

        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
        while (1) {
            ptrace(PTRACE_SYSCALL, child, 0, 0); // ask the tracee to stop by syscall start
            waitpid(child, &status, 0);
            if (WIFEXITED(status))
                break;

            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child, 0, &regs);

            long long unsigned int syscall = regs.orig_rax;

            if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80)) {
                // syscall entry/exit (we look for entry here :)
                for (int i = 0; i < 2; i++) {
                    status = syscall_tracers[i](syscall, child, &regs);
                    if (status == -1) // case found
                        break;
                    else if (status > -1) { // prog exit status
                        return_ = 1;
                        break;
                    }
                }
            }
            if (return_)
                break;
        }
        fprintf(g_conf.log_file, "<netact data-sent=\"%.3f\" data-received=\"%.3f\"></netact>\n",
                (float)g_conf.data_sent / 1024.0, (float)g_conf.data_received / 1024.0);
        return status;
    }
}

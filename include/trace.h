#ifndef TRACE_H
#define TRACE_H

#include <signal.h>
#include <sys/user.h>

extern int (*syscall_tracers[])(long long unsigned int, pid_t, struct user_regs_struct *);

/**
 * Execute and and monitor sys calls set on `g_config.exec`
 * return status code of the tracee;
 */
int trace();

/**
 * Parse and record exec family of syscalls
 * Return -1 on successfull trace >=0 for exit signals <=-1 if not found
 */
int trace_exec_calls(long long unsigned int syscall, pid_t pid, struct user_regs_struct *regs);

/**
 * Parse and record network syscalls
 * Return -1 on successfull trace >=0 for exit signals <=-1 if not found
 */
int trace_network_calls(long long unsigned int syscall, pid_t pid, struct user_regs_struct *regs);

/**
 * Wait for sycall exit and do nothing ("take a 5star")
 * Return -1 on successfull trace >=0 for exit signals <=-1 if not found
 */
int empty_trace_syscall_exit(long long unsigned int syscall, pid_t pid,
                             struct user_regs_struct *regs);

/**
 * Extract string argument from pid at addr to put into dst.
 * Don't forget to deallocate dst mem
 */
int _extract_str_arg(pid_t pid, unsigned long long int addr, char **dst);

#endif // TRACE_H

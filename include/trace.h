#ifndef TRACE_H
#define TRACE_H

#include <sys/user.h>

extern int (*syscall_tracers[])(long long unsigned int, struct user_regs_struct *);

/**
 * Execute and and monitor sys calls set on `g_config.exec`
 * return status code of the tracee;
 */
int trace();

/**
 * Parse and record exec family of syscalls
 * Return 0 on successfull trace otherwise 1
 */
int trace_exec_calls(long long unsigned int syscall, struct user_regs_struct *regs);

/**
 * Parse and record network syscalls
 * Return 0 on successfull trace otherwise 1
 */
int trace_network_calls(long long unsigned int syscall, struct user_regs_struct *regs);

#endif // TRACE_H

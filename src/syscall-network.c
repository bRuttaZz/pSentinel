#include "../include/conf.h"
#include "../include/trace.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/user.h>

void _extract_sockaddr(pid_t pid, unsigned long addr, long long unsigned int addrlen) {
    size_t word_s = sizeof(long);
    struct sockaddr_in sa;
    if (addrlen < sizeof(sa)) {
        fprintf(g_conf.log_file, "<netcon ");
        return;
    }

    for (size_t i = 0; i < sizeof(sa); i += word_s) {
        *((long *)(((char *)&sa) + i)) = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
    }

    if (sa.sin_family == AF_INET) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa.sin_addr, ip, sizeof(ip));
        fprintf(g_conf.log_file, "<netcon data-ip=\"%s\" data-port=\"%d\" data-domain=", ip,
                ntohs(sa.sin_port));

        // get hostname if prensent in dns cache
        struct hostent *he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET);
        if (he) {
            fprintf(g_conf.log_file, "\"%s\"", he->h_name);
        } else {
            fprintf(g_conf.log_file, "\"\"");
        }
    } else if (sa.sin_family == AF_INET6) {
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &sa.sin_addr, ip, sizeof(ip));
        fprintf(g_conf.log_file, "<netcon data-ip=\"%s\" data-port=\"%d\" data-domain=", ip,
                ntohs(sa.sin_port));

        // get hostname if prensent in dns cache
        struct hostent *he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET6);
        if (he) {
            fprintf(g_conf.log_file, "\"%s\"", he->h_name);
        } else {
            fprintf(g_conf.log_file, "\"\"");
        }
    }
}

int trace_network_calls(long long unsigned int syscall, pid_t pid, struct user_regs_struct *regs) {
    int status;
    long return_;

    switch (syscall) {
    case SYS_connect:
        // int sys_connect(int sockfd, struct sockaddr __user *addr, int addrlen);
        _extract_sockaddr(pid, regs->rsi, regs->rdx);
        status = empty_trace_syscall_exit(syscall, pid, regs);
        if (status != -1) {
            fprintf(g_conf.log_file, "></exec>\n");
            return status;
        }
        // get sys call return code
        ptrace(PTRACE_GETREGS, pid, 0, regs);
        return_ = (long)regs->rax;
        if (return_ == 0) {
            fprintf(g_conf.log_file, "data-status=\"success\" data-code=\"%ld\"></exec>\n",
                    return_);
        } else if (return_ == -115) {
            fprintf(g_conf.log_file, "data-status=\"progress\" data-code=\"%ld\"></exec>\n",
                    return_);
        } else {
            fprintf(g_conf.log_file, "data-status=\"failed\" data-code=\"%ld\"></exec>\n", return_);
        }
        return -1;

    case SYS_sendto:
        g_conf.data_sent += (long)regs->rdx;
        return empty_trace_syscall_exit(syscall, pid, regs);

    case SYS_recvfrom: // incomplete
        g_conf.data_received += (long)regs->rdx;
        return empty_trace_syscall_exit(syscall, pid, regs);

    default:
        return -2;
    }
}

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

void print_execve_args(pid_t pid, unsigned long addr) {
    char *arg;
    unsigned long val;
    int i = 0;

    while (1) {
        ptrace(PTRACE_PEEKDATA, pid, addr + i * sizeof(void *), &val);
        if (val == 0)
            break;

        char buf[4096];
        int j = 0;
        while (j < 4095) {
            long c = ptrace(PTRACE_PEEKDATA, pid, val + j, NULL);
            if (c == -1)
                break;
            memcpy(buf + j, &c, sizeof(c));
            if (memchr(&c, 0, sizeof(c)))
                break;
            j += sizeof(c);
        }
        buf[4095] = '\0';
        printf("  Arg[%d]: %s\n", i, buf);
        i++;
    }
}

void print_sockaddr(pid_t pid, unsigned long addr, socklen_t addrlen) {
    struct sockaddr_in sa;
    if (addrlen < sizeof(sa))
        return;

    for (int i = 0; i < sizeof(sa); i += sizeof(long)) {
        *((long *)(((char *)&sa) + i)) = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
    }

    if (sa.sin_family == AF_INET) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa.sin_addr, ip, sizeof(ip));
        printf("  [NETWORK] connect() to IP: %s, Port: %d\n", ip, ntohs(sa.sin_port));

        struct hostent *he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET);
        if (he) {
            printf("  Hostname: %s\n", he->h_name);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program-to-trace> [args...]\n", argv[0]);
        return 1;
    }

    pid_t child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        kill(getpid(), SIGSTOP); // wait for parent to attach
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);

        ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD);

        while (1) {
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            waitpid(child, &status, 0);
            if (WIFEXITED(status))
                break;

            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child, NULL, &regs);

            long syscall = regs.orig_rax;

            // Entry point of syscall
            if ((status >> 8) & 0x80) {
                switch (syscall) {
                case SYS_execve:
                    printf("[EXEC] execve called with filename: ");
                    {
                        char filename[512] = {0};
                        for (int i = 0; i < sizeof(filename); i += sizeof(long)) {
                            long val = ptrace(PTRACE_PEEKDATA, child, regs.rdi + i, NULL);
                            memcpy(filename + i, &val, sizeof(val));
                            if (memchr(&val, 0, sizeof(val)))
                                break;
                        }
                        printf("%s\n", filename);
                    }
                    print_execve_args(child, regs.rsi);
                    break;
                case SYS_connect:
                    print_sockaddr(child, regs.rsi, regs.rdx);
                    break;
                case SYS_sendto:
                    printf("[NETWORK] sendto called (details not parsed in this version)\n");
                    break;
                case SYS_execveat:
                    printf("[EXEC] execveat called (details not fully parsed)\n");
                    break;
                default:
                    break;
                }
            }

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            waitpid(child, &status, 0);
            if (WIFEXITED(status))
                break;
        }
    }
    return 0;
}

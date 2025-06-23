#include "../include/conf.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *default_args[] = {NULL};
Conf g_conf = {.dump_file = "report.html", .args = default_args};

void parse_args(int argc, char *argv[]) {
    char *arg;
    for (int i = 1; i < argc; i++) {
        arg = argv[i];
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            g_conf.help = 1;
        } else if (!strcmp(arg, "-v") || !strcmp(arg, "--version")) {
            g_conf.version = 1;
        } else if (!strcmp(arg, "-f") || !strcmp(arg, "--follow")) {
            g_conf.follow = 1;
        } else if (!strcmp(arg, "-o") || !strcmp(arg, "--out")) {
            i++;
            g_conf.dump_file = argv[i];
        } else {
            g_conf.exec = arg;
            g_conf.args = &argv[i]; // 0th element will be the program itself
            break;
        }
    }
}

void collect_garbage(int sig) {
    if (g_conf.log_file != NULL) {
        if (sig != -99)
            fprintf(stderr, "[pSentinel] Closing report file..\n");
        fclose(g_conf.log_file);
        g_conf.log_file = NULL;
    }
    if (sig != -99)
        exit(130);
}

int _setup_output_file() {
    g_conf.log_file = fopen(g_conf.dump_file, "w");
    if (g_conf.log_file == NULL) {
        perror("Error opening report file");
        return 1;
    }
    signal(SIGINT, collect_garbage);
    return 0;
}

int validate_args() {
    if (g_conf.help) {
        fprintf(stdout, HELP);
        return 0;
    } else if (g_conf.version) {
        fprintf(stdout, "%s %s", PROG, VERSION);
        return 0;
    } else if (g_conf.exec == NULL) {
        fprintf(stderr, "Invalid usage!\n\n%s", HELP);
        return 64;
    }
    if (_setup_output_file())
        return 1;
    return -1;
}

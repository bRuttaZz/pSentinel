#include "../include/conf.h"
#include <stdio.h>
#include <string.h>

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
            g_conf.args = &argv[i + 1];
            break;
        }
    }
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
    return -1;
}

#ifndef CONFIGS_H
#define CONFIGS_H

#ifdef DATA_HEADER
#include "data.h"
#endif

#include <stdio.h>

#ifndef REPORT_HTML
// example report string : "<html><h1>pSentinel: Program Activity Report</h1></html>"
#define REPORT_HTML                                                                                \
    {                                                                                              \
        0x3c, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0x3c, 0x68, 0x31, 0x3e, 0x70, 0x53, 0x65, 0x6e, 0x74,  \
            0x69, 0x6e, 0x65, 0x6c, 0x3a, 0x20, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x61, 0x6d, 0x20,    \
            0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x20, 0x52, 0x65, 0x70, 0x6f, 0x72,    \
            0x74, 0x3c, 0x2f, 0x68, 0x31, 0x3e, 0x3c, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0x0a,    \
            0x00                                                                                   \
    }
#endif

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#define PROG "pSentinel"

#define HELP                                                                                       \
    "Usage: psentinel [options...] [cmd] [args]\n\n"                                               \
    "   -h, --help          Print help and exit\n"                                                 \
    "   -v, --version       Print program version and exit\n"                                      \
    "   -f, --follow        Follow all the nested syscalls\n"                                      \
    "   -o, --out           File path for html report. Defaults to 'report.html'\n"                \
    "\n"                                                                                           \
    "   cmd                 Executable to be monitored\n"                                          \
    "   args                Additional arguments for cmd\n"

typedef struct {
    int help;
    int version;
    int follow; // follow all the nested syscalls
    long data_sent;
    long data_received;
    char *dump_file; // output file
    char *exec;      // executable
    char **args;     // NULL terminated array of arguments
    FILE *log_file;
} Conf;

extern Conf g_conf;

/**
 * Parse cmd line arguments
 */
void parse_args(int argc, char *argv[]);

/**
 * Validate cmd line argument usage
 * @returns int
 *      - <0  : If validation succeed
 *      - >=0 : Exit status codes
 */
int validate_args();

/** Collect config garbage
 * if the `sig` set to -99 : will not exit with status
 */
void collect_garbage(int sig);

#endif // CONFIGS_H

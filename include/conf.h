#ifndef CONFIGS_H
#define CONFIGS_H

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
    int follow;      // follow all the nested syscalls
    char *dump_file; // output file
    char *exec;      // executable
    char **args;     // NULL terminated array of arguments
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

#endif // CONFIGS_H

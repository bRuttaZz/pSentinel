#include "include/conf.h"

int main(int argc, char *argv[]) {

    parse_args(argc, argv);
    int _status = validate_args();
    if (_status >= 0)
        return _status;

    return 0;
}

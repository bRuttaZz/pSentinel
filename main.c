#include "include/conf.h"
#include "include/trace.h"

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    int _status = validate_args();
    if (_status >= 0)
        return _status;
    _status = trace();
    collect_garbage(-99);
    return _status;
}

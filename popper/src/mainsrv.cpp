#include "server.h"

using namespace std;
using namespace srv;
using namespace msg;
using namespace lio;

int main(int argc, char* argv[]) {
    const auto print_help = [&]() {
        printf("USAGE:\t%s [-l logfile] [-f] [-c] [-v] [-u] [-h]\n"
               "\tlogfile<string>: all the errors will be logged here"
               " (defaults to %s)\n"
               "\t-f:              if the swpawned process fails just move to"
               " the next command\n"
               "\t                 (the default is to halt on error)\n"
               "\t-c:              if the swpawned process succedes move to"
               " the next command\n"
               "\t-u:              start in unsafe mode, to use with other"
               " resource limiters\n"
               "\t                 (the default is safe)\n"
               "\t-v:              print the version number\n"
               "\t-h:              print this message\n"
               "SCOPE:\texecutes a process with an input string got from a"
               " client as argv[1] if the string\n"
               "\tfollows a regex specification.\n", argv[0], global::stdlog);
    };
    bool force{false}, cont{false}, unsafe{false};
    char* logfname{nullptr};
    int opt;
    while ((opt = ansi::getopt(argc, argv, "hvfcul:")) != -1) {
        switch (opt) {
        case 'f':           // continue after error
            force = true;
            break;
        case 'c':           // continue after success
            cont = true;
            break;
        case 'l':           // log to file '-l file'
            logfname = ansi::optarg;
            break;
        case 'u':           // unsafe mode
            unsafe = true;
            break;
        case 'v':           // print version and exit
            printf("%s version: %s\n", argv[0], version);
            return 0;
        case 'h':           // print help and exit
            print_help();
            return 0;
        default:            // print help and die
            print_help();
            return 1;
        }
    }
    if (ansi::optind != argc) {
        print_help();
        return 1;
    }
    LockStream log(logfname ? logfname : global::stdlog);
    if (!indfork(unsafe)) {
        return 0;
    }
    try {
        Server(log, !cont, !force).run();
    } catch(exception& e) {
        log.timed("failed: "s + e.what());
        return 1;
    }
}

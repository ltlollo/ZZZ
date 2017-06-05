#include "compute.h"
#include "dircrap.h"

#include <iostream>
#include <extra/utils.h>

#include <unistd.h>

int main(int argc, char* argv[]) {
    const auto print_help = [&]() {
        std::cerr << "USAGE:\t" << argv[0] << " [-w Num] [-r] [-h]\n"
                     "\tNum<int>: show the first n elemtents"
                     " (defaults to 100)\n"
                     "\t-r:              reverse the ordering\n"
                     "\t-h:              print this message\n"
                     "SCOPE:\tFind files with similar names.\n"
                  << std::endl;
    };

    size_t window{100};
    auto files = fs::dir().list();
    bool reverse{false};
    int opt;

    while ((opt = getopt(argc, argv, "hrw:")) != -1) {
        switch (opt) {
        case 'r':
            reverse = true;
            break;
        case 'w':
            window = strtoul(optarg, nullptr, 0);
            break;
        case 'h':
        default:
            print_help();
            return 1;
        }
    }
    if (optind != argc || !window) {
        print_help();
        return 1;
    }
    cmp::compute_exp(files, window, reverse);
    return 0;
}

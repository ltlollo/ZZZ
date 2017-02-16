#include "client.h"

using namespace std;
using namespace cli;
using namespace msg;

int main(int argc, char* argv[]) {
    const auto print_help = [&]() {
        cerr << "USAGE:\t" << argv[0]
             << " input [-]\n"
                "\tinput<string>: the input to send to the server\n"
                "\t-:             forward the stdin constent to the server\n"
                "SCOPE:\tsends the input and the optional stdin to the popper\n"
                "\tserver via an unix socket, the server process it and"
                " responds\n"
                "\twhen finished, then the program returns.\n"
             << endl;
    };
    if (argc < 2) {
        print_help();
        return 1;
    }
    try {
        string msg(argv[1]);
        vector<char> input;
        make_absolute(msg);
        curate_url(msg);
        if (argc > 2 && string("-") == argv[2]) {
            cin >> noskipws;
            istream_iterator<char> begin(cin), end;
            input = vector<char>(begin, end);
        }
        Client(move(msg), move(input));
        return 0;
    } catch(exception& e) {
        cerr << "failed: " << e.what() << endl;
        return 1;
    }
}

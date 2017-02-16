#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <limits.h>

constexpr int OP_FIELDS{4};
constexpr int N_CHARS{UCHAR_MAX + 1};

namespace d_types
{

typedef std::array<unsigned char, N_CHARS> arr_uc;
typedef std::vector<arr_uc> v_arr;

struct Data
{
public:
    v_arr msg;
    v_arr key;
    v_arr alg;

    unsigned int argc{0};
    char **argv;
    bool work_on_stdin, work_on_stdout;
    bool encryption_flag, alg_on_alg_flag;
    std::string command, content;
    std::string msg_file, key_file, alg_file, out_file;

    arr_uc pop, guard;

    unsigned int message_end_pos;

    unsigned int times{256};
    static const unsigned int distance{1};

    Data();
    Data(int argc,char *argv[]);
};

}

#endif // DATA_H

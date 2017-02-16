#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>

#include "data.h"


struct op_t
{
    std::bitset<CHAR_BIT> visit_l;
    long pos_l;
    std::bitset<CHAR_BIT> visit_r;
    long pos_r;
};

enum intent
{
    intent_xor,
    intent_swap,
    intent_enc,
    intent_dec,
    intent_onalg,
    intent_not_onalg
};

int util_read_file(const std::string& filename, d_types::v_arr& v);

std::string util_get_arg(const std::string& command,
                         const std::string& flag,
                         const size_t& found
                         );

void alg_to_op_vector(const d_types::v_arr& alg,
                      std::vector<op_t>& v
                      );

template<intent op_mode, intent onalg>
void apply_round(d_types::arr_uc& chunk,
                 const d_types::arr_uc& key,
                 const std::vector<op_t>& alg
                 ) noexcept;


void apply_rounds(d_types::arr_uc& msg,
                  const d_types::arr_uc& key,
                  const std::vector<op_t>& alg,
                  const unsigned int times,
                  const bool encrypt,
                  const bool on_alg
                  ) noexcept;

void chunk_to_op_vector(const d_types::arr_uc& chunk,
                        std::vector<op_t>& v
                        );


#endif // UTILS_H

#include "utils.h"

using namespace std;
using namespace d_types;


static inline bitset<CHAR_BIT> to_bitmask(const unsigned char input) noexcept
{
    return bitset<CHAR_BIT>(input);
}

static inline long to_position(const unsigned char input) noexcept
{
    return long(input);
}

int util_read_file(const string& filename, v_arr& v)
{
    arr_uc u_array;
    char mmask[N_CHARS];
    int status{0};
    ifstream::pos_type size;
    ifstream file(filename, ios::in | ios::binary | ios::ate);

    size = file.tellg();
    file.seekg(0, ios::beg);

    if (file.is_open())
    {
        if (size > 0 && size % N_CHARS == 0)
        {
            v.reserve(size/N_CHARS);
            while (file.good())
            {
                file.read(mmask, N_CHARS);
                if (file.good())
                {
                    for (unsigned int i = 0u; i < N_CHARS; ++i)
                        u_array[i] = (unsigned char)mmask[i];
                    v.push_back(u_array);
                }
            }
            if (unsigned(size) == v.size()*N_CHARS)
                status = -1;
            else
                status = 0;
        }
        else
            status = 1;
        file.close();
    }
    else
        status = 2;
    return status;
}


string util_get_arg(const string& command,
                    const string& flag,
                    const size_t& found
                    )
{
    string temp{""};
    if (found + flag.size() < command.size())
        for (auto it = found + flag.size();
             command.at(it) != ' ' && command.at(it) != '-';
             ++it
             )
            temp += command.at(it);
    return temp;
}


void chunk_to_op_vector(const arr_uc& chunk,
                        vector<op_t>& v
                        )
{
    for (auto it = chunk.cbegin(); it != chunk.cend(); it+=OP_FIELDS)
    {
        op_t op{to_bitmask(*it),
                    to_position(*(it+1)),
                    to_bitmask(*(it+2)),
                    to_position(*(it+3))
               };
        v.push_back(move(op));
    }
}


void alg_to_op_vector(const v_arr& alg,
                      vector<op_t>& v
                      )
{
    for (const auto& it: alg) chunk_to_op_vector(it, v);
}


template<intent op_mode, intent onalg>
static inline void apply_op(arr_uc& chunk,
                            const arr_uc& key,
                            op_t op
                            ) noexcept
{
    long pos_l = op.pos_l;
    long pos_r = op.pos_r;   

    for (unsigned int i{0u}; i < CHAR_BIT; ++i)
    {
        if (op.visit_l[i]) pos_l = key[pos_l];
        else if (onalg == intent_onalg) pos_l = chunk[pos_l];
        if (op.visit_r[i]) pos_r = key[pos_r];
        else if (onalg == intent_onalg) pos_r = chunk[pos_r];
    }
    if (op_mode == intent_xor) chunk[pos_l] ^= key[pos_r];
    if (op_mode == intent_swap) swap(chunk[pos_l], chunk[pos_r]);
}

template<intent op_mode, intent onalg>
void apply_round(arr_uc& chunk,
                 const arr_uc& key,
                 const vector<op_t>& alg
                 ) noexcept
{
    for (const auto& it: alg)
    {
        apply_op<op_mode, onalg>(chunk, key, it);
    }
}

template<intent op_mode>
static inline void apply_shift(arr_uc& chunk) noexcept
{

    if (op_mode == intent_enc)
    {
        auto it = chunk.begin();
        auto temp = *it;
        for (; it != chunk.end()-1; ++it) *it = *(it+1);
        *it = move(temp);
    } else {
        auto it = chunk.rbegin();
        auto temp = *it;
        for (; it != chunk.rend()-1; ++it) *it = *(it+1);
        *it = move(temp);
    }
}

template<intent op_mode>
static inline void apply_step_shift(arr_uc& chunk,
                                    const unsigned int step,
                                    const unsigned int start
                                    )
{
    const unsigned int start_n = start % step;
    if (op_mode == intent_enc)
    {
        auto it = chunk.begin() + start_n;
        auto temp = *it;
        for (; it != chunk.end() - step + start_n; it+=step) *it = *(it+step);
        *it = move(temp);
    } else {
        auto it = chunk.rbegin() + (step - start_n - 1);
        auto temp = *it;
        for (; it != chunk.rend() - start_n - 1; it+=step) *it = *(it+step);
        *it = move(temp);
    }
}

template<intent op_mode>
static inline void apply_cshift(arr_uc& chunk)
{
    for (unsigned int i{0u}; i < CHAR_BIT; i+=2)
    {
        apply_step_shift<op_mode>(chunk, CHAR_BIT, i);
    }
}

void apply_rounds(arr_uc& msg,
                  const arr_uc& key,
                  const vector<op_t>& alg,
                  const unsigned int times,
                  const bool encrypt,
                  const bool on_alg
                  ) noexcept
{
    for (unsigned int i{0u}; i < times; ++i)
    {
        if (encrypt)
        {
            if (on_alg)
            {
                apply_round<intent_xor, intent_onalg>(msg, key, alg);
                apply_round<intent_swap, intent_onalg>(msg, key, alg);
            }
            else
            {
                apply_round<intent_xor, intent_not_onalg>(msg, key, alg);
                apply_round<intent_swap, intent_not_onalg>(msg, key, alg);
            }
            apply_cshift<intent_enc>(msg);
            apply_shift<intent_enc>(msg);
        }
        else
        {
            apply_shift<intent_dec>(msg);
            apply_cshift<intent_dec>(msg);
            if (on_alg)
            {
                apply_round<intent_swap, intent_onalg>(msg, key, alg);
                apply_round<intent_xor, intent_onalg>(msg, key, alg);
            }
            else
            {
                apply_round<intent_swap, intent_not_onalg>(msg, key, alg);
                apply_round<intent_xor, intent_not_onalg>(msg, key, alg);
            }
        }
    }
}

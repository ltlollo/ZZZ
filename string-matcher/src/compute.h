#ifndef COMPUTE_H
#define COMPUTE_H

#include <algorithm>

#include "work/workers.h"
#include "algos.h"

namespace cmp {

namespace {
using Data = work::cdata_t<std::string>;
}   // anon namespace

class Res {
    Data data;
    double mch;
public:
    Res(const Data& data, double mch) noexcept : data{data}, mch{mch} {
    }
    void print() const noexcept {
        printf("%s, %s is %f%%\n", work::first(data)->c_str(),
               work::second(data)->c_str(), mch*100);
    }
    void printn() const noexcept {
        printf("%s, %s is %f\n", work::first(data)->c_str(),
               work::second(data)->c_str(), mch);
    }
    bool operator>(const Res& rhs) const noexcept {
        return mch > rhs.mch;
    }
};

template<typename T, typename U>
inline Res norm_match(const T* f, const U* s) {
    double max = f->size() > s->size() ? f->size() : s->size(),
            match = algos::match_containers_sim(f, s);
    return Res(make_pair(f, s), max ? match/max : 0);
}

void compute(const std::vector<std::string>&, const size_t,
             const bool);

void compute_ssub_seq(const std::vector<std::string>& files,
                      const size_t window, const bool rev);

void compute_exp(std::vector<std::string>& files, const size_t,
                 const bool);

}

#endif // COMPUTE_H

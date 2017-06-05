#include "compute.h"
#include <iostream>
#include <extra/task.h>

using std::vector;
using std::string;
using std::tuple;
using std::get;

using namespace algos;
using namespace work;

namespace cmp {

void compute(const vector<string>& files, const size_t window,
             const bool rev) {
    if (files.empty()) {
        return;
    }
    const auto fun = [](Data p) noexcept {
        return norm_match(first(p), second(p));
    };
    vector<Data> data;
    const size_t size{files.size()};
    data.reserve(size*(size-1)/2);

    for(size_t i{0}; i < size-1; ++i) {
        for (size_t j{i+1}; j<size; ++j) {
            data.emplace_back(make_pair(&files[i], &files[j]));
        }
    }
    auto result = static_work_balancer(data, fun);

    auto order = [rev](const Res& f, const Res& s) {
        return rev ? s > f : f > s;
    };
    if (window < result.size()) {
        partial_sort(begin(result), begin(result)+window, end(result), order);
    } else {
        sort(begin(result), end(result), order);
    }
    for (size_t i{0}; i < window && i < result.size(); ++i) {
        result[i].print();
    }
}

auto compute_exp_h(const vector<string>& files) {
    const auto fun = [&](const string& s, size_t pos) {
        vector<double> v(files.size()-pos-1, 0);
        for (size_t i = 0; i < v.size(); ++i) {
            double ml = std::max(s.size(), files[pos+i+1].size());
            v[i] = match_containers(&s, &files[pos+i+1])/ml;
        }
        return v;
    };
    const auto filter = [size = files.size()](const string&, size_t pos)
                        noexcept {
        return pos < size - 1;
    };
    return task::Parallel<4>::collect_indexed(files, fun, filter);
}

void compute_exp(std::vector<std::string>& files, const size_t window,
                 const bool rev) {
    if (files.empty()) {
        return;
    }
    sort(begin(files), end(files), [](const auto& f, const auto& s){
        return f.size() < s.size();
    });
    using data = std::tuple<double, size_t, size_t>;
    auto v = compute_exp_h(files);
    vector<data> result;
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < v[i].size(); ++j) {
            result.emplace_back(std::make_tuple(v[i][j], i, i+j+1));
        }
    }
    const auto order = [rev](const data& f, const data& s) {
        return !(rev^(get<0>(s) > get<0>(f)));
    };
    if (window < result.size()) {
        partial_sort(begin(result), begin(result)+window, end(result), order);
    } else {
        sort(begin(result), end(result), order);
    }
    for (size_t i{0}; i < window && i < result.size(); ++i) {
        std::cout << files[get<1>(result[i])] << ' '
                                              << files[get<2>(result[i])] << ' '
                                              << get<0>(result[i]) << '\n';
    }
}

void compute_ssub_seq(const vector<string>& files, const size_t window,
                      const bool rev) {
    if (files.empty()) {
        return;
    }
    const auto fun = [](Data p) noexcept {
        return norm_match(first(p), second(p));
    };
    auto result = swork_balancer_sub_seq(files, fun);

    const auto order = [rev](const Res& f, const Res& s) {
        return rev ? s > f : f > s;
    };
    if (window < result.size()) {
        partial_sort(begin(result), begin(result)+window, end(result), order);
    } else {
        sort(begin(result), end(result), order);
    }
    for (size_t i{0}; i < window && i < result.size(); ++i) {
        result[i].print();
    }
}

}

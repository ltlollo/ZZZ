#ifndef WORKERS_H
#define WORKERS_H

#include <vector>
#include <future>

#include "ranges.h"

namespace work {


namespace sched {

struct sequencial {};
struct roundrobin {};

}   // sched namespace

template<typename T> using data_t = std::pair<T*, T*>;
template<typename T> using cdata_t = data_t<const T>;
using range::indexes;
using range::tiny_t;
using range::t_of;
template<tiny_t N> struct Num{};
constexpr tiny_t nthreads{4};

template<typename T, typename F, typename S, tiny_t nworkers>
static inline void iterate(std::vector<std::result_of_t<F(T)>>& res,
                           const std::vector<T>& work_queue,
                           const tiny_t id,
                           const size_t load,
                           F fun,
                           sched::roundrobin) {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        res.emplace_back(fun(work_queue[sched]));
    }
}

template<typename T, typename F, typename S, tiny_t nworkers>
static inline void iterate(std::vector<std::result_of_t<F(T)>>& res,
                           const std::vector<T>& work_queue,
                           const tiny_t id,
                           const size_t load,
                           F fun,
                           sched::sequencial) {

    const size_t past_load{range::past_load(load, id, nworkers)};
    const size_t worker_load{range::split_equally(load, id, nworkers)};

    for (size_t i{past_load}; i < past_load+worker_load; ++i) {
        res.emplace_back(fun(work_queue[i]));
    }
}

template<typename T, typename F, typename S, tiny_t nworkers>
std::vector<std::result_of_t<F(T)>
> worker(const std::vector<T>& work_queue, F fun, tiny_t id, S sched) {
    static_assert(nworkers > 0, "need at least one worker");
    using U = std::result_of_t<F(T)>;
    const size_t total_load{work_queue.size()};
    const size_t worker_load{range::split_equally(total_load, id, nworkers)};
    std::vector<U> res;
    if (!worker_load) {
        return res;
    }
    res.reserve(worker_load);
    iterate<T, F, S, nworkers>(res, work_queue, id, total_load, fun, sched);
    return res;
}

template<typename T, typename F, typename S = sched::sequencial,
         tiny_t nworkers = nthreads> std::vector<std::result_of_t<F(T)>
> work_balancer(const std::vector<T>& data,
                F& fun,
                S sched = sched::sequencial()) {
    using U = std::result_of_t<F(T)>;
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    std::array<std::future<std::vector<U>>, nworkers> workers;

    for(tiny_t i{0}; i < nworkers; ++i) {
        workers[i] = std::async(std::launch::async, [&data, fun, i, sched]() {
            return worker<T, U, S, nworkers>(data, fun, i, sched);
        });
    }
    for (auto& w : workers) {
        auto wres = w.get();
        res.insert(std::end(res), std::make_move_iterator(std::begin(wres)),
                   std::make_move_iterator(std::end(wres)));
    }
    return res;
}


template<typename T, typename F, typename S = sched::sequencial,
         tiny_t nworkers>
std::vector<std::result_of_t<F(T)>
> work_balancer(const std::vector<T>& data,
                F& fun,
                Num<nworkers>,
                S sched = sched::sequencial()) {
    return work_balancer<T, F, S, nworkers>(data, fun, sched);
}

template<typename R, typename F, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(data_t<t_of<R>>)>
> worker_onrange_byref(R& data, F fun) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");
    using T = t_of<R>;
    using U = std::result_of_t<F(data_t<T>)>;
    std::vector<U> res;

    while(data.valid()) {
        res.emplace_back(fun(data.get()));
        data.next();
    }
    return res;
}

template<typename R, typename F, tiny_t id, tiny_t nworkers>
constexpr auto async_worker_onrange_byref(R& data, F& fun) {
    return std::async(std::launch::async, [&data, fun]() {
        return worker_onrange_byref<R, F, id, nworkers>(data, fun);
    });
}

template<typename R, typename F, tiny_t... Is>
constexpr auto workers_onrange(std::array<R, sizeof...(Is)>& data,
                               F& fun,
                               std::integer_sequence<tiny_t, Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    using U = std::result_of_t<F(data_t<t_of<R>>)>;
    return std::array<std::future<std::vector<U>>, nworkers>{
        {async_worker_onrange_byref<R, F, Is, nworkers>(data[Is], fun)...}
    };
}

template<typename R, typename T, typename F, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(data_t<T>)>
> worker_onrange(R data, F fun) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");
    using U = std::result_of_t<F(data_t<T>)>;
    std::vector<U> res;

    while(data.valid()) {
        res.emplace_back(fun(data.get()));
        data.next();
    }
    return res;
}

template<typename R, typename T, typename F, tiny_t id, tiny_t nworkers>
constexpr auto async_worker_onrange(R data, F& fun) {
    return std::async(std::launch::async, [data, fun]() {
        return worker_onrange<R, T, F, id, nworkers>(data, fun);
    });
}

template<typename T, typename F, tiny_t... Is>
constexpr auto csworkers_onrange_sub_seq(const std::vector<T>& data,
                                         F& fun,
                                         std::integer_sequence<tiny_t, Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    using U = std::result_of_t<F(cdata_t<T>)>;
    return std::array<std::future<std::vector<U>>, nworkers>{
        {
            async_worker_onrange<range::csrange_sub_seq<T, Is, nworkers>,
                    const T, F, Is, nworkers>(range::csrange_sub_seq<
                                              T, Is, nworkers>(&data[0],
                                              &data[0]+data.size()), fun)...
        }
    };
}

template<typename T, typename F, tiny_t nworkers = nthreads>
std::vector<std::result_of_t<F(cdata_t<T>)>
> swork_balancer_sub_seq(const std::vector<T>& data,
                         F& fun) {
    using U = std::result_of_t<F(cdata_t<T>)>;
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    auto workers = csworkers_onrange_sub_seq(data, fun, indexes<nworkers>());

    for (tiny_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(std::end(res), std::make_move_iterator(std::begin(wres)),
                   std::make_move_iterator(std::end(wres)));
    }
    return res;
}

template<typename T, typename F, typename S, tiny_t id, tiny_t nworkers>
static inline void iterate(std::vector<std::result_of_t<F(T)>>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           F& fun,
                           sched::roundrobin) {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        res.emplace_back(fun(work_queue[sched]));
    }
}

template<typename T, typename F, typename S, tiny_t id, tiny_t nworkers>
static inline void iterate(std::vector<std::result_of_t<F(T)>>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           F& fun,
                           sched::sequencial) {
    const size_t past_load{range::past_load(load, id, nworkers)};
    const size_t worker_load{range::split_equally(load, id, nworkers)};

    for (size_t i{past_load}; i < past_load+worker_load; ++i) {
        res.emplace_back(fun(work_queue[i]));
    }
}

template<typename T, typename F, typename S, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(T)>
> static_worker(const std::vector<T>& work_queue,
                F fun, S sched) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");
    using U = std::result_of_t<F(T)>;
    const size_t total_load{work_queue.size()};
    const size_t worker_load{range::split_equally(total_load, id, nworkers)};
    std::vector<U> res;
    if (!worker_load) {
        return res;
    }
    res.reserve(worker_load);
    iterate<T, F, S, id, nworkers>(res, work_queue, total_load, fun, sched);
    return res;
}

template<typename T, typename F, typename S, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(T)>
> static_worker(const std::vector<T>& work_queue,
                F fun, Num<id>, S sched) {
    return static_worker<T, F, S, id, nworkers>(work_queue, fun, sched);
}

template<typename T, typename F, typename S, tiny_t id, tiny_t nworkers>
constexpr auto async_static_worker(const std::vector<T>& data,
                                   F& fun, S sched) {
    return std::async(std::launch::async, [&data, fun, sched]() {
        return static_worker<T, F, S, id, nworkers>(data, fun, sched);
    });
}

template<typename T, typename F, typename S, tiny_t... Is>
constexpr auto static_workers(const std::vector<T>& data,
                              F& fun, S sched,
                              std::integer_sequence<tiny_t, Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    using U = std::result_of_t<F(T)>;
    return std::array<std::future<std::vector<U>>, nworkers>{
        {async_static_worker<T, F, S, Is, nworkers>(data, fun, sched)...}
    };
}

template<typename T, typename F, typename S = sched::sequencial,
         tiny_t nworkers = nthreads> std::vector<std::result_of_t<F(T)>
> static_work_balancer(const std::vector<T>& data,
                       F& fun,
                       S sched = sched::sequencial()) {
    using U = std::result_of_t<F(T)>;
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    auto workers = static_workers(data, fun, sched, indexes<nworkers>());

    for (tiny_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(std::end(res), std::make_move_iterator(std::begin(wres)),
                   std::make_move_iterator(std::end(wres)));
    }
    return res;
}

template<typename T, typename F, typename S = sched::sequencial,
         tiny_t nworkers>
std::vector<std::result_of_t<F(T)>
> static_work_balancer(const std::vector<T>& data,
                       F& fun,
                       Num<nworkers>,
                       S sched = sched::sequencial()) {
    return static_work_balancer<T, F, S, nworkers>(data, fun, sched);
}

namespace exp {

template<typename T, typename F, typename FF, typename S,tiny_t id, tiny_t nworkers>
static inline void iterate_gen(std::vector<std::result_of_t<F(T, size_t)>>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           F fun, FF filter,
                           sched::roundrobin) {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        if (filter(work_queue[sched], sched))
            res.emplace_back(fun(work_queue[sched], sched));
    }
}

template<typename T, typename F,typename FF, typename S, tiny_t id, tiny_t nworkers>
static inline void iterate_gen(std::vector<std::result_of_t<F(T, size_t)>>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           F fun, FF filter,
                           sched::sequencial) {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        if (filter(work_queue[sched], sched))
            res.emplace_back(fun(work_queue[sched], sched));
    }
}

template<typename T, typename F, typename FF,typename S, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(T, size_t)>
> gen_worker(const std::vector<T>& work_queue,
                F fun, FF filter,S sched) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");
    using U = std::result_of_t<F(T, size_t)>;
    const size_t total_load{work_queue.size()};
    const size_t worker_load{range::split_equally(total_load, id, nworkers)};
    std::vector<U> res;
    if (!worker_load) {
        return res;
    }
    res.reserve(worker_load);
    iterate_gen<T, F, FF, S, id, nworkers>(res, work_queue, total_load, fun, filter, sched);
    return res;
}

template<typename T, typename F,typename FF, typename S, tiny_t id, tiny_t nworkers>
std::vector<std::result_of_t<F(T, size_t)>
> gen_worker(const std::vector<T>& work_queue,
                F fun, FF filter, Num<id>, S sched) {
    return gen_worker<T, F, FF, S, id, nworkers>(work_queue, fun, filter, sched);
}

template<typename T, typename F, typename FF,typename S, tiny_t id, tiny_t nworkers>
constexpr auto async_gen_worker(const std::vector<T>& data,
                                   F& fun, FF& filter, S sched) {
    return std::async(std::launch::async, [&data, fun, filter, sched]() {
        return gen_worker<T, F, FF, S, id, nworkers>(data, fun, filter, sched);
    });
}

template<typename T, typename F, typename FF, typename S, tiny_t... Is>
constexpr auto gen_workers(const std::vector<T>& data,
                              F& fun, FF& filter, S sched,
                              std::integer_sequence<tiny_t, Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    using U = std::result_of_t<F(T, size_t)>;
    return std::array<std::future<std::vector<U>>, nworkers>{
        {async_gen_worker<T, F , FF, S, Is, nworkers>(data, fun, filter, sched)...}
    };
}

template<typename T, typename F,typename FF, typename S = sched::sequencial,
         tiny_t nworkers = nthreads> std::vector<std::result_of_t<F(T, size_t)>
> gen_work_balancer(const std::vector<T>& data,
                       F& fun, FF& filter,
                       S sched = sched::sequencial()) {
    using U = std::result_of_t<F(T, size_t)>;
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    auto workers = gen_workers(data, fun, filter, sched, indexes<nworkers>());

    for (tiny_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(std::end(res), std::make_move_iterator(std::begin(wres)),
                   std::make_move_iterator(std::end(wres)));
    }
    res.shrink_to_fit();
    return res;
}

template<typename T, typename F,typename FF, typename S = sched::sequencial,
         tiny_t nworkers>
std::vector<std::result_of_t<F(T, size_t)>
> gen_work_balancer(const std::vector<T>& data,
                       F& fun, FF& filter,
                       Num<nworkers>,
                       S sched = sched::sequencial()) {
    return gen_work_balancer<T, F, FF, S, nworkers>(data, fun, filter, sched);
}
} // namespace exp

template<typename T>
constexpr const T* first(cdata_t<T> t) {
    return std::get<0>(t);
}

template<typename T>
constexpr const T* second(cdata_t<T> t) {
    return std::get<1>(t);
}

template<typename T>
constexpr T* first(data_t<T> t) {
    return std::get<0>(t);
}

template<typename T>
constexpr T* second(data_t<T> t) {
    return std::get<1>(t);
}

}   // work namespace

#endif // WORKERS_H

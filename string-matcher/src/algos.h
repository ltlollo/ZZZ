#ifndef ALGOS_H
#define ALGOS_H

#include <cstddef>

namespace algos
{

template<typename T, typename U>
inline auto match_containers(const T*, const U*) noexcept -> size_t;

template<typename T, typename U, typename P>
inline auto match_containers(const T*, const U*, P&&) noexcept -> size_t;

template<typename T, typename U, typename P>
inline auto match_containers_sim(const T*, const U*, P&&) noexcept -> size_t;

template<typename T, typename U>
inline auto match_containers_sim(const T*, const U*) noexcept -> size_t;

template<typename T, typename U>
inline size_t match_containers(const T* fst, const U* snd) noexcept
{
    size_t match, max_match{0};

    for (auto it_f = begin(*fst); it_f != end(*fst); ++it_f) {
        match = 0;

        for (auto it_curr = it_f, it_s = begin(*snd);
             it_curr != end(*fst) && it_s != end(*snd); ++it_curr, ++it_s) {
            if (*it_curr == *it_s) { ++match ; }
        }
        if (match > max_match) { max_match = match; }
    }
    for (auto it_s = begin(*snd); it_s != end(*snd); ++it_s) {
        match = 0;

        for (auto it_curr = it_s, it_f = begin(*fst);
             it_curr != end(*snd) && it_f != end(*fst); ++it_curr, ++it_f) {
            if (*it_curr == *it_f) ++match;
        }
        if (match > max_match) { max_match = match; }
    }
    return max_match;
}

template<typename T, typename U, typename P>
inline size_t match_containers(const T* fst, const U* snd, P&& pred) noexcept
{
    size_t match, max_match{0};

    for (auto it_f = begin(*fst); it_f != end(*fst); ++it_f) {
        match = 0;

        for (auto it_curr = it_f, it_s = begin(*snd);
             it_curr != end(*fst) && it_s != end(*snd); ++it_curr, ++it_s) {
            if (pred(*it_curr, *it_s)) { ++match ; }
        }
        if (match > max_match) { max_match = match; }
    }

    for (auto it_s = begin(*snd); it_s != end(*snd); ++it_s) {
        match = 0;

        for (auto it_curr = it_s, it_f = begin(*fst);
             it_curr != end(*snd) && it_f != end(*fst); ++it_curr, ++it_f) {
            if (pred(*it_curr, *it_f)) ++match;
        }
        if (match > max_match) { max_match = match; }
    }
    return max_match;
}

template<typename T, typename U, typename P>
inline size_t match_containers_sim(const T* fst, const U* snd, P&& pred) noexcept
{
    /* This function generally kills more puppies than it saves
     * (with the max_match < len check).
     * But it's still _correct_ and might be faster when data has certain
     * proprieties. Also if testing against |A-B|< diff, this could save cycles.
     */
    size_t match, max_match{0};

    size_t len = end(*fst) - begin(*fst);
    for (auto it_f = begin(*fst);
         it_f != end(*fst) && max_match < len; ++it_f, --len) {
        match = 0;

        for (auto it_curr = it_f, it_s = begin(*snd);
             it_curr != end(*fst) && it_s != end(*snd); ++it_curr, ++it_s) {
            if (pred(*it_curr, *it_s)) { ++match; }
        }

        if (match > max_match) { max_match = match; }
    }
    len = end(*fst) - begin(*fst);
    for (auto it_s = begin(*snd);
         it_s != end(*snd) && max_match < len; ++it_s, --len) {
        match = 0;

        for (auto it_curr = it_s, it_f = begin(*fst);
             it_curr != end(*snd) && it_f != end(*fst); ++it_curr, ++it_f) {
            if (pred(*it_curr, *it_f)) { ++match; }
        }
        if (match > max_match) { max_match = match; }
    }
    return max_match;
}

template<typename T, typename U>
inline size_t match_containers_sim(const T* fst, const U* snd) noexcept
{
    size_t match, max_match{0};

    size_t len = end(*fst) - begin(*fst);
    for (auto it_f = begin(*fst);
         it_f != end(*fst) && max_match < len; ++it_f, --len) {
        match = 0;

        for (auto it_curr = it_f, it_s = begin(*snd);
             it_curr != end(*fst) && it_s != end(*snd); ++it_curr, ++it_s) {
            if (*it_curr == *it_s) { ++match; }
        }

        if (match > max_match) { max_match = match; }
    }

    len = end(*snd) - begin(*snd);
    for (auto it_s = begin(*snd);
         it_s != end(*snd) && max_match < len; ++it_s, --len) {
        match = 0;

        for (auto it_curr = it_s, it_f = begin(*fst);
             it_curr != end(*snd) && it_f != end(*fst); ++it_curr, ++it_f) {
            if (*it_curr == *it_f) { ++match; }
        }
        if (match > max_match) { max_match = match; }
    }
    return max_match;
}

}   // namespace algos

#endif // ALGOS_H

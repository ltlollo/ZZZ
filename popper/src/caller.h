#ifndef CALLER_H
#define CALLER_H

#include <string>
#include <regex>

#include "config.h"
#include "lockio.h"

namespace cll {

enum ExitCode {
    Ok    =  1,
    Pass  =  0,
    Error =  2
};

class Caller {
    const std::string fname;
    const std::vector<char> input;
    const bool single_state, halt_on_err, has_stdin;
    const bool dolog{true};

public:
    template<typename T>
    Caller(T&& name, const bool simple = true, const bool halt = true);
    template<typename T, typename U>
    Caller(T&& name, U&& in, const bool simple = true, const bool halt = true);
    ExitCode call_prog(const std::regex& rx,
                        const cnf::Spec& rule) const noexcept;
    ExitCode run(const cnf::Script& conf, lio::LockStream& log) const noexcept;
};

template<typename T>
Caller::Caller(T&& name, const bool simple, const bool halt)
    : fname{std::forward<T>(name)}, single_state{simple}, halt_on_err{halt},
      has_stdin{false}
{}

template<typename T, typename U>
Caller::Caller(T&& name, U&& in, const bool simple, const bool halt)
    : fname{std::forward<T>(name)}, input{std::forward<U>(in)},
      single_state{simple}, halt_on_err{halt}, has_stdin{!input.empty()}
{}

}

#endif // CALLER_H

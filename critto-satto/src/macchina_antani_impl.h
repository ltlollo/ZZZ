#ifndef MACCHINA_ANTANI_IMPL_H
#define MACCHINA_ANTANI_IMPL_H

#include "macchina_antani.h"
#include "function.h"


template <typename First>
void GlobalScope<First>::run()
{
    for (auto it: __flist) ProtectExec(*it, __data, __safe);
}

template <typename First>
void GlobalScope<First>::profile()
{
    for (auto it: __flist)
    {
        chrono::high_resolution_clock clk;
        auto start = clk.now();
        ProtectExec(*it, __data, __safe);
        auto end = clk.now();
        std::cerr << it->getPassed() << " took: "
                  << (end-start).count() << std::endl;
    }
}

template <typename First>
void GlobalScope<First>::run_with_except()
{
    for (auto it: __flist)
    {
        if (__safe)
        {
            try {
                ProtectExec(*it, __data, __safe);
            }
            catch(int& except)
            {
                std::cerr << "Exception occurred, code: "
                          << except << std::endl;
            }
        }
    }
}

template <typename First>
void GlobalScope<First>::getInput(int argc,char *argv[])
{
    __data.argc = argc;
    __data.argv = argv;
}

template <typename First>
void GlobalScope<First>::addState(Function* func)
{
    __safe = false;

    __flist.push_back(func);

    __safe = true;
}

template <typename First>
GlobalScope<First>::~GlobalScope() noexcept
{
    if (!__safe) __unclean_exit();
}

template <typename First>
void GlobalScope<First>::__unclean_exit() noexcept
{
    cout << __data.content << std::endl;
}

#endif // MACCHINA_ANTANI_H

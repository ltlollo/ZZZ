#ifndef MACCHINA_ANTANI_H
#define MACCHINA_ANTANI_H

#include <iostream>
#include <chrono>

#include "function.h"

namespace stmac
{
using namespace func;
using namespace std;

template <typename Func, typename Arg>
class SafetyGuard
{
public:
    SafetyGuard(Func& fclass, Arg& data, bool& state)
        : __rfunc(fclass), __safe(state), __data(data)
    { __safe = false; }

    void dismiss()
    {
        __current_state = __rfunc.getMessage();

        __rfunc(__data);

        __current_state = __rfunc.getPassed();

        __dismiss();
    }

    ~SafetyGuard() noexcept
    {
        if (__safe)
            __rfunc.cleanexit();
        else { __print_error(); __rfunc.rollback(); }
    }


protected:
    Func& __rfunc;
    bool& __safe;
    Arg& __data;
    string __current_state;

    void __print_error() const noexcept
    {
        cerr << "Error: " << __current_state << endl;
        cerr << "Problem: " << __rfunc.getMessage() << endl;
        cerr << "while: " << __rfunc.getPassed() << endl;
        cerr << "Exception code id: " << __rfunc.getErrorId() << endl;
    }

    void __dismiss() noexcept { __safe = true; }
};

template <typename Func, typename Arg>
void ProtectExec(Func& fclass, Arg& data, bool &safe)
{
    SafetyGuard<Func, Arg> p(fclass, data, safe);
    p.dismiss();
}

template <typename First>
class GlobalScope
{
protected:
    First __data;
    bool __safe;
    vector<Function*> __flist;

    void __unclean_exit() noexcept;

public:
    GlobalScope() : __safe(true) {}

    void addState(Function* func);

    void run();

    void run_with_except();

    void profile();

    void getInput(int argc,char *argv[]);

    ~GlobalScope() noexcept;
};

#include "macchina_antani_impl.h"
}

#endif // MACCHINA_ANTANI_H

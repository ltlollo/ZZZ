#ifndef FUNCTION_H
#define FUNCTION_H

#include "data.h"

namespace func
{
using namespace d_types;

class Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    Function()
        : error_id(0),
          message("Error: does not compute"),
          passed("like a boss...") {}

    Function(const int err,
             std::string msg,
             const std::string pass)
        : error_id(err), message(msg), passed(pass) {}

    virtual void operator()(Data& data) = 0;

    virtual void rollback() noexcept {}

    virtual void cleanexit() noexcept {}

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }


    virtual int getErrorId() const noexcept
    { return this->error_id; }
};

}
#endif // FUNCTION_H

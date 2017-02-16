#ifndef CRITTOSATTO_H
#define CRITTOSATTO_H

#include "function.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <iosfwd>
#include <istream>

using namespace func;


class check_argv final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    check_argv()
        : error_id(10),
          message("Command line plespem"),
          passed("checking the command line") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class grab_stdin final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    grab_stdin()
        : error_id(11),
          message("Does not compute."),
          passed("reading stdin") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class read_file final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    read_file()
        : error_id(20),
          message("Error reading file."),
          passed("loading files") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class random_pop final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    random_pop()
        : error_id(25),
          message("Does not compute."),
          passed("populating the boundaries") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class read_stdin final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    read_stdin()
        : error_id(30),
          message("Error reading stdin."),
          passed("reading stdin") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class read_message final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

    char *mmask{nullptr};

    void __collect_garbage() noexcept
    {
        if (mmask != nullptr)
        { delete[] mmask; mmask = nullptr; }
    }

public:
    read_message()
        : error_id(31),
          message("Error reading file."),
          passed("reading input file") {}

    virtual void rollback() noexcept
    { __collect_garbage(); }

    virtual void cleanexit() noexcept
    { __collect_garbage(); }

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class enc_stub final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    enc_stub()
        : error_id(40),
          message("Incorrect file sizes."),
          passed("encripting message") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class dec_stub final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    dec_stub()
        : error_id(41),
          message("Incorrect format."),
          passed("decripting message") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};


class write_output final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    write_output()
        : error_id(50),
          message("Cannot write output file."),
          passed("writing output") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};

class output_message final : public Function
{
private:
    const int error_id;
    std::string message;
    const std::string passed;

public:
    output_message()
        : error_id(51),
          message("Does not compute."),
          passed("writing output") {}

    virtual void operator()(Data& data);

    virtual std::string getMessage() const noexcept
    { return this->message; }

    virtual const std::string getPassed() const noexcept
    { return this->passed; }

    virtual int getErrorId() const noexcept
    { return this->error_id; }
};

#endif // CRITTOSATTO_H

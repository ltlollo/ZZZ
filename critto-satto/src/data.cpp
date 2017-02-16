#include "data.h"

using namespace d_types;

Data::Data(): work_on_stdin(true), work_on_stdout(true)
{}

Data::Data(int argc,char *argv[])
    : Data()
{
    this->argc = argc;
    this->argv = argv;
}

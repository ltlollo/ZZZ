#ifndef SERVER_H
#define SERVER_H

#include <sys/prctl.h>
#include <linux/prctl.h>
#include <linux/capability.h>
#include <linux/securebits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>
#include <thread>

#include "caller.h"
#include "extra/socket.h"
#include "pass.h"
#include "lockio.h"
#include "conf.h"
#include "global.h"

namespace srv {

class Server {
    const cnf::Script conf;
    const bool simple {true}, halt {true};
    file::Socket sock;
    file::Bind socfile;
    lio::LockStream& log;

    void setsigs() const;
    void init();

public:
    explicit Server(lio::LockStream& log, const bool simple = true,
                    const bool halt = true);
    [[noreturn]] void run() const &&;
};

bool indfork(bool unsafe);

void job(file::Socket&& s, const cnf::Script& conf, lio::LockStream& log,
         const bool halt, const bool simple);

}

#endif // SERVER_H


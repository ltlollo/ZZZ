#include "server.h"

using namespace fun;
using namespace cll;
using namespace cnf;
using namespace std;
using namespace msg;
using namespace file;
using namespace lio;

namespace srv {

Server::Server(LockStream& log, const bool simple, const bool halt)
    : simple{simple}, halt{halt}, sock{}, socfile(global::sockpath, sock),
      log{log} {
    setsigs();
    init();
}

void Server::setsigs() const {
    err::donotfail_errno("sigaction", ansi::signal, ansi::sigint,
                         [](int, siginfo_t*, void*){
        ansi::unlink(global::sockpath);
        ansi::exit(0);
    });
}

void Server::init() {
    // prepare data or stfd
    /* TODO: decide if i want this in cmdline
    ansi::close(ansi::stdout);
    ansi::close(ansi::stdin);
    */
    auto& conf_wref = const_cast<Script&>(conf);
    ifstream io{global::confpath};
    sock.provide_cred();
    for (string line; getline(io, line); ) {
        line_parser(move(line), conf_wref);
    }
}

void job(Socket&& s, const Script& conf, LockStream& log,
         const bool halt, const bool simple) {
    try {
        Socket sock{move(s)};
        sock.time(100);
        CredRecv csock{sock};

        auto pass = csock.recv_cred<string>();
        err::doreturn("auth", pass.data != passw());
        err::doreturn("root", pass.cred.uid == 0 || pass.cred.gid == 0);
        auto cmd = csock.recv_cred<string>();
        auto msgsize = csock.recv_cred<size_t>();
        err::doreturn("message too big", msgsize.data > maxsize);
        err::doreturn("cred mismatch", !(cmd.eq_cred(msgsize) &&
                                         cmd.eq_cred(pass)));
        auto response = [&]() {
            if (msgsize.data) {
                auto stdin = csock.recv_cred_notnil<vector<char>>(msgsize.data);
                err::doreturn("cred mismatch", !cmd.eq_cred(stdin));
                return Caller(move(cmd.data), move(stdin.data), simple, halt);
            } else {
                return Caller(move(cmd.data), simple, halt);
            }
        }().run(conf, log);
        sock.send(response);
    } catch (runtime_error& e) {
        log.timed("failed: "s + e.what());
    }
}

void Server::run() const && {
    // open socket accept(block), spawn thread, move socket to thread
    sock.listen();
    loop([&]() {
        thread(job, sock.clone(), conf, ref(log), halt, simple).detach();
    });
}

bool indfork(bool unsafe) {
    if (debug) {
        ansi::unlink(global::sockpath);
        return true;
    }
    pid_t fd{ansi::fork()};
    err::donotfail_errno("fork once", fd);
    if (fd) {
        return false;
    }
    fd = ansi::fork();
    err::donotfail_errno("fork twice", fd);
    if (fd) {
        return false;
    }
    umask(0017);
    if (unsafe) {
        return true;
    }
    for (unsigned cap{0}; cap <= CAP_LAST_CAP; ++cap) {
        err::donotfail_errno("capdrop", prctl, PR_CAPBSET_DROP, cap, 0, 0, 0);
    }
    err::donotfail_errno("secure", prctl, PR_SET_SECUREBITS,
                    SECURE_ALL_BITS | SECURE_ALL_LOCKS);
    return true;
}

}

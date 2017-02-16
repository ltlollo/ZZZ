#include "caller.h"

using namespace std;
using namespace cnf;
using namespace lio;

namespace cll {

ExitCode Caller::call_prog(const regex& rx, const Spec& rule) const noexcept {
    smatch what;
    if (regex_match(fname, what, rx) && what.size() == rule.wnums.size() + 1) {
        string cmd;
        for (size_t i{0}; i < rule.wnums.size(); ++i) {
            cmd += rule.wnums[i].str;
            cmd += what[rule.wnums[i].pos+1];
        }
        cmd += rule.rest;

        FILE* fd{popen(cmd.c_str(), "w")};
        if (!fd) {
            return ExitCode::Error;
        }
        if (has_stdin && fwrite(&input[0], sizeof(char), input.size(), fd) !=
            input.size()*sizeof(char)) {
            pclose(fd);
            return ExitCode::Error;
        }
        if (pclose(fd)) {
            return ExitCode::Error;
        }
        return ExitCode::Ok;
    }
    return ExitCode::Pass;
}

ExitCode Caller::run(const cnf::Script& conf, LockStream& log) const noexcept {
    ExitCode err {ExitCode::Ok};
    for (const auto& it: conf) {
        err = call_prog(it.rx, it.rule);
        if (err == ExitCode::Error) {
            if (dolog) log << log.timestamp() + " FAIL: \"" +
                              get_line(it.rule) + "\" with: \"" + fname +
                              '\"';
            if (halt_on_err) {
                break;
            }
        }
        if (single_state && err == ExitCode::Ok) {
            break;
        }
    }
    if (err == ExitCode::Pass || err == ExitCode::Ok) {
        return ExitCode::Ok;
    }
    return ExitCode::Error;
}

}

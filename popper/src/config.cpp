#include "config.h"

using namespace std;

namespace cnf {

static regex rs_tester(string&& r) {
    string rs{move(r)};

    string filter_dbs{remove_chars(rs, R"(\\)")};
    string filter_bparen{remove_chars(move(filter_dbs), R"(\()")};
    string ignore_ncaptgroup{remove_chars(move(filter_bparen), "(?:")};
    const regex tester_rx(R"(\([^\)]*\()");

    if (!regex_search(ignore_ncaptgroup, tester_rx)) {
        const regex is_valid(rs, regex::optimize);
        return is_valid;
    }
    throw runtime_error("nested captures: " + rs);
}

static void make_line(const string& cmd, Spec& conf) {
    const regex spec("(.*?)\\\\(\\d+)(.*)");
    smatch what;

    if (regex_match(cmd, what, spec)) {
        conf.wnums.emplace_back(Wordnum{what[1], stoul(what[2])});
        if (what.size() > 3) {
            make_line(what[3], conf);
        }
        return;
    }
    conf.rest = cmd;
}

string get_line(const Spec& l) {
    string s;
    for (const auto& it : l.wnums) {
        s += it.str;
        s += '\\' + to_string(it.pos);
    }
    s += l.rest;
    return s;
}

static void look_for_holes(Spec& l, const size_t ncaptures) {
    if (l.wnums.empty()) {
        for (size_t i{0}; i < ncaptures; ++i) {
            if (!i) {
                l.wnums.emplace_back(Wordnum {move(l.rest), 0});
                l.rest = "";
            } else {
                l.wnums.emplace_back(Wordnum {"", i});
            }
        }
        return;
    }
    if (l.wnums.size() != ncaptures) {
        throw runtime_error("wrong number of captures: " + get_line(l));
    }

    vector<size_t> seq, is_valid;
    seq.reserve(l.wnums.size());
    is_valid.reserve(l.wnums.size());

    for (size_t i {0}; i < l.wnums.size(); ++i) {
        seq.push_back(l.wnums[i].pos);
        is_valid.push_back(i);
    }
    sort(seq.begin(), seq.end());
    if (seq[0] == 0 && seq == is_valid) {
        return;
    }
    throw(runtime_error("holes in line: " + get_line(l)));
}

static Spec cmd_tester(string&& cmd, const size_t ncaptures) {
    Spec is_valid{};
    make_line(move(cmd), is_valid);
    look_for_holes(is_valid, ncaptures);
    return is_valid;
}

void line_parser(const string& conf_line, Script& conf) {
    smatch what;
    if (regex_match(conf_line, what, regex("(.*\\S)\\s+" "\\(\\(\\)\\)"
                                           "\\s+(\\S.*)"))) {
        regex valid_rx {rs_tester(move(what[1]))};
        Spec valid_cmd = cmd_tester(move(what[2]), valid_rx.mark_count());
        conf.emplace_back(Confline {move(valid_rx), move(valid_cmd)});
        return;
    }
    throw runtime_error("cannot parse: " + conf_line);
}
}

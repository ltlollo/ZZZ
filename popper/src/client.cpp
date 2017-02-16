#include "client.h"

using namespace std;
using namespace fun;
using namespace msg;
using namespace file;
using namespace cll;

namespace cli {

void Client::init() {
    Socket sock;
    sock.connect(global::sockpath);
    sock << passw() << command << stdin.size();
    sock.send(stdin);
    ExitCode response{Error};
    sock >> response;
    err::doreturn("confirm", response != Ok);
}

DirPath::DirPath() : m_dir {ansi::get_current_dir_name()} {
    err::doreturn_errno("getdir", !m_dir);
}

DirPath::~DirPath() {
    free(m_dir);
}

string DirPath::str() const {
    return string(m_dir);
}

void make_absolute(string& path) {
    err::doreturn("empty", path.empty());
    if (path[0] == '/') {
        return;
    }
    if (regex_search(path, regex("\\w+://", regex::optimize))) {
        return;
    }
    string current_dir {DirPath().str()};
    path = current_dir + '/' + path;
}

void curate_url(string& url) {
    insert_before<'(',  '\\'>(url);
    insert_before<')',  '\\'>(url);
    insert_before<' ',  '\\'>(url);
    insert_before<'\'', '\\'>(url);
    insert_before<'"',  '\\'>(url);
    insert_before<'&',  '\\'>(url);
}

}

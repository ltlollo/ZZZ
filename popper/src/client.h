#ifndef CLIENT_H
#define CLIENT_H

#include "pass.h"
#include "caller.h"
#include "global.h"

#include <extra/socket.h>
#include <iostream>

namespace cli {

class Client {
    const std::string command;
    const std::vector<char> stdin;

    void init();

public:
    template<typename U, typename V>
    Client(U&& cmd, V&& in);
};

template<typename U, typename V>
Client::Client(U&& cmd, V&& in) : command{std::forward<U>(cmd)},
    stdin{std::forward<V>(in)} {
    init();
}

template<char before, char c>
void insert_before(std::string& s) {
    static_assert(before != c, "cannot be the same");

    for (auto it = s.begin(); it != s.end(); ++it) {
        if (*it == before) {
            it = s.insert(it, c) + 1;
        }
    }
}

void make_absolute(std::string& path);
void curate_url(std::string& url);

class DirPath {
    char* m_dir;

public:
    DirPath();
    ~DirPath();
    std::string str() const;
};

}

#endif // CLIENT_H

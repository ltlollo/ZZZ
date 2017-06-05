#ifndef DIRCRAP_H
#define DIRCRAP_H

#include <vector>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

namespace fs {

class dir {
    bool passedinit{false};
    const std::string dirname;
    DIR* dp;

public:
    template<typename T> dir(T&& str);
    dir(char) = delete;
    dir();
    ~dir();
    std::string get() const;
    std::vector<std::string> list() const noexcept;
};

template<typename T>
dir::dir(T&& str) : dirname{std::forward<T>(str)}, dp{opendir(str)} {
    if (dp) { passedinit = true; }
}

}

#endif // DIRCRAP_H

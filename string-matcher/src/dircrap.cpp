#include "dircrap.h"

using std::vector;
using std::string;

namespace fs {

vector<string> dir::list() const noexcept {
    vector<string> files;
    if (!passedinit) return files;

    struct dirent* dirp;
    while ((dirp = readdir(dp)) != nullptr) {
        string content{dirp->d_name};
        if (!content.empty() && content[0] != '.') {
            files.push_back(move(content));
        }
    }
    rewinddir(dp);
    return files;
}

dir::dir() : dir(".") {
}

string dir::get() const { return dirname; }

dir::~dir() { closedir(dp); }

}

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <regex>

namespace cnf {

struct Wordnum {
    const std::string str;
    const std::size_t pos;
};

struct Spec {
    std::vector<Wordnum> wnums{};
    std::string rest{};
};

struct Confline {
    const std::regex rx;
    const Spec rule;
};

using Script = std::vector<Confline>;

std::string get_line(const Spec& l);

void line_parser(const std::string& conf_line, Script& conf);

template<typename T>
static std::string remove_chars(T&& base, std::string&& sub) {
    std::string new_str {std::forward<T>(base)};
    auto found = new_str.find(move(sub));
    while(found != std::string::npos) {
        for (size_t i {0}; i < sub.size(); ++i, ++found) {
            new_str[found] = ' ';
        }
        found = new_str.find(sub);
    }
    return new_str;
}

}

#endif // CONFIG_H


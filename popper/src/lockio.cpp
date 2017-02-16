#include "lockio.h"

using namespace std;
using chrono::system_clock;

namespace lio {

LockStream::LockStream(FILE* fd) : m_fd{fd} {
    if (!fd) {
        throw NonLoggable("fopen");
    }
    if(setvbuf(fd, nullptr, _IOLBF, 0)) {
        fclose(fd);
        throw NonLoggable("setvbuf");
    }
}

LockStream::LockStream(const char* fname) : LockStream(fopen(fname, "ab+")) {
}

template<unsigned N>
LockStream::LockStream(const char (&fname)[N])
    : LockStream(fopen(fname, "ab+")) {
}

LockStream::LockStream(const std::string& fname) : LockStream(fname.c_str()) {
}

LockStream::~LockStream() noexcept {
    if (m_fd) {
        fclose(m_fd);
    }
}

void LockStream::close() {
    lock_guard<mutex> lock(this->m_mutex);
    int res{0};
    if (m_fd) {
        res = fclose(m_fd);
    }
    m_fd = nullptr;
    if(res) {
        throw NonLoggable("fclose");
    }
}

void LockStream::add(const char* str) {
    lock_guard<mutex> lock(this->m_mutex);
    fprintf(this->m_fd, "%s\n", str);
}

void LockStream::operator<<(const char* c_str) {
    this->add(c_str);
}

void LockStream::operator<<(const string &str) {
    this->add(str.c_str());
}

void LockStream::timed(const string& str) {
    lock_guard<mutex> lock(this->m_mutex);
    fprintf(this->m_fd, "%s\n", (timestamp() + ' ' + str).c_str());
}

void LockStream::timed(const string& str, WithErr) {
    auto err = errno;
    lock_guard<mutex> lock(this->m_mutex);
    fprintf(this->m_fd, "%s\n", (timestamp() + ' ' + str + ", errno: " +
                                 std::to_string(err)).c_str());
}

string LockStream::timestamp() const {
    auto tp = system_clock::to_time_t(system_clock::now());
    string time = ctime(&tp);
    time.resize(time.size()-1);
    return time;
}

}

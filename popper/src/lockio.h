#ifndef LOCKIO_H
#define LOCKIO_H

#include <mutex>
#include <string>
#include <chrono>
#include <errno.h>

namespace lio {

struct WithErr {};

class NonLoggable {
    const char* msg;

public:
    template<unsigned N>
    explicit NonLoggable(const char (&msg)[N]) : msg{msg} {}
    const char* what() const noexcept {
        return msg;
    }
};

class LockStream {
    mutable FILE* m_fd{nullptr};
    mutable std::mutex m_mutex;
    void add(const char* str);

public:
    explicit LockStream(FILE* m_fd);
    explicit LockStream(const char* fname);
    explicit LockStream(const std::string& fname);
    template<unsigned N> LockStream(const char (&fname)[N]);
    ~LockStream() noexcept;
    LockStream(LockStream&& rhs) = delete;
    LockStream& operator=(LockStream&& rhs) = delete;
    void close();
    void operator<<(const std::string& str);
    void operator<<(const char* c_str);
    void timed(const std::string& str);
    void timed(const std::string& str, WithErr);
    std::string timestamp() const;
};

}

#endif // LOCKIO_H

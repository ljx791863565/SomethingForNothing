#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include "noncopyable.h"
#include "types.h"

namespace muduo {
namespace detail {
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable {
public:
    FixedBuffer()
        : cur_(data_)
    {}

    ~FixedBuffer()
    {}

    const char *data() const
    {
        return data_;
    }

    int length() const
    {
        return static_cast<int>(cur_ - data_);
    }

    char *current()
    {
        return cur_;
    }

    int avail() const
    {
        return static_cast<int>(end() - cur_);
    }

    // 这里不考虑cur_超出end()？
    void add(size_t len)
    {
        cur_ += len;
    }

    void bzero()
    {
        memZero(data_, sizeof(data_));
    }

    string toString() const
    {
        return string(data_, length());
    }

    void append(const char *buf, size_t len)
    {
        // like safe static_cast<>
        if (implicit_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* debugString();
private:
    const char *end() const
    {
        return data_ + sizeof(data_);
    }
    char data_[SIZE];
    char *cur_;
};
} // namespace detail

class LogStream : noncopyable {
public:
    typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
private:
    Buffer buffer_;
    static const int kMaxNumericSize = 32;


};
} // namespace muduo

#endif // LOG_STREAM_H

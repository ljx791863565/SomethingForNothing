#include "x_string.h"

std::string XString::valueOf(int32_t val)
{
    return _valueOf<int32_t>(val);
}

std::string XString::valueOf(int64_t val)
{
    return _valueOf<int64_t>(val);
}

std::string XString::valueOf(float val)
{
    return _valueOf<float>(val);
}

std::string XString::valueOf(double val)
{
    return _valueOf<double>(val);
}

XString::XString()
    : XString("")
{}

XString::XString(const char *s)
    : str_(s)
{}

XString::XString(const std::string &o)
    : str_(o)
{}

XString::XString(const XString &s)
    : str_(s.str_)
{}

XString::~XString()
{}

std::string& XString::str()
{
    return str_;
}

const char* XString::c_str()
{
    return str_.c_str();
}

XString& XString::append(const std::string &str)
{
    str_.append(str);
    return *this;
}

XString& XString::append(const char *str)
{
    str_.append(str);
    return *this;
}

XString& XString::append(int32_t val)
{
    return append(valueOf(val));
}

XString& XString::append(int64_t val)
{
    return append(valueOf(val));
}

XString& XString::append(float val)
{
    return append(valueOf(val));
}

XString& XString::append(double val)
{
    return append(valueOf(val));
}

size_t XString::length ()
{
    return str_.length();
}

void XString::clear ()
{
    str_.clear();
}

XString& XString::replaceAll(const char *oldStr, const char *newStr)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += strlen(newStr)) {
        pos = str_.find(oldStr, pos);
        if (pos != std::string::npos) {
            str_.replace(pos, strlen(oldStr), newStr);
        } else {
            break;
        }
    }
    return *this;
}
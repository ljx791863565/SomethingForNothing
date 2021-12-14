#ifndef X_STRING_H
#define X_STRING_H

#include <string>
#include <sstream>
// using namespace std;
class XString {
public:
    static std::string valueOf(int32_t val);
    static std::string valueOf(int64_t val);
    static std::string valueOf(float val);
    static std::string valueOf(double val);

private:
    template <typename T>
    static std::string _valueOf(T value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    } 

public:
    XString();
    XString(const char *s);
    XString(const std::string &o);
    XString(const XString &s);
    virtual ~XString();

    std::string& str();
    const char* c_str();
    XString& append(const std::string &str);
    XString& append(const char *str);
    XString& append(int32_t val);
    XString& append(int64_t val);
    XString& append(float val);
    XString& append(double val);

    size_t length();
    void clear();
    XString& replaceAll(const char *oldStr, const char *newStr);
private:
    std::string str_;
};

#endif // X_STRING_H
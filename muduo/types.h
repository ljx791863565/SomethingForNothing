#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <string.h>
#include <stdint.h>

#ifndef NDEBUG
#include <assert.h>
#endif // NDEBUG

namespace muduo {

using std::string;

inline void memZero(void *ptr, size_t len)
{
	memset(ptr, 0, len);
}


/*
1. 用implicit_cast可以实现 一种 static_cast 或者 const_cast在向上转型时的安全版本。
（例如 ： 将Foo指针转换为 SuperclassOfFoo 或者 将 Foo 指针转换为const Foo）
注： 向上安全转型指的是派生类向基类的转型。

2. 当你使用implicit_cast的时候，编译器会检查相应的转换是否安全。
  这样的隐式转换implicit_cast在 C++中需要完全类型匹配而非将一个参数类型转换为目标类型 时显得十分有用

3. From的类型可以被推断， 所以implicit_cast推荐的使用方法和static_cast一样：
implicit_cast<ToType> (expr)

在up_cast时应该使用implicit_cast替换static_cast,因为前者比后者要安全，
这就是一个向上安全转型的用法 ，因为如果用static_cast的话可能因为纵容其它的转换而出错。
*/
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

/*
1.因为但凡程序设计正确，dynamic_cast就可以用static_cast来替换，而后者比前者更有效率。
2.dynamic_cast可能失败(在运行时crash)，运行时RTTI不是好的设计，不应该在运行时RTTI或者需要RTTI时一般都有更好的选择。
*/
template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}

} /* namespace muduo */

#endif /* end of :TYPES_H */

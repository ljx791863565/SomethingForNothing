#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H 
namespace muduo {

class noncopyable {
public:
	noncopyable(const noncopyable &) = delete;
	noncopyable(const noncopyable &&) = delete;
	noncopyable& operator=(const noncopyable &) = delete;
	noncopyable& operator=(const noncopyable &&) = delete;
protected:
	noncopyable() = default;
	virtual ~noncopyable() = default;
};
	
} /* namespace muduo */
#endif /* NONCOPYABLE_H */

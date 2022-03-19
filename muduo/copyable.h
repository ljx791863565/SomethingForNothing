#ifndef COPYABLE_H
#define COPYABLE_H 
namespace muduo {

class copyable {
protected:
	copyable() = default;
	virtual ~copyable() = default;
};

} /* namespace muduo */
#endif // COPYABLE_H 

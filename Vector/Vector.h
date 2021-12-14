#ifndef _VECTOR_H_
#define _VECTOR_H_ 

using rank = int;
constexpr int defaultCapacity = 3;

template <typename T>
class Vector {
public:
	Vector(int capa = defaultCapacity, int s = 0, T e = 0)
		: capacity_(capa), size_(s) 
	{
		elem_ = new T[capacity_];
		for (size_ = 0; size_ < s; size_++) {
			elem_[size_] = e;
		}
	}

	Vector(const T *t, rank hi)
	{
		copyFrom(t, 0, hi);
	}

	Vector(const T *t, rank lo, rank hi)
	{
		copyFrom(t, lo, hi);
	}
	
	Vector(const Vector<T> &v)
	{
		copyFrom(v.elem_, 0, v.size());
	}

	Vector(const Vector<T> &v, rank lo, rank hi)
	{
		copyFrom(v.elem_, lo, hi);
	}

	~Vector()
	{
		delete [] elem_;
	}

	rank size() const 
	{
		return size;
	}

	bool empty() const 
	{
		return size_ == 0;
	}

	T& operator[](rank r) const;

	Vector<T>& operator=(const Vector<T> &v);

	rank insert(rank r, const T &t);

	rank insert(const T &t) 
	{
		return insert(size_, t);
	}

	T remove(rank r);

	int remove(rank lo, rank hi);
protected:
	void copyFrom(const T *t, rank lo, rank hi);
	void expand();
	void shrink();
private:
	int capacity_;
	rank size_;
	T *elem_;
};
#endif // _VECTOR_H_ 

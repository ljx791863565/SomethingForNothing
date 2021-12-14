#include "Vector.h"

template <typename T>
void Vector<T>::copyFrom(const T *t, rank lo, rank hi)
{
    elem_ = new T[capacity_ = 2*(hi - lo)];
    size_ = 0;
    while (lo < hi) {
        elem_[size_++] = t[lo++];
    }    
}
template <typename T>
void Vector<T>::expand()
{
    if (size_ < capacity_) {
        return;
    }
    if (capacity_ < defaultCapacity) {
        capacity_ = defaultCapacity;
    }
    T *oldElem = elem_;
    elem_ = new T[capacity_ *= 2];
    for (int i = 0; i < size_; ++i) {
        elem_[i] = oldElem[i];
    }
    delete [] oldElem;
}

template <typename T>
void Vector<T>::shrink()
{
    if (capacity_ < (defaultCapacity * 2)) {
        return;
    }
    if (capacity_ < (size_ * 4)) {
        return;
    }
    T *oldElem = elem_;
    elem_ = new T[capacity_ /= 2];
    for (int i = 0; i < size_; ++i) {
        elem_[i] = oldElem[i];
    }
    delete [] oldElem;
}

template <typename T>
T& Vector<T>::operator[](rank r) const
{
    return elem_[r];
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T> &v)
{
    if (elem_) {
        delete [] elem_;
    }
    copyFrom(v.elem_, 0, v.size());
    return *this;
}

template <typename T>
rank Vector<T>::insert(rank r, const T &t)
{
    expand();
    for (int i = size_; i > r; --i) {
        elem_[i] = elem_[i-1];
    }
    elem_[r] = t;
    size_ ++;
    return r;
}

template <typename T>
T Vector<T>::remove(rank r)
{
    T t = elem_[r];
    remove(r, r+1);
    return t;
}

template <typename T>
int Vector<T>::remove(rank lo, rank hi)
{
    if (lo == hi) {
        return 0;
    }
    while (hi < size_) {
        elem_[lo++] = elem_[hi++];
    }
    size_ = lo;
    shrink();
    return hi - lo;
}


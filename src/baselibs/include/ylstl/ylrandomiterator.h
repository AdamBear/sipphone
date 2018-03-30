/*

YLRandomIterator.h
用于随机访问容器的迭代器
lusy
2013年7月18日14:05:55
参考http://en.cppreference.com/w/cpp/concept/RandomAccessIterator

*/
#ifndef _YLSTL_RANDOM_ITERATOR_
#define _YLSTL_RANDOM_ITERATOR_

namespace std
{
struct random_access_iterator_tag;
}

//对于下标[]运算符可用容器的iterator
template <typename _Tx, typename _Container>
class random_iterator
{
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef _Tx value_type;
    typedef int difference_type;
    typedef int distance_type;  // retained
    typedef value_type * pointer;
    typedef value_type & reference;
public:
    random_iterator()
        : data_(NULL), pos_(0)
    {
    }
    random_iterator(_Container & _con, distance_type pos)
        : data_(&_con), pos_(pos)
    {
    }
    random_iterator(const random_iterator & other)
        : data_(other.data_), pos_(other.pos_)
    {
    }
    random_iterator & operator= (const random_iterator & other)
    {
        data_ = other.data_;
        pos_ = other.pos_;
        return *this;
    }

    reference operator*() const
    {
        return data()[pos_];
    }
    pointer operator->() const
    {
        return & ** this;
    }

    random_iterator & operator+= (distance_type rhs)
    {
        pos_ += rhs;
        return *this;
    }
    random_iterator & operator++()
    {
        pos_ += 1;
        return *this;
    }
    random_iterator operator++(int)
    {
        random_iterator temp(*this);
        ++(*this);
        return temp;
    }
    random_iterator operator+ (distance_type rhs) const
    {
        random_iterator temp(*this);
        return temp += rhs;
    }
    random_iterator operator-= (distance_type rhs)
    {
        pos_ -= rhs;
        return *this;
    }
    random_iterator & operator--()
    {
        pos_ -= 1;
        return *this;
    }
    random_iterator operator--(int)
    {
        random_iterator temp(*this);
        --(*this);
        return temp;
    }
    random_iterator operator- (distance_type rhs) const
    {
        random_iterator temp(*this);
        return temp -= rhs;
    }
    distance_type operator- (const random_iterator & rhs) const
    {
        return pos_ - rhs.pos_;
    }

    reference operator[](const distance_type index) const
    {
        return data()[pos_];
    }

    bool operator== (const random_iterator & rhs) const
    {
        if (is_iterator_invaild() && rhs.is_iterator_invaild())
        {
            return true;
        }

        if (data_ == rhs.data_
                && pos_ == rhs.pos_)
        {
            return true;
        }

        return false;
    }
    bool operator!= (const random_iterator & rhs) const
    {
        return !(*this == rhs);
    }
    bool operator< (const random_iterator & rhs) const
    {
        return *this - rhs < 0;
    }
    bool operator> (const random_iterator & rhs) const
    {
        return rhs < *this;
    }
    bool operator<= (const random_iterator & rhs) const
    {
        return !(*this > rhs);
    }
    bool operator>= (const random_iterator & rhs) const
    {
        return !(*this < rhs);
    }
private:
    _Container & data() const
    {
        return *data_;
    }
    bool is_iterator_invaild() const
    {
        return data_ == NULL || pos_ < 0 || pos_ > data().size();
    }
private:
    _Container * data_;
    distance_type pos_;
};

template<typename _Tx, typename _Container>
random_iterator<_Tx, _Container>
operator+ (typename random_iterator<_Tx, _Container>::distance_type lhs,
           random_iterator<_Tx, _Container> & rhs)
{
    return rhs + lhs;
}


//对于下标[]运算符可用容器的const_iterator
template <typename _Tx, typename _Container>
class const_random_iterator
{
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef _Tx value_type;
    typedef int difference_type;
    typedef int distance_type;  // retained
    typedef value_type * pointer;
    typedef const value_type * const_pointer;
    typedef value_type & reference;
    typedef const value_type & const_reference;
public:
    const_random_iterator()
        : data_(NULL), pos_(0)
    {
    }
    const_random_iterator(const random_iterator<_Tx, _Container> & non_const_iterator)
        : data_(non_const_iterator.data_), pos_(non_const_iterator.pos_)
    {
    }
    const_random_iterator(const _Container & _con, distance_type pos)
        : data_(&_con), pos_(pos)
    {
    }
    const_random_iterator(const const_random_iterator & other)
        : data_(other.data_), pos_(other.pos_)
    {
    }
    const_random_iterator & operator= (const const_random_iterator & other)
    {
        data_ = other.data_;
        pos_ = other.pos_;
        return *this;
    }

    const_reference operator*() const
    {
        return data()[pos_];
    }
    const_pointer operator->() const
    {
        return & ** this;
    }

    const_random_iterator & operator+= (distance_type rhs)
    {
        pos_ += rhs;
        return *this;
    }
    const_random_iterator & operator++()
    {
        pos_ += 1;
        return *this;
    }
    const_random_iterator operator++(int)
    {
        const_random_iterator temp(*this);
        ++(*this);
        return temp;
    }
    const_random_iterator operator+ (distance_type rhs) const
    {
        const_random_iterator temp(*this);
        return temp += rhs;
    }
    const_random_iterator operator-= (distance_type rhs)
    {
        pos_ -= rhs;
        return *this;
    }
    const_random_iterator & operator--()
    {
        pos_ -= 1;
        return *this;
    }
    const_random_iterator operator--(int)
    {
        const_random_iterator temp(*this);
        --(*this);
        return temp;
    }
    const_random_iterator operator- (distance_type rhs) const
    {
        const_random_iterator temp(*this);
        return temp -= rhs;
    }
    distance_type operator- (const const_random_iterator & rhs) const
    {
        return pos_ - rhs.pos_;
    }

    const_reference operator[](const distance_type index) const
    {
        return data()[pos_];
    }

    bool operator== (const const_random_iterator & rhs) const
    {
        if (data_ == rhs.data_
                && pos_ == rhs.pos_)
        {
            return true;
        }

        if (is_iterator_invaild() && rhs.is_iterator_invaild())
        {
            return true;
        }

        return false;
    }
    bool operator!= (const const_random_iterator & rhs) const
    {
        return !(*this == rhs);
    }
    bool operator< (const const_random_iterator & rhs) const
    {
        return *this - rhs < 0;
    }
    bool operator> (const const_random_iterator & rhs) const
    {
        return rhs < *this;
    }
    bool operator<= (const const_random_iterator & rhs) const
    {
        return !(*this > rhs);
    }
    bool operator>= (const const_random_iterator & rhs) const
    {
        return !(*this < rhs);
    }
private:
    const _Container & data() const
    {
        return *data_;
    }
    bool is_iterator_invaild() const
    {
        return data_ == NULL || pos_ < 0 || pos_ > data().size();
    }
private:
    const _Container * data_;
    distance_type pos_;
};

template<typename _Tx, typename _Container>
const_random_iterator<_Tx, _Container>
operator+ (typename const_random_iterator<_Tx, _Container>::distance_type lhs,
           const_random_iterator<_Tx, _Container> & rhs)
{
    return rhs + lhs;
}
#endif

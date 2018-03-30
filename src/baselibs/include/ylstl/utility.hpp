#ifndef __YLSTL_UTILITY_HPP__
#define __YLSTL_UTILITY_HPP__

namespace yl
{

//交换
template<typename Type>
void swap(Type & lhs, Type & rhs)
{
    Type temp = lhs;
    lhs = rhs;
    rhs = temp;
}

//不可拷贝类
class NonCopyable
{
public:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable(const NonCopyable &);
    NonCopyable & operator= (const NonCopyable &);
};

//临时储存区 使用new/delete
template<typename Type, typename SizeType>
class _TempBuf: private NonCopyable
{
public:
    typedef Type type;
    typedef SizeType size_type;
    typedef Type * iterator;
    typedef const Type * const_iterator;
public:
    explicit _TempBuf(SizeType size)
        : buf_(new Type[size]), size_(size) {}

    ~_TempBuf()
    {
        delete [] buf_;
    }

    Type * data()
    {
        return buf_;
    }
    const Type * data() const
    {
        return buf_;
    }
    iterator begin()
    {
        return buf_;
    }
    iterator end()
    {
        return buf_ + size_;
    }
    const_iterator begin()const
    {
        return buf_;
    }
    const_iterator end()const
    {
        return buf_ + size_;
    }

    size_type size()
    {
        return size_;
    }
    void detech()
    {
        buf_ = NULL;
        size_ = 0;
    }

    Type & operator[](SizeType index)
    {
        return buf_[index];
    }
private:
    Type * buf_;
    SizeType size_;
};

//临时储存区 使用alloc家族
template<typename Type, typename SizeType>
class _TempOrginBuf: private NonCopyable
{
public:
    typedef Type type;
    typedef SizeType size_type;
    typedef Type * iterator;
    typedef const Type * const_iterator;
public:
    explicit _TempOrginBuf(SizeType size)
        : buf_(NULL), size_(0)
    {
        buf_ = (type *)malloc(size * sizeof(type));
        size_ = size;
    }

    ~_TempOrginBuf()
    {
        if (buf_)
        {
            free(buf_);
        }
    }

    Type * data()
    {
        return buf_;
    }
    const Type * data() const
    {
        return buf_;
    }
    iterator begin()
    {
        return buf_;
    }
    iterator end()
    {
        return buf_ + size_;
    }
    const_iterator begin()const
    {
        return buf_;
    }
    const_iterator end()const
    {
        return buf_ + size_;
    }

    size_type size()
    {
        return size_;
    }
    void detech()
    {
        buf_ = NULL;
        size_ = 0;
    }

    Type & operator[](SizeType index)
    {
        return buf_[index];
    }
private:
    Type * buf_;
    SizeType size_;
};

//比较仿函数
template<typename Type>
struct less
{
    bool operator()(const Type & lhs, const Type & rhs) const
    {
        return lhs < rhs;
    }
};

template<typename Type>
struct more
{
    bool operator()(const Type & lhs, const Type & rhs) const
    {
        return rhs < lhs;
    }
};

template<typename Type>
struct equal
{
    bool operator()(const Type & lhs, const Type & rhs) const
    {
        return !(lhs < rhs) && !(rhs < lhs);
    }
};


template<typename FirstType, typename SecondType>
struct pair
{
    FirstType first;
    SecondType second;
};

template<typename FirstType, typename SecondType>
pair<FirstType, SecondType> make_pair(FirstType first, SecondType second)
{
    pair<FirstType, SecondType> pr;
    pr.first = first;
    pr.second = second;
    return pr;
};

//指针删除器
template<typename type>
struct pointer_deleter
{
    void operator()(type * pointer)
    {
        delete pointer;
    }
};

//数组删除器
template<typename type>
struct array_deleter
{
    void operator()(type * pointer)
    {
        delete [] pointer;
    }
};

//simple guard
#define ADD_GUARD(type, var, function) \
struct _##var##_guard_6943\
{\
    _##var##_guard_6943(type rhs):var(rhs), touch(true){}\
    ~_##var##_guard_6943(){relase();}\
    void detach(){touch = false;}\
    void relase(){if(touch){detach(); function} }\
    bool touch;\
    type var;\
} _##var##_guard_inst_6943(var)

#define DETACH_GUARD(var)    _##var##_guard_inst_6943.detach()
#define RELEASE_GUARD(var)  _##var##_guard_inst_6943.realse()

}//namespace yl
#endif //__YLSTL_STATBLE_SORT_HPP__


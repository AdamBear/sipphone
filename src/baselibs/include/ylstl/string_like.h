// -*- C++ -*-
//===--------------------------- string -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef STRING_LIKE_H
#define STRING_LIKE_H

//comment this to use std::string
#define BASE_YL_STRING

#ifndef BASE_YL_STRING

#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <assert.h>
#define _LIBCPP_ASSERT(condition, message) assert(condition)

#include "ylmove.h"
#include "utility.hpp"
#include <algorithm>
#include "ylalgorithm.h"

namespace yl
{

template<typename CharT>
class basic_string
    : public std::basic_string<CharT>
{
public:
    typedef std::basic_string<CharT>            detail_type;
    typedef yl::rref<basic_string<CharT> >      right_reference;
    typedef typename detail_type::value_type    value_type;
    typedef typename detail_type::size_type     size_type;
    typedef typename detail_type::pointer       pointer;
    typedef typename detail_type::const_pointer const_pointer;
    typedef typename detail_type::reference     reference;
    typedef typename detail_type::const_reference const_reference;

    using detail_type::npos;
public:
    basic_string()
        : detail_type()
    {
    };

    basic_string(const detail_type & other)
        : detail_type(other)
    {
    }

    basic_string(size_type count, CharT ch)
        : detail_type(count, ch)
    {
    };
    basic_string(const basic_string<CharT> & other, size_type pos, size_type count = basic_string<CharT>::npos)
        : detail_type(other, pos, count)
    {
    };

    basic_string(const CharT * s, size_type count);
    basic_string(const CharT * s);

    basic_string(const basic_string<CharT> & other)
        : detail_type(other)
    {
    };
    basic_string(const CharT * begin, const CharT * end)
    {
        size_type count = end - begin;
        this->assign(begin, count);
    };

    ~basic_string()
    {
    };

    //C++11 interface
    //move constructor
    basic_string(right_reference other)
    {
        this->swap(*other);
    }

    //pop_back
    void pop_back()
    {
        this->erase(this->size() - 1);
    }

    reference      front()
    {
        return this->at(0);
    }
    reference      back()
    {
        return this->at(this->size() - 1);
    }
    const_reference front()const
    {
        return this->at(0);
    }
    const_reference back()const
    {
        return this->at(this->size() - 1);
    }

    //extend functions
    basic_string & CopyTextFrom(const_pointer lpszText, int nCopySize)
    {
        this->assign((pointer)lpszText, nCopySize);
        return *this;
    }

    size_type GetLength() const
    {
        return this->length();
    }

    pointer get_buffer(int buffer_size)
    {
        this->resize(buffer_size);
        return get_pointer();
    }

    pointer get_pointer()
    {
        return &((*this)[0]);
    }

    //using detail_type::operator=;
    using detail_type::replace;

    //replace all <search> to <str>
    basic_string & replace(const basic_string & search, const basic_string & str)
    {
        //空字符串是所有字符串的子串，往下执行会死循环，直接返回
        if (search.empty())
        {
            return *this;
        }

        size_type pos = 0;
        while (pos < this->size())
        {
            pos = this->find(search, pos);
            if (pos != npos)
            {
                this->replace(pos, search.size(), str);
                pos += str.size(); //从替换后字符串的末尾继续查找
            }
            else
            {
                break;
            }
        }

        return *this;
    }

    basic_string substr(size_type pos = 0, size_type count = npos) const
    {
        return detail_type::substr(pos, count);
    }
    //using specialization definition <CharT = char,wchar_t>
    basic_string & to_lower();
    basic_string & to_upper();

    basic_string & MakeLower()
    {
        return to_lower();
        //return *this;
    }

    basic_string & trim_both(const basic_string & trim_str = " ")
    {
        trim_left(trim_str);
        trim_right(trim_str);
        return *this;
    }

    basic_string & trim_left(const basic_string & trim_str = " ")
    {
        size_type pos = this->find_first_not_of(trim_str);
        this->erase(0, pos);
        return *this;
    }

    basic_string & trim_right(const basic_string & trim_str = " ")
    {
        size_type pos = this->find_last_not_of(trim_str);
        this->erase(pos + 1);
        return *this;
    }
};

template<typename CharT>
inline basic_string<CharT>::basic_string(const CharT * s, size_type count)
    : detail_type(s ? s : "", count)
{
}

template<>
inline basic_string<wchar_t>::basic_string(const wchar_t * s, size_type count)
    : detail_type(s ? s : L"", count)
{
}

template<typename CharT>
inline basic_string<CharT>::basic_string(const CharT * s)
    : detail_type(s ? s : "")
{
}
template<>
inline basic_string<wchar_t>::basic_string(const wchar_t * s)
    : detail_type(s ? s : L"")
{
}

//+操作符
template<typename CharT>
basic_string<CharT>
inline operator+ (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    basic_string<CharT> temp(lhs);
    return temp.append(rhs);
}

template<typename CharT>
basic_string<CharT>
inline operator+ (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return basic_string<CharT>(lhs) + rhs;
}

template<typename CharT>
basic_string<CharT>
inline operator+ (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return lhs + basic_string<CharT>(rhs);
}

//比较操作符
//operator ==
template<class CharT>
inline bool operator== (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    return !lhs.compare(rhs);
}

template<class CharT>
inline bool operator== (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return !lhs.compare(rhs);
}

template<class CharT>
inline bool operator== (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return !rhs.compare(lhs);
}

//operator !=
template<class CharT>
inline bool operator!= (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!= (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!= (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return !(lhs == rhs);
}


//operator <
template<class CharT>
inline bool operator< (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    return lhs.compare(rhs) < 0;
}

template<class CharT>
inline bool operator< (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return lhs.compare(rhs) < 0;
}

template<class CharT>
inline bool operator< (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return rhs.compare(lhs) > 0;
}

//operator <=
template<class CharT>
inline bool operator<= (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<class CharT>
inline bool operator<= (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<class CharT>
inline bool operator<= (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return lhs < rhs || lhs == rhs;
}

//operator >
template<class CharT>
inline bool operator> (const basic_string<CharT> & lhs, const basic_string<CharT> & rhs)
{
    return !(lhs <= rhs);
}

template<class CharT>
inline bool operator> (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return !(lhs <= rhs);
}

template<class CharT>
inline bool operator> (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return !(lhs <= rhs);
}

//operator >=
template<class CharT>
inline bool operator>= (const basic_string<CharT> & lhs, const  basic_string<CharT> & rhs)
{
    return !(lhs < rhs);
}

template<class CharT>
inline bool operator>= (const basic_string<CharT> & lhs, const CharT * rhs)
{
    return !(lhs < rhs);
}

template<class CharT>
inline bool operator>= (const CharT * lhs, const basic_string<CharT> & rhs)
{
    return !(lhs < rhs);
}

//to_lower specialization
template<typename CharT>
basic_string<CharT> & basic_string<CharT>::to_lower()
{
    //STATIC_ASSERT(false);
    return *this;
}

template<>
inline basic_string<char> & basic_string<char>::to_lower()
{
    std::transform(this->begin(), this->end(), this->begin(), ::tolower);
    return *this;
}

template<>
inline basic_string<wchar_t> & basic_string<wchar_t>::to_lower()
{
    std::transform(this->begin(), this->end(), this->begin(), ::towlower);
    return *this;
}

//to_upper specialization
template<typename CharT>
basic_string<CharT> & basic_string<CharT>::to_upper()
{
    //STATIC_ASSERT(false);
    return *this;
}

template<>
inline basic_string<char> & basic_string<char>::to_upper()
{
    std::transform(this->begin(), this->end(), this->begin(), ::toupper);
    return *this;
}

template<>
inline basic_string<wchar_t> & basic_string<wchar_t>::to_upper()
{
    std::transform(this->begin(), this->end(), this->begin(), ::towupper);
    return *this;
}

}//namespace yl


#else

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <malloc.h>
#include <assert.h>
#define _LIBCPP_ASSERT(condition, message) assert(condition)

#include "ylmove.h"
#include "ylalgorithm.h"
#include "utility.hpp"

#ifndef nullptr
#define nullptr NULL
#endif // !nullptr

#define ALIGN_TO_16 1


namespace yl
{
template<typename value_type>
size_t string_length(const value_type * __s)
{
    if (__s == nullptr)
    {
        return 0;
    }
    size_t count = 0;
    while (*__s)
    {
        ++__s;
        ++count;
    }
    return count;
}

// basic_string
template<class _CharT>
class basic_string
{
public:
    typedef basic_string                                 __self;
    typedef _CharT                                       value_type;
    typedef unsigned int                                 size_type;
    typedef int                                          difference_type;
    typedef value_type                 &                 reference;
    typedef const value_type              &              const_reference;
    typedef value_type                 *                 pointer;
    typedef const value_type              *              const_pointer;

    typedef pointer                                      iterator;
    typedef const_pointer                                const_iterator;
    typedef yl::rref<basic_string<value_type> >          right_reference;

private:

    struct __long
    {
        size_type __cap_;
        size_type __size_;
        pointer   __data_;
    };

#if _LIBCPP_BIG_ENDIAN
    enum { __short_mask = 0x80 };
    enum { __long_mask = ~(size_type(~0) >> 1) };
#else  // _LIBCPP_BIG_ENDIAN
    enum { __short_mask = 0x01 };
    enum { __long_mask = 0x1ul };
#endif  // _LIBCPP_BIG_ENDIAN

#if ALIGN_TO_16
    enum { __min_size = 16 };
    enum { __size = sizeof(__long) > __min_size ? sizeof(__long) : __min_size };
    enum
    {
        __min_cap = (__size - 1) / sizeof(value_type) > 2 ?
                    (__size - 1) / sizeof(value_type) : 2
    };
#else
    enum
    {
        __min_cap = (sizeof(__long) - 1) / sizeof(value_type) > 2 ?
                    (sizeof(__long) - 1) / sizeof(value_type) : 2
    };
#endif // ALIGN_TO_16

    struct __short
    {
        union
        {
            unsigned char __size_;
            value_type __lx;
        };
        value_type __data_[__min_cap];
    };


    union __ulx
    {
        __long __lx;
        __short __lxx;
    };

    enum { __n_words = sizeof(__ulx) / sizeof(size_type) };

    struct __raw
    {
        size_type __words[__n_words];
    };

    struct __rep
    {
        union
        {
            __long  __l;
            __short __s;
            __raw   __r;
        };
    };

    __rep __r_;

public:
    static const size_type npos = -1;

    basic_string();
    basic_string(const basic_string & __str);
    basic_string(const value_type * __s);
    basic_string(const value_type * __s, size_type __n);
    basic_string(size_type __n, value_type __c);
    basic_string(const basic_string & __str, size_type __pos, size_type __n = npos);
    basic_string(right_reference __str);
    basic_string(yl::rref<pointer> __s, size_type __sz = 0, size_type __n = 0);
    basic_string(const_iterator __first, const_iterator __last);
    ~basic_string();

    basic_string & operator=(const basic_string & __str);
    basic_string & operator=(const value_type * __s)
    {
        return assign(__s);
    }
    basic_string & operator=(value_type __c);

    iterator begin()
    {
        return iterator(__get_pointer());
    }

    const_iterator begin() const
    {
        return const_iterator(__get_pointer());
    }

    iterator end()
    {
        return iterator(__get_pointer() + size());
    }

    const_iterator end() const
    {
        return const_iterator(__get_pointer() + size());
    }

    size_type size() const
    {
        return __is_long() ? __get_long_size() : __get_short_size();
    }
    size_type length() const
    {
        return size();
    }
    size_type GetLength() const
    {
        return length();
    }
    size_type max_size() const;
    size_type capacity() const
    {
        return (__is_long() ? __get_long_cap() : static_cast<size_type>(__min_cap)) - 1;
    }

    void resize(size_type __n, value_type __c);
    void resize(size_type __n)
    {
        resize(__n, value_type());
    }

    void reserve(size_type res_arg = 0);

    void clear();
    bool empty() const
    {
        return size() == 0;
    }

    const_reference operator[](size_type __pos) const;
    reference       operator[](size_type __pos);

    const_reference at(size_type __n) const;
    reference       at(size_type __n);

    basic_string & operator+=(const basic_string & __str)
    {
        return append(__str);
    }
    basic_string & operator+=(const value_type * __s)
    {
        return append(__s);
    }
    basic_string & operator+=(value_type __c)
    {
        push_back(__c);
        return *this;
    }

    basic_string & append(const basic_string & __str);
    basic_string & append(const basic_string & __str, size_type __pos, size_type __n = npos);
    basic_string & append(const value_type * __s, size_type __n);
    basic_string & append(const value_type * __s);
    basic_string & append(size_type __n, value_type __c);

    void push_back(value_type __c);
    void pop_back();

    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;

    basic_string & assign(const basic_string & __str)
    {
        return *this = __str;
    }
    basic_string & assign(const basic_string & __str, size_type __pos, size_type __n = npos);
    basic_string & assign(const value_type * __s, size_type __n);
    basic_string & assign(const value_type * __s);
    basic_string & assign(size_type __n, value_type __c);

    basic_string & insert(size_type __pos1, const basic_string & __str);
    basic_string & insert(size_type __pos1, const basic_string & __str, size_type __pos2,
                          size_type __n = npos);
    basic_string & insert(size_type __pos, const value_type * __s, size_type __n);
    basic_string & insert(size_type __pos, const value_type * __s);
    basic_string & insert(size_type __pos, size_type __n, value_type __c);

    //iterator      insert(const_iterator __pos, value_type __c);
    //iterator      insert(const_iterator __pos, size_type __n, value_type __c);

    basic_string & erase(size_type __pos = 0, size_type __n = npos);
    iterator      erase(const_iterator __pos);
    iterator      erase(const_iterator __first, const_iterator __last);

    basic_string & replace(size_type __pos1, size_type __n1, const basic_string & __str);
    basic_string & replace(size_type __pos1, size_type __n1, const basic_string & __str,
                           size_type __pos2, size_type __n2 = npos);
    basic_string & replace(size_type __pos, size_type __n1, const value_type * __s, size_type __n2);
    basic_string & replace(size_type __pos, size_type __n1, const value_type * __s);
    basic_string & replace(size_type __pos, size_type __n1, size_type __n2, value_type __c);

    basic_string & replace(const basic_string & search, const basic_string & str)
    {
        // 空字符串是所有字符串的子串，往下执行会死循环，直接返回
        if (search.empty())
        {
            return *this;
        }
        size_type pos = 0;
        while (pos < this->size())
        {
            pos = this->find(search, pos);
            if (pos != npos)
            {
                this->replace(pos, search.size(), str);
                pos += str.size(); // 从替换后字符串的末尾继续查找
            }
            else
            {
                break;
            }
        }
        return *this;
    }

    basic_string & replace(const_iterator __i1, const_iterator __i2, const basic_string & __str);
    basic_string & replace(const_iterator __i1, const_iterator __i2, const value_type * __s,
                           size_type __n);
    basic_string & replace(const_iterator __i1, const_iterator __i2, const value_type * __s);
    basic_string & replace(const_iterator __i1, const_iterator __i2, size_type __n, value_type __c);

    size_type copy(value_type * __s, size_type __n, size_type __pos = 0) const;

    basic_string substr(size_type __pos = 0, size_type __n = npos) const;

    void swap(basic_string & __str);

    const value_type * c_str() const
    {
        return data();
    }
    const value_type * data() const
    {
        return __get_pointer();
    }

    basic_string & CopyTextFrom(const_pointer __s, int __n)
    {
        assign(__s, __n);
        return *this;
    }
    pointer get_buffer(int __n)
    {
        resize(__n);
        return begin();
    }

    size_type find(const basic_string & __str, size_type __pos = 0) const;
    size_type find(const value_type * __s, size_type __pos, size_type __n) const;
    size_type find(const value_type * __s, size_type __pos = 0) const;
    size_type find(value_type __c, size_type __pos = 0) const;

    size_type rfind(const basic_string & __str, size_type __pos = npos) const;
    size_type rfind(const value_type * __s, size_type __pos, size_type __n) const;
    size_type rfind(const value_type * __s, size_type __pos = npos) const;
    size_type rfind(value_type __c, size_type __pos = npos) const;

    size_type find_first_of(const basic_string & __str, size_type __pos = 0) const;
    size_type find_first_of(const value_type * __s, size_type __pos, size_type __n) const;
    size_type find_first_of(const value_type * __s, size_type __pos = 0) const;
    size_type find_first_of(value_type __c, size_type __pos = 0) const;

    size_type find_last_of(const basic_string & __str, size_type __pos = npos) const;
    size_type find_last_of(const value_type * __s, size_type __pos, size_type __n) const;
    size_type find_last_of(const value_type * __s, size_type __pos = npos) const;
    size_type find_last_of(value_type __c, size_type __pos = npos) const;

    size_type find_first_not_of(const basic_string & __str, size_type __pos = 0) const;
    size_type find_first_not_of(const value_type * __s, size_type __pos, size_type __n) const;
    size_type find_first_not_of(const value_type * __s, size_type __pos = 0) const;
    size_type find_first_not_of(value_type __c, size_type __pos = 0) const;

    size_type find_last_not_of(const basic_string & __str, size_type __pos = npos) const;
    size_type find_last_not_of(const value_type * __s, size_type __pos, size_type __n) const;
    size_type find_last_not_of(const value_type * __s, size_type __pos = npos) const;
    size_type find_last_not_of(value_type __c, size_type __pos = npos) const;

    int compare(const basic_string & __str) const;
    int compare(size_type __pos1, size_type __n1, const basic_string & __str) const;
    int compare(size_type __pos1, size_type __n1, const basic_string & __str, size_type __pos2,
                size_type __n2 = npos) const;
    int compare(const value_type * __s) const;
    int compare(size_type __pos1, size_type __n1, const value_type * __s) const;
    int compare(size_type __pos1, size_type __n1, const value_type * __s, size_type __n2) const;

    // using specialization definition <CharT = char,wchar_t>
    basic_string & to_lower();
    basic_string & to_upper();
    basic_string & MakeLower()
    {
        return to_lower();
    }
    basic_string & trim_both(const basic_string & trim_str = " ")
    {
        trim_left(trim_str);
        trim_right(trim_str);
        return *this;
    }
    basic_string & trim_left(const basic_string & trim_str = " ")
    {
        size_type pos = find_first_not_of(trim_str);
        erase(0, pos);
        return *this;
    }
    basic_string & trim_right(const basic_string & trim_str = " ")
    {
        size_type pos = find_last_not_of(trim_str);
        erase(pos + 1);
        return *this;
    }

    bool __is_long() const
    {
        return bool(__r_.__s.__size_ & __short_mask);
    }

private:

    void __set_short_size(size_type __s)
#   if _LIBCPP_BIG_ENDIAN
    {
        __r_.__s.__size_ = (unsigned char)(__s);
    }
#   else
    {
        __r_.__s.__size_ = (unsigned char)(__s << 1);
    }
#   endif


    size_type __get_short_size() const
#   if _LIBCPP_BIG_ENDIAN
    {
        return __r_.__s.__size_;
    }
#   else
    {
        return __r_.__s.__size_ >> 1;
    }
#   endif


    void __set_long_size(size_type __s)
    {
        __r_.__l.__size_ = __s;
    }

    size_type __get_long_size() const
    {
        return __r_.__l.__size_;
    }

    void __set_size(size_type __s)
    {
        if (__is_long())
        {
            __set_long_size(__s);
        }
        else
        {
            __set_short_size(__s);
        }
    }

    void __set_long_cap(size_type __s)
    {
        __r_.__l.__cap_ = __long_mask | __s;
    }

    size_type __get_long_cap() const
    {
        return __r_.__l.__cap_ & size_type(~__long_mask);
    }

    void __set_long_pointer(pointer __p)
    {
        __r_.__l.__data_ = __p;
    }

    pointer __get_long_pointer()
    {
        return __r_.__l.__data_;
    }

    const_pointer __get_long_pointer() const
    {
        return __r_.__l.__data_;
    }

    pointer __get_short_pointer()
    {
        return &(__r_.__s.__data_[0]);
    }

    const_pointer __get_short_pointer() const
    {
        return &(__r_.__s.__data_[0]);
    }

    pointer __get_pointer()
    {
        return __is_long() ? __get_long_pointer() : __get_short_pointer();
    }

    const_pointer __get_pointer() const
    {
        return __is_long() ? __get_long_pointer() : __get_short_pointer();
    }

    void __zero()
    {
        size_type(&__a)[__n_words] = __r_.__r.__words;
        for (unsigned __i = 0; __i < __n_words; ++__i)
        {
            __a[__i] = 0;
        }
    }

    template <size_type __a>
    static size_type __align_it(size_type __s)
    {
        return (__s + (__a - 1)) & ~(__a - 1);
    }

    enum { __alignment = 16 };
    static size_type __recommend(size_type __s)
    {
        return (__s < __min_cap ? static_cast<size_type>(__min_cap) :
                __align_it < (sizeof(value_type) < __alignment ? __alignment / sizeof(value_type) : 1) >
                (__s + 1)) - 1;
    }

    void __init(const value_type * __s, size_type __sz, size_type __reserve);
    void __init(const value_type * __s, size_type __sz);
    void __init(size_type __n, value_type __c);

    void __grow_by(size_type __old_cap, size_type __delta_cap, size_type __old_sz,
                   size_type __n_copy, size_type __n_del, size_type __n_add = 0);
    void __grow_by_and_replace(size_type __old_cap, size_type __delta_cap, size_type __old_sz,
                               size_type __n_copy, size_type __n_del, size_type __n_add, const value_type * __p_new_stuff);

    void __erase_to_end(size_type __pos);

    // for value_type
    static value_type & __vt_assign(value_type & __d, value_type __s)
    {
        __d = __s;
        return __d;
    }
    static void __vt_assign(value_type * __s, size_type __n, value_type __c)
    {
        for (; __n; ++__s, --__n)
        {
            *__s = __c;
        }
    }
    static void __vt_copy(value_type * __d, const value_type * __s, size_type __n)
    {
        memcpy(__d, __s, __n * sizeof(value_type));
    }
    static void __vt_move(value_type * __l, const value_type * __r, size_type __n)
    {
        size_type i = 0;
        if (__l < __r)
        {
            i = 0; // 从前往后复制
        }
        else if (__l > __r)
        {
            i = __n - 1; // 从后往前复制
        }
        else // __l == __r
        {
            return;
        }
        while (i >= 0 && i < __n)
        {
            __l[i] = __r[i];
            if (__l < __r)
            {
                ++i; // 从前往后复制
            }
            else
            {
                --i; // 从后往前复制
            }
        }
    }
    static int __vt_compare(const value_type * __l, const value_type * __r, size_type __n)
    {
        for (; __n; --__n, ++__l, ++__r)
        {
            if (*__l != *__r)
            {
                return static_cast<int>((*__l) - (*__r));
            }
        }
        return 0;
    }
    static pointer __vt_allocate(size_type __n)
    {
        return static_cast<pointer>(malloc(__n * sizeof(value_type)));
    }
    static void __vt_deallocate(pointer __p, size_type)
    {
        free(__p);
    }

    //find...of系列函数的实现函数
    size_type find_of(const value_type * __s, size_type __pos, size_type __n, int __incl_value,
                      bool __found_char) const;

    //字符串中查找字符
    static size_type find_char_in_str(value_type __c, const value_type * __s, size_type __n = npos)
    {
        if (__s == nullptr)
        {
            return npos;
        }
        const value_type * pos = __s;
        size_type cmpcount = 0;
        while (*pos && cmpcount < __n)
        {
            if (*pos == __c)
            {
                return cmpcount;
            }
            ++pos;
            ++cmpcount;
        }
        return npos;
    };
};


//template<class _CharT>
//basic_string<_CharT>
//inline operator+(const basic_string<_CharT>& __x, const basic_string<_CharT>& __y);
//
//template<class _CharT>
//basic_string<_CharT>
//inline operator+(const _CharT* __x, const basic_string<_CharT>& __y);
//
//template<class _CharT>
//basic_string<_CharT>
//inline operator+(_CharT __x, const basic_string<_CharT>& __y);
//
//template<class _CharT>
//basic_string<_CharT>
//inline operator+(const basic_string<_CharT>& __x, const _CharT* __y);
//
//template<class _CharT>
//basic_string<_CharT>
//inline operator+(const basic_string<_CharT>& __x, _CharT __y);
//



template <class _CharT>
inline basic_string<_CharT>::basic_string()
{
    __zero();
}

template <class _CharT>
inline void basic_string<_CharT>::__init(const value_type * __s, size_type __sz,
        size_type __reserve)
{
    if (__reserve > max_size())
    {
        return;
    }
    pointer __p;
    if (__reserve < __min_cap)
    {
        __set_short_size(__sz);
        __p = __get_short_pointer();
    }
    else
    {
        size_type __cap = __recommend(__reserve);
        __p = __vt_allocate(__cap + 1);
        __set_long_pointer(__p);
        __set_long_cap(__cap + 1);
        __set_long_size(__sz);
    }
    __vt_copy(__p, __s, __sz);
    __vt_assign(__p[__sz], value_type());
}

template <class _CharT>
inline void basic_string<_CharT>::__init(const value_type * __s, size_type __sz)
{
    if (__sz > max_size())
    {
        return;
    }
    pointer __p;
    if (__sz < __min_cap)
    {
        __set_short_size(__sz);
        __p = __get_short_pointer();
    }
    else
    {
        size_type __cap = __recommend(__sz);
        __p = __vt_allocate(__cap + 1);
        __set_long_pointer(__p);
        __set_long_cap(__cap + 1);
        __set_long_size(__sz);
    }
    __vt_copy(__p, __s, __sz);
    __vt_assign(__p[__sz], value_type());
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(const value_type * __s)
{
    //_LIBCPP_ASSERT(__s != nullptr, "basic_string(const char*) detected nullptr");
    if (__s == nullptr)
    {
        __zero();
        return;
    }
    __init(__s, string_length(__s));
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(const value_type * __s, size_type __n)
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "basic_string(const char*, n) detected nullptr");
    __init(__s, __n);
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(const basic_string & __str)
{
    if (!__str.__is_long())
    {
        __r_.__r = __str.__r_.__r;
    }
    else
    {
        __init(__str.__get_long_pointer(), __str.__get_long_size());
    }
}

template <class _CharT>
inline void basic_string<_CharT>::__init(size_type __n, value_type __c)
{
    if (__n > max_size())
    {
        return;
    }
    pointer __p;
    if (__n < __min_cap)
    {
        __set_short_size(__n);
        __p = __get_short_pointer();
    }
    else
    {
        size_type __cap = __recommend(__n);
        __p = __vt_allocate(__cap + 1);
        __set_long_pointer(__p);
        __set_long_cap(__cap + 1);
        __set_long_size(__n);
    }
    __vt_assign(__p, __n, __c);
    __vt_assign(__p[__n], value_type());
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(size_type __n, value_type __c)
{
    __init(__n, __c);
}

template<class _CharT>
inline basic_string<_CharT>::basic_string(const basic_string & __str, size_type __pos,
        size_type __n)
{
    __zero();
    assign(__str, __pos, __n);
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(right_reference __str)
{
    __zero();
    this->swap(*__str);
}

template <class _CharT>
inline basic_string<_CharT>::basic_string(yl::rref<pointer> __s, size_type __sz, size_type __n)
{
    if (*__s == nullptr)
    {
        __zero();
        return;
    }
    if (__sz == 0)
    {
        __sz = string_length(*__s);
    }
    if (__n == 0)
    {
        __n = __sz + 1;
    }
    if (__sz < __min_cap)
    {
        __set_short_size(__sz);
        pointer __p = __get_short_pointer();
        __vt_copy(__p, *__s, __sz);
        __vt_assign(__p[__sz], value_type());
        __vt_deallocate(*__s, __n);
    }
    else
    {
        size_type __cap = __n;
        pointer __p = *__s;
        __set_long_pointer(__p);
        __set_long_cap(__cap);
        __set_long_size(__sz);
    }
    *__s = nullptr;
}

template<class _CharT>
inline basic_string<_CharT>::basic_string(const_iterator __first, const_iterator __last)
{
    if (__first == nullptr || __last == nullptr || __last <= __first)
    {
        __zero();
        return;
    }
    __init(__first, __last - __first);
}

template <class _CharT>
basic_string<_CharT>::~basic_string()
{
    if (__is_long())
    {
        __vt_deallocate(__get_long_pointer(), __get_long_cap());
    }
}

template <class _CharT>
void basic_string<_CharT>::__grow_by_and_replace(size_type __old_cap, size_type __delta_cap,
        size_type __old_sz,
        size_type __n_copy, size_type __n_del, size_type __n_add, const value_type * __p_new_stuff)
{
    size_type __ms = max_size();
    if (__delta_cap > __ms - __old_cap - 1)
    {
        return;
    }
    pointer __old_p = __get_pointer();
    size_type __cap = __old_cap < __ms / 2 - __alignment ?
                      __recommend(max(__old_cap + __delta_cap, 2 * __old_cap)) :
                      __ms - 1;
    pointer __p = __vt_allocate(__cap + 1);
    if (__n_copy != 0)
    {
        __vt_copy(__p, __old_p, __n_copy);
    }
    if (__n_add != 0)
    {
        __vt_copy(__p + __n_copy, __p_new_stuff, __n_add);
    }
    size_type __sec_cp_sz = __old_sz - __n_del - __n_copy;
    if (__sec_cp_sz != 0)
    {
        __vt_copy(__p + __n_copy + __n_add, __old_p + __n_copy + __n_del, __sec_cp_sz);
    }
    if (__old_cap + 1 != __min_cap)
    {
        __vt_deallocate(__old_p, __old_cap + 1);
    }
    __set_long_pointer(__p);
    __set_long_cap(__cap + 1);
    __old_sz = __n_copy + __n_add + __sec_cp_sz;
    __set_long_size(__old_sz);
    __vt_assign(__p[__old_sz], value_type());
}

template <class _CharT>
void basic_string<_CharT>::__grow_by(size_type __old_cap, size_type __delta_cap, size_type __old_sz,
                                     size_type __n_copy, size_type __n_del, size_type __n_add)
{
    size_type __ms = max_size();
    if (__delta_cap > __ms - __old_cap)
    {
        return;
    }
    pointer __old_p = __get_pointer();
    size_type __cap = __old_cap < __ms / 2 - __alignment ?
                      __recommend(max(__old_cap + __delta_cap, 2 * __old_cap)) :
                      __ms - 1;
    pointer __p = __vt_allocate(__cap + 1);
    if (__n_copy != 0)
    {
        __vt_copy(__p, __old_p, __n_copy);
    }
    size_type __sec_cp_sz = __old_sz - __n_del - __n_copy;
    if (__sec_cp_sz != 0)
        __vt_copy(__p + __n_copy + __n_add, __old_p + __n_copy + __n_del,
                  __sec_cp_sz);
    if (__old_cap + 1 != __min_cap)
    {
        __vt_deallocate(__old_p, __old_cap + 1);
    }
    __set_long_pointer(__p);
    __set_long_cap(__cap + 1);
}

// assign

template <class _CharT>
basic_string<_CharT> & basic_string<_CharT>::assign(const value_type * __s, size_type __n)
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::assign received nullptr");
    size_type __cap = capacity();
    if (__cap >= __n)
    {
        value_type * __p = __get_pointer();
        __vt_move(__p, __s, __n);
        __vt_assign(__p[__n], value_type());
        __set_size(__n);
    }
    else
    {
        size_type __sz = size();
        __grow_by_and_replace(__cap, __n - __cap, __sz, 0, __sz, __n, __s);
    }
    return *this;
}

template <class _CharT>
basic_string<_CharT> & basic_string<_CharT>::assign(size_type __n, value_type __c)
{
    size_type __cap = capacity();
    if (__cap < __n)
    {
        size_type __sz = size();
        __grow_by(__cap, __n - __cap, __sz, 0, __sz);
    }
    value_type * __p = __get_pointer();
    __vt_assign(__p, __n, __c);
    __vt_assign(__p[__n], value_type());
    __set_size(__n);
    return *this;
}

template <class _CharT>
basic_string<_CharT> & basic_string<_CharT>::operator=(value_type __c)
{
    pointer __p;
    if (__is_long())
    {
        __p = __get_long_pointer();
        __set_long_size(1);
    }
    else
    {
        __p = __get_short_pointer();
        __set_short_size(1);
    }
    __vt_assign(*__p, __c);
    __vt_assign(*++__p, value_type());
    return *this;
}

template <class _CharT>
basic_string<_CharT> & basic_string<_CharT>::operator=(const basic_string & __str)
{
    if (this != &__str)
    {
        assign(__str.data(), __str.size());
    }
    return *this;
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::assign(const basic_string & __str, size_type __pos, size_type __n)
{
    size_type __sz = __str.size();
    assert(__pos <= __sz);
    return assign(__str.data() + __pos, min(__n, __sz - __pos));
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::assign(const value_type * __s)
{
    //_LIBCPP_ASSERT(__s != nullptr, "string::assign received nullptr");
    if (__s == nullptr)
    {
        clear();
        return *this;
    }
    return assign(__s, string_length(__s));
}

// append

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::append(const value_type * __s, size_type __n)
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::append received nullptr");
    size_type __cap = capacity();
    size_type __sz = size();
    if (__cap - __sz >= __n)
    {
        if (__n)
        {
            value_type * __p = __get_pointer();
            __vt_copy(__p + __sz, __s, __n);
            __sz += __n;
            __set_size(__sz);
            __vt_assign(__p[__sz], value_type());
        }
    }
    else
    {
        __grow_by_and_replace(__cap, __sz + __n - __cap, __sz, __sz, 0, __n, __s);
    }
    return *this;
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::append(size_type __n, value_type __c)
{
    if (__n)
    {
        size_type __cap = capacity();
        size_type __sz = size();
        if (__cap - __sz < __n)
        {
            __grow_by(__cap, __sz + __n - __cap, __sz, __sz, 0);
        }
        pointer __p = __get_pointer();
        __vt_assign(__p + __sz, __n, __c);
        __sz += __n;
        __set_size(__sz);
        __vt_assign(__p[__sz], value_type());
    }
    return *this;
}

template <class _CharT>
void
basic_string<_CharT>::push_back(value_type __c)
{
    bool __is_short = !__is_long();
    size_type __cap;
    size_type __sz;
    if (__is_short)
    {
        __cap = __min_cap - 1;
        __sz = __get_short_size();
    }
    else
    {
        __cap = __get_long_cap() - 1;
        __sz = __get_long_size();
    }
    if (__sz == __cap)
    {
        __grow_by(__cap, 1, __sz, __sz, 0);
        __is_short = !__is_long();
    }
    pointer __p;
    if (__is_short)
    {
        __p = __get_short_pointer() + __sz;
        __set_short_size(__sz + 1);
    }
    else
    {
        __p = __get_long_pointer() + __sz;
        __set_long_size(__sz + 1);
    }
    __vt_assign(*__p, __c);
    __vt_assign(*++__p, value_type());
}


template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::append(const basic_string & __str)
{
    return append(__str.data(), __str.size());
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::append(const basic_string & __str, size_type __pos, size_type __n)
{
    size_type __sz = __str.size();
    assert(__pos <= __sz);
    return append(__str.data() + __pos, min(__n, __sz - __pos));
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::append(const value_type * __s)
{
    _LIBCPP_ASSERT(__s != nullptr, "string::append received nullptr");
    return append(__s, string_length(__s));
}

// insert

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::insert(size_type __pos, const value_type * __s, size_type __n)
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::insert received nullptr");
    size_type __sz = size();
    assert(__pos <= __sz);
    size_type __cap = capacity();
    if (__cap - __sz >= __n)
    {
        if (__n)
        {
            value_type * __p = __get_pointer();
            size_type __n_move = __sz - __pos;
            if (__n_move != 0)
            {
                if (__p + __pos <= __s && __s < __p + __sz)
                {
                    __s += __n;
                }
                __vt_move(__p + __pos + __n, __p + __pos, __n_move);
            }
            __vt_move(__p + __pos, __s, __n);
            __sz += __n;
            __set_size(__sz);
            __vt_assign(__p[__sz], value_type());
        }
    }
    else
    {
        __grow_by_and_replace(__cap, __sz + __n - __cap, __sz, __pos, 0, __n, __s);
    }
    return *this;
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::insert(size_type __pos, size_type __n, value_type __c)
{
    size_type __sz = size();
    assert(__pos <= __sz);
    if (__n)
    {
        size_type __cap = capacity();
        value_type * __p;
        if (__cap - __sz >= __n)
        {
            __p = __get_pointer();
            size_type __n_move = __sz - __pos;
            if (__n_move != 0)
            {
                __vt_move(__p + __pos + __n, __p + __pos, __n_move);
            }
        }
        else
        {
            __grow_by(__cap, __sz + __n - __cap, __sz, __pos, 0, __n);
            __p = __get_long_pointer();
        }
        __vt_assign(__p + __pos, __n, __c);
        __sz += __n;
        __set_size(__sz);
        __vt_assign(__p[__sz], value_type());
    }
    return *this;
}

template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::insert(size_type __pos1, const basic_string & __str)
{
    return insert(__pos1, __str.data(), __str.size());
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::insert(size_type __pos1, const basic_string & __str,
                             size_type __pos2, size_type __n)
{
    size_type __str_sz = __str.size();
    assert(__pos2 <= __str_sz);
    return insert(__pos1, __str.data() + __pos2, min(__n, __str_sz - __pos2));
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::insert(size_type __pos, const value_type * __s)
{
    _LIBCPP_ASSERT(__s != nullptr, "string::insert received nullptr");
    return insert(__pos, __s, string_length(__s));
}

//template <class _CharT>
//typename basic_string<_CharT>::iterator
//basic_string<_CharT>::insert(const_iterator __pos, value_type __c)
//{
//  size_type __ip = static_cast<size_type>(__pos - begin());
//  size_type __sz = size();
//  size_type __cap = capacity();
//  value_type* __p;
//  if (__cap == __sz)
//  {
//      __grow_by(__cap, 1, __sz, __ip, 0, 1);
//      __p = __get_long_pointer();
//  }
//  else
//  {
//      __p = __get_pointer();
//      size_type __n_move = __sz - __ip;
//      if (__n_move != 0)
//          __vt_move(__p + __ip + 1, __p + __ip, __n_move);
//  }
//  __vt_assign(__p[__ip], __c);
//  __vt_assign(__p[++__sz], value_type());
//  __set_size(__sz);
//  return begin() + static_cast<difference_type>(__ip);
//}
//
//template <class _CharT>
//inline
//typename basic_string<_CharT>::iterator
//basic_string<_CharT>::insert(const_iterator __pos, size_type __n, value_type __c)
//{
//#if _LIBCPP_DEBUG_LEVEL >= 2
//  _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__pos) == this,
//      "string::insert(iterator, n, value) called with an iterator not"
//      " referring to this string");
//#endif
//  difference_type __p = __pos - begin();
//  insert(static_cast<size_type>(__p), __n, __c);
//  return begin() + __p;
//}

// replace

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::replace(size_type __pos, size_type __n1, const value_type * __s,
                              size_type __n2)
{
    _LIBCPP_ASSERT(__n2 == 0 || __s != nullptr, "string::replace received nullptr");
    size_type __sz = size();
    assert(__pos <= __sz);
    __n1 = min(__n1, __sz - __pos);
    size_type __cap = capacity();
    if (__cap - __sz + __n1 >= __n2)
    {
        value_type * __p = __get_pointer();
        if (__n1 != __n2)
        {
            size_type __n_move = __sz - __pos - __n1;
            if (__n_move != 0)
            {
                if (__n1 > __n2)
                {
                    __vt_move(__p + __pos, __s, __n2);
                    __vt_move(__p + __pos + __n2, __p + __pos + __n1, __n_move);
                    goto __finish;
                }
                if (__p + __pos < __s && __s < __p + __sz)
                {
                    if (__p + __pos + __n1 <= __s)
                    {
                        __s += __n2 - __n1;
                    }
                    else // __p + __pos < __s < __p + __pos + __n1
                    {
                        __vt_move(__p + __pos, __s, __n1);
                        __pos += __n1;
                        __s += __n2;
                        __n2 -= __n1;
                        __n1 = 0;
                    }
                }
                __vt_move(__p + __pos + __n2, __p + __pos + __n1, __n_move);
            }
        }
        __vt_move(__p + __pos, __s, __n2);
__finish:
        __sz += __n2 - __n1;
        __set_size(__sz);
        __vt_assign(__p[__sz], value_type());
    }
    else
    {
        __grow_by_and_replace(__cap, __sz - __n1 + __n2 - __cap, __sz, __pos, __n1, __n2, __s);
    }
    return *this;
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::replace(size_type __pos, size_type __n1, size_type __n2, value_type __c)
{
    size_type __sz = size();
    assert(__pos <= __sz);
    __n1 = min(__n1, __sz - __pos);
    size_type __cap = capacity();
    value_type * __p;
    if (__cap - __sz + __n1 >= __n2)
    {
        __p = __get_pointer();
        if (__n1 != __n2)
        {
            size_type __n_move = __sz - __pos - __n1;
            if (__n_move != 0)
            {
                __vt_move(__p + __pos + __n2, __p + __pos + __n1, __n_move);
            }
        }
    }
    else
    {
        __grow_by(__cap, __sz - __n1 + __n2 - __cap, __sz, __pos, __n1, __n2);
        __p = __get_long_pointer();
    }
    __vt_assign(__p + __pos, __n2, __c);
    __sz += __n2 - __n1;
    __set_size(__sz);
    __vt_assign(__p[__sz], value_type());
    return *this;
}


template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::replace(size_type __pos1, size_type __n1, const basic_string & __str)
{
    return replace(__pos1, __n1, __str.data(), __str.size());
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::replace(size_type __pos1, size_type __n1, const basic_string & __str,
                              size_type __pos2, size_type __n2)
{
    size_type __str_sz = __str.size();
    assert(__pos2 <= __str_sz);
    return replace(__pos1, __n1, __str.data() + __pos2, min(__n2, __str_sz - __pos2));
}

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::replace(size_type __pos, size_type __n1, const value_type * __s)
{
    _LIBCPP_ASSERT(__s != nullptr, "string::replace received nullptr");
    return replace(__pos, __n1, __s, string_length(__s));
}

template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::replace(const_iterator __i1, const_iterator __i2, const basic_string & __str)
{
    return replace(static_cast<size_type>(__i1 - begin()), static_cast<size_type>(__i2 - __i1),
                   __str.data(), __str.size());
}

template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::replace(const_iterator __i1, const_iterator __i2, const value_type * __s,
                              size_type __n)
{
    return replace(static_cast<size_type>(__i1 - begin()), static_cast<size_type>(__i2 - __i1), __s,
                   __n);
}

template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::replace(const_iterator __i1, const_iterator __i2, const value_type * __s)
{
    return replace(static_cast<size_type>(__i1 - begin()), static_cast<size_type>(__i2 - __i1), __s);
}

template <class _CharT>
inline
basic_string<_CharT> &
basic_string<_CharT>::replace(const_iterator __i1, const_iterator __i2, size_type __n,
                              value_type __c)
{
    return replace(static_cast<size_type>(__i1 - begin()), static_cast<size_type>(__i2 - __i1), __n,
                   __c);
}

// erase

template <class _CharT>
basic_string<_CharT> &
basic_string<_CharT>::erase(size_type __pos, size_type __n)
{
    size_type __sz = size();
    assert(__pos <= __sz);
    if (__n)
    {
        value_type * __p = __get_pointer();
        __n = min(__n, __sz - __pos);
        size_type __n_move = __sz - __pos - __n;
        if (__n_move != 0)
        {
            __vt_move(__p + __pos, __p + __pos + __n, __n_move);
        }
        __sz -= __n;
        __set_size(__sz);
        __vt_assign(__p[__sz], value_type());
    }
    return *this;
}

template <class _CharT>
inline
typename basic_string<_CharT>::iterator
basic_string<_CharT>::erase(const_iterator __pos)
{
    _LIBCPP_ASSERT(__pos != end(),
                   "string::erase(iterator) called with a non-dereferenceable iterator");
    iterator __b = begin();
    size_type __r = static_cast<size_type>(__pos - __b);
    erase(__r, 1);
    return __b + static_cast<difference_type>(__r);
}

template <class _CharT>
inline
typename basic_string<_CharT>::iterator
basic_string<_CharT>::erase(const_iterator __first, const_iterator __last)
{
    _LIBCPP_ASSERT(__first <= __last, "string::erase(first, last) called with invalid range");
    iterator __b = begin();
    size_type __r = static_cast<size_type>(__first - __b);
    erase(__r, static_cast<size_type>(__last - __first));
    return __b + static_cast<difference_type>(__r);
}

template <class _CharT>
inline
void
basic_string<_CharT>::pop_back()
{
    _LIBCPP_ASSERT(!empty(), "string::pop_back(): string is already empty");
    size_type __sz;
    if (__is_long())
    {
        __sz = __get_long_size() - 1;
        __set_long_size(__sz);
        __vt_assign(*(__get_long_pointer() + __sz), value_type());
    }
    else
    {
        __sz = __get_short_size() - 1;
        __set_short_size(__sz);
        __vt_assign(*(__get_short_pointer() + __sz), value_type());
    }
}

template <class _CharT>
inline void basic_string<_CharT>::clear()
{
    if (__is_long())
    {
        __vt_assign(*__get_long_pointer(), value_type());
        __set_long_size(0);
    }
    else
    {
        __vt_assign(*__get_short_pointer(), value_type());
        __set_short_size(0);
    }
}

template <class _CharT>
inline void basic_string<_CharT>::__erase_to_end(size_type __pos)
{
    if (__is_long())
    {
        __vt_assign(*(__get_long_pointer() + __pos), value_type());
        __set_long_size(__pos);
    }
    else
    {
        __vt_assign(*(__get_short_pointer() + __pos), value_type());
        __set_short_size(__pos);
    }
}

template <class _CharT>
inline void basic_string<_CharT>::resize(size_type __n, value_type __c)
{
    size_type __sz = size();
    if (__n > __sz)
    {
        append(__n - __sz, __c);
    }
    else
    {
        __erase_to_end(__n);
    }
}

template <class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::max_size() const
{
    size_type __m = npos - 1; // __alloc_traits::max_size(__alloc());
#if _LIBCPP_BIG_ENDIAN
    return (__m <= ~__long_mask ? __m : __m / 2) - __alignment;
#else
    return __m - __alignment;
#endif
}

template <class _CharT>
void
basic_string<_CharT>::reserve(size_type __res_arg)
{
    assert(__res_arg <= max_size());
    size_type __cap = capacity();
    size_type __sz = size();
    __res_arg = max(__res_arg, __sz);
    __res_arg = __recommend(__res_arg);
    if (__res_arg != __cap)
    {
        pointer __new_data, __p;
        bool __was_long, __now_long;
        if (__res_arg == __min_cap - 1)
        {
            __was_long = true;
            __now_long = false;
            __new_data = __get_short_pointer();
            __p = __get_long_pointer();
        }
        else
        {
            if (__res_arg > __cap)
            {
                __new_data = __vt_allocate(__res_arg + 1);
            }
            else
            {
                __new_data = __vt_allocate(__res_arg + 1);
                if (__new_data == nullptr)
                {
                    return;
                }
            }
            __now_long = true;
            __was_long = __is_long();
            __p = __get_pointer();
        }
        __vt_copy(__new_data, __p, size() + 1);
        if (__was_long)
        {
            __vt_deallocate(__p, __cap + 1);
        }
        if (__now_long)
        {
            __set_long_cap(__res_arg + 1);
            __set_long_size(__sz);
            __set_long_pointer(__new_data);
        }
        else
        {
            __set_short_size(__sz);
        }
    }
}

template <class _CharT>
inline
typename basic_string<_CharT>::const_reference
basic_string<_CharT>::operator[](size_type __pos) const
{
    _LIBCPP_ASSERT(__pos <= size(), "string index out of bounds");
    return *(data() + __pos);
}

template <class _CharT>
inline
typename basic_string<_CharT>::reference
basic_string<_CharT>::operator[](size_type __pos)
{
    _LIBCPP_ASSERT(__pos <= size(), "string index out of bounds");
    return *(__get_pointer() + __pos);
}

template <class _CharT>
typename basic_string<_CharT>::const_reference
basic_string<_CharT>::at(size_type __n) const
{
    assert(__n < size());
    return (*this)[__n];
}

template <class _CharT>
typename basic_string<_CharT>::reference
basic_string<_CharT>::at(size_type __n)
{
    assert(__n < size());
    return (*this)[__n];
}

template <class _CharT>
inline
typename basic_string<_CharT>::reference
basic_string<_CharT>::front()
{
    _LIBCPP_ASSERT(!empty(), "string::front(): string is empty");
    return *__get_pointer();
}

template <class _CharT>
inline
typename basic_string<_CharT>::const_reference
basic_string<_CharT>::front() const
{
    _LIBCPP_ASSERT(!empty(), "string::front(): string is empty");
    return *data();
}

template <class _CharT>
inline
typename basic_string<_CharT>::reference
basic_string<_CharT>::back()
{
    _LIBCPP_ASSERT(!empty(), "string::back(): string is empty");
    return *(__get_pointer() + size() - 1);
}

template <class _CharT>
inline
typename basic_string<_CharT>::const_reference
basic_string<_CharT>::back() const
{
    _LIBCPP_ASSERT(!empty(), "string::back(): string is empty");
    return *(data() + size() - 1);
}

template <class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::copy(value_type * __s, size_type __n, size_type __pos) const
{
    size_type __sz = size();
    assert(__pos <= __sz);
    size_type __rlen = min(__n, __sz - __pos);
    __vt_copy(__s, data() + __pos, __rlen);
    return __rlen;
}

template <class _CharT>
inline
basic_string<_CharT>
basic_string<_CharT>::substr(size_type __pos, size_type __n) const
{
    return basic_string(*this, __pos, __n);
}

template <class _CharT>
inline void basic_string<_CharT>::swap(basic_string & __str)
{
    ::yl::swap(__r_, __str.__r_);
}

// find

template<typename _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find(const value_type * __s, size_type __pos, size_type __n) const
{
    if (__s == nullptr || __pos >= size())
    {
        return npos;
    }
    if (__pos == npos)
    {
        __pos = 0;
    }
    size_type __len = string_length(__s);
    for (size_type i = 0; i + __len + __pos <= size(); ++i)
    {
        const value_type * there = c_str() + __pos + i;
        const value_type * other = __s;
        size_type cmpcount = 0;
        bool is_equal = true;
        while (cmpcount < __n)
        {
            if (*other == *there)
            {
                ++there;
                ++other;
                ++cmpcount;
            }
            else
            {
                is_equal = false;
                break;
            }
        }
        if (is_equal)
        {
            return __pos + i;
        }
    }
    return npos;
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find(const basic_string & __str,
                           size_type __pos) const
{
    return find(__str.data(), __pos, __str.size());
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find(const value_type * __s,
                           size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::find(): received nullptr");
    return find(__s, __pos, string_length(__s));
}

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find(value_type __c,
                           size_type __pos) const
{
    value_type __t[2] = { __c, value_type() };
    return find(__t, __pos, 1);
}

// rfind

template<typename _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(const value_type * __s,
                            size_type __pos,
                            size_type __n) const
{
    if (__s == nullptr || size() == 0)
    {
        return npos;
    }
    if (__pos == npos)
    {
        __pos = size();
    }
    for (size_type i = 0; i <= size(); ++i)
    {
        if (__pos - i > size())
        {
            continue;
        }
        const value_type * there = c_str() + __pos - i;
        const value_type * other = __s;
        size_type cmpcount = 0;
        bool is_equal = true;
        while (cmpcount < __n)
        {
            if (*other == *there)
            {
                ++there;
                ++other;
                ++cmpcount;
            }
            else
            {
                is_equal = false;
                break;
            }
        }
        if (is_equal)
        {
            return __pos - i;
        }
    }
    return npos;
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(const basic_string & __str,
                            size_type __pos) const
{
    return rfind(__str.data(), __pos, __str.size());
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(const value_type * __s,
                            size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::rfind(): received nullptr");
    return rfind(__s, __pos, string_length(__s));
}

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(value_type __c,
                            size_type __pos) const
{
    value_type __t[2] = { __c, value_type() };
    return rfind(__t, __pos, 1);
}

// find_of

template<typename _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_of(const value_type * __s, size_type __pos, size_type __n,
                              int __incl_value, bool __found_char) const
{
    if (__s == nullptr)
    {
        return npos;
    }
    if (__pos == npos)
    {
        __pos = __incl_value > 0 ? 0 : size() - 1;
    }
    for (size_type cmpcount = 0; cmpcount < size() && __pos < size(); ++cmpcount)
    {
        if ((find_char_in_str(at(__pos), __s, __n) != npos) == __found_char)
        {
            return __pos;
        }
        __pos += __incl_value;
    }
    return npos;
}

// find_first_of

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_of(const value_type * __s,
                                    size_type __pos,
                                    size_type __n) const
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::find_first_of(): received nullptr");
    return find_of(__s, __pos, __n, 1, true);
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_of(const basic_string & __str,
                                    size_type __pos) const
{
    return find_first_of(__str.data(), __pos, __str.size());
}


template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_of(const value_type * __s,
                                    size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::find_first_of(): received nullptr");
    return find_first_of(__s, __pos, string_length(__s));
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_of(value_type __c,
                                    size_type __pos) const
{
    return find(__c, __pos);
}

// find_last_of

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_of(const value_type * __s,
                                   size_type __pos,
                                   size_type __n) const
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::find_last_of(): received nullptr");
    return find_of(__s, __pos, __n, -1, true);
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_of(const basic_string & __str,
                                   size_type __pos) const
{
    return find_last_of(__str.data(), __pos, __str.size());
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_of(const value_type * __s,
                                   size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::find_last_of(): received nullptr");
    return find_last_of(__s, __pos, string_length(__s));
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_of(value_type __c,
                                   size_type __pos) const
{
    return rfind(__c, __pos);
}

// find_first_not_of

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(const value_type * __s,
                                        size_type __pos,
                                        size_type __n) const
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::find_first_not_of(): received nullptr");
    return find_of(__s, __pos, __n, 1, false);
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(const basic_string & __str,
                                        size_type __pos) const
{
    return find_first_not_of(__str.data(), __pos, __str.size());
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(const value_type * __s,
                                        size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::find_first_not_of(): received nullptr");
    return find_first_not_of(__s, __pos, string_length(__s));
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(value_type __c,
                                        size_type __pos) const
{
    value_type __t[2] = { __c, value_type() };
    return find_first_not_of(__t, __pos, 1);
}

// find_last_not_of

template<class _CharT>
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_not_of(const value_type * __s,
                                       size_type __pos,
                                       size_type __n) const
{
    _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string::find_last_not_of(): received nullptr");
    return find_of(__s, __pos, __n, -1, false);
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_not_of(const basic_string & __str,
                                       size_type __pos) const
{
    return find_last_not_of(__str.data(), __pos, __str.size());
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_not_of(const value_type * __s,
                                       size_type __pos) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::find_last_not_of(): received nullptr");
    return find_last_not_of(__s, __pos, string_length(__s));
}

template<class _CharT>
inline
typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_not_of(value_type __c,
                                       size_type __pos) const
{
    value_type __t[2] = { __c, value_type() };
    return find_last_not_of(__t, __pos, 1);
}

// compare

template <class _CharT>
inline
int
basic_string<_CharT>::compare(const basic_string & __str) const
{
    size_t __lhs_sz = size();
    size_t __rhs_sz = __str.size();
    int __result = __vt_compare(data(), __str.data(),
                                min(__lhs_sz, __rhs_sz));
    if (__result != 0)
    {
        return __result;
    }
    if (__lhs_sz < __rhs_sz)
    {
        return -1;
    }
    if (__lhs_sz > __rhs_sz)
    {
        return 1;
    }
    return 0;
}

template <class _CharT>
int
basic_string<_CharT>::compare(size_type __pos1,
                              size_type __n1,
                              const value_type * __s,
                              size_type __n2) const
{
    _LIBCPP_ASSERT(__n2 == 0 || __s != nullptr, "string::compare(): received nullptr");
    size_type __sz = size();
    assert(__pos1 <= __sz && __n2 != npos);
    size_type __rlen = min(__n1, __sz - __pos1);
    int __r = __vt_compare(data() + __pos1, __s, min(__rlen, __n2));
    if (__r == 0)
    {
        if (__rlen < __n2)
        {
            __r = -1;
        }
        else if (__rlen > __n2)
        {
            __r = 1;
        }
    }
    return __r;
}

template <class _CharT>
inline
int
basic_string<_CharT>::compare(size_type __pos1,
                              size_type __n1,
                              const basic_string & __str) const
{
    return compare(__pos1, __n1, __str.data(), __str.size());
}

template <class _CharT>
int
basic_string<_CharT>::compare(size_type __pos1,
                              size_type __n1,
                              const basic_string & __str,
                              size_type __pos2,
                              size_type __n2) const
{
    return compare(__pos1, __n1, __self_view(__str), __pos2, __n2);
}

template <class _CharT>
int
basic_string<_CharT>::compare(const value_type * __s) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::compare(): received nullptr");
    return compare(0, npos, __s, string_length(__s));
}

template <class _CharT>
int
basic_string<_CharT>::compare(size_type __pos1,
                              size_type __n1,
                              const value_type * __s) const
{
    _LIBCPP_ASSERT(__s != nullptr, "string::compare(): received nullptr");
    return compare(__pos1, __n1, __s, string_length(__s));
}

// operator==

template<class _CharT>
inline bool operator==(const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return __lhs.compare(__rhs) == 0;
}

template<class _CharT>
inline bool operator==(const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    typedef basic_string<_CharT> _String;
    _LIBCPP_ASSERT(__lhs != nullptr, "operator==(char*, basic_string): received nullptr");
    size_t __lhs_len = string_length(__lhs);
    if (__lhs_len != __rhs.size())
    {
        return false;
    }
    return __rhs.compare(0, _String::npos, __lhs, __lhs_len) == 0;
}

template<class _CharT>
inline bool operator==(const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    typedef basic_string<_CharT> _String;
    _LIBCPP_ASSERT(__rhs != nullptr, "operator==(basic_string, char*): received nullptr");
    size_t __rhs_len = string_length(__rhs);
    if (__rhs_len != __lhs.size())
    {
        return false;
    }
    return __lhs.compare(0, _String::npos, __rhs, __rhs_len) == 0;
}

template<class _CharT>
inline bool operator!=(const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__lhs == __rhs);
}

template<class _CharT>
inline bool operator!=(const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__lhs == __rhs);
}

template<class _CharT>
inline bool operator!=(const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    return !(__lhs == __rhs);
}

// operator<

template<class _CharT>
inline bool operator< (const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT>
inline bool operator< (const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT>
inline bool operator< (const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    return __rhs.compare(__lhs) > 0;
}

// operator>

template<class _CharT>
inline bool operator> (const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return __rhs < __lhs;
}

template<class _CharT>
inline bool operator> (const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    return __rhs < __lhs;
}

template<class _CharT>
inline bool operator> (const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    return __rhs < __lhs;
}

// operator<=

template<class _CharT>
inline bool operator<=(const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__rhs < __lhs);
}

template<class _CharT>
inline bool operator<=(const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    return !(__rhs < __lhs);
}

template<class _CharT>
inline bool operator<=(const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__rhs < __lhs);
}

// operator>=

template<class _CharT>
inline bool operator>=(const basic_string<_CharT> & __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__lhs < __rhs);
}

template<class _CharT>
inline bool operator>=(const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    return !(__lhs < __rhs);
}

template<class _CharT>
inline bool operator>=(const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    return !(__lhs < __rhs);
}

// operator +

#if 0
template<class _CharT>
basic_string<_CharT>
operator+(const basic_string<_CharT> & __lhs,
          const basic_string<_CharT> & __rhs)
{
    basic_string<_CharT> __r;
    typename basic_string<_CharT>::size_type __lhs_sz = __lhs.size();
    typename basic_string<_CharT>::size_type __rhs_sz = __rhs.size();
    __r.__init(__lhs.data(), __lhs_sz, __lhs_sz + __rhs_sz);
    __r.append(__rhs.data(), __rhs_sz);
    return __r;
}

template<class _CharT>
basic_string<_CharT>
operator+(const _CharT * __lhs, const basic_string<_CharT> & __rhs)
{
    basic_string<_CharT> __r;
    typename basic_string<_CharT>::size_type __lhs_sz = string_length(__lhs);
    typename basic_string<_CharT>::size_type __rhs_sz = __rhs.size();
    __r.__init(__lhs, __lhs_sz, __lhs_sz + __rhs_sz);
    __r.append(__rhs.data(), __rhs_sz);
    return __r;
}

template<class _CharT>
basic_string<_CharT>
operator+(_CharT __lhs, const basic_string<_CharT> & __rhs)
{
    basic_string<_CharT> __r;
    typename basic_string<_CharT>::size_type __rhs_sz = __rhs.size();
    __r.__init(&__lhs, 1, 1 + __rhs_sz);
    __r.append(__rhs.data(), __rhs_sz);
    return __r;
}

template<class _CharT>
basic_string<_CharT>
operator+(const basic_string<_CharT> & __lhs, const _CharT * __rhs)
{
    basic_string<_CharT> __r;
    typename basic_string<_CharT>::size_type __lhs_sz = __lhs.size();
    typename basic_string<_CharT>::size_type __rhs_sz = string_length(__rhs);
    __r.__init(__lhs.data(), __lhs_sz, __lhs_sz + __rhs_sz);
    __r.append(__rhs, __rhs_sz);
    return __r;
}

template<class _CharT>
basic_string<_CharT>
operator+(const basic_string<_CharT> & __lhs, _CharT __rhs)
{
    basic_string<_CharT> __r;
    typename basic_string<_CharT>::size_type __lhs_sz = __lhs.size();
    __r.__init(__lhs.data(), __lhs_sz, __lhs_sz + 1);
    __r.push_back(__rhs);
    return __r;
}
#else
template<typename _CharT>
basic_string<_CharT>
inline operator+ (const basic_string<_CharT> & lhs, const basic_string<_CharT> & rhs)
{
    basic_string<_CharT> temp(lhs);
    return temp.append(rhs);
}

template<typename _CharT>
basic_string<_CharT>
inline operator+ (const _CharT * lhs, const basic_string<_CharT> & rhs)
{
    return basic_string<_CharT>(lhs) + rhs;
}

template<typename _CharT>
basic_string<_CharT>
inline operator+ (const basic_string<_CharT> & lhs, const _CharT * rhs)
{
    return lhs + basic_string<_CharT>(rhs);
}
#endif // 0

// swap
template<class _CharT>
inline void swap(basic_string<_CharT> & __lhs, basic_string<_CharT> & __rhs)
{
    __lhs.swap(__rhs);
}

template<class _CharT>
const typename basic_string<_CharT>::size_type
basic_string<_CharT>::npos;

//to_lower specialization

template<typename _CharT>
basic_string<_CharT> & basic_string<_CharT>::to_lower()
{
    assert(false);
    return *this;
}

template<>
inline basic_string<char> & basic_string<char>::to_lower()
{
    yl::transform(begin(), end(), begin(), ::tolower);
    return *this;
}

template<>
inline basic_string<wchar_t> & basic_string<wchar_t>::to_lower()
{
    yl::transform(begin(), end(), begin(), ::towlower);
    return *this;
}

//to_upper specialization
template<typename _CharT>
basic_string<_CharT> & basic_string<_CharT>::to_upper()
{
    assert(false);
    return *this;
}

template<>
inline basic_string<char> & basic_string<char>::to_upper()
{
    yl::transform(begin(), end(), begin(), ::toupper);
    return *this;
}

template<>
inline basic_string<wchar_t> & basic_string<wchar_t>::to_upper()
{
    yl::transform(begin(), end(), begin(), ::towupper);
    return *this;
}
}

#endif // BASE_YL_STRING

#endif  // STRING_LIKE_H

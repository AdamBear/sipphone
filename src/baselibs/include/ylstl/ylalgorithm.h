#ifndef __YLSTL_YLALGORITHM_H__
#define __YLSTL_YLALGORITHM_H__

#include "stable_sort.hpp"

namespace yl
{
#ifndef min
template<typename Tx>
const Tx & min(const Tx & lhs, const Tx & rhs)
{
    return lhs < rhs ? lhs : rhs;
}
#endif

#ifndef max
template<typename Tx>
const Tx & max(const Tx & lhs, const Tx & rhs)
{
    return lhs > rhs ? lhs : rhs;
}
#endif

//transform
template<typename InputIt, typename OutputIt, typename UnaryOperation>
OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first,
                   UnaryOperation unary_op)
{
    while (first1 != last1)
    {
        *d_first++ = unary_op(*first1++);
    }
    return d_first;
}

template<typename InputIt1, typename InputIt2, typename OutputIt, typename BinaryOperation>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                   OutputIt d_first, BinaryOperation binary_op)
{
    while (first1 != last1)
    {
        *d_first++ = binary_op(*first1++, *first2++);
    }
    return d_first;
}

//for each
//reference http://en.cppreference.com/w/cpp/algorithm/for_each
template<typename InputIt, typename UnaryFunction>
UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f)
{
    for (; first != last; ++first)
    {
        f(*first);
    }
    return f;
}

template<class InputIt, class T>
InputIt find(InputIt first, InputIt last, const T & value)
{
    for (; first != last; ++first)
    {
        if (*first == value)
        {
            return first;
        }
    }
    return last;
}

template<class ForwardIt>
ForwardIt unique(ForwardIt first, ForwardIt last)
{
    if (first == last)
    {
        return last;
    }

    ForwardIt result = first;
    while (++first != last)
    {
        if (!(*result == *first))
        {
            *(++result) = *first;
        }
    }
    return ++result;
}

template<class ForwardIt, class BinaryPredicate>
ForwardIt unique(ForwardIt first, ForwardIt last,
                 BinaryPredicate p)
{
    if (first == last)
    {
        return last;
    }

    ForwardIt result = first;
    while (++first != last)
    {
        if (!p(*result, *first))
        {
            *(++result) = *first;
        }
    }
    return ++result;
}
}

#endif


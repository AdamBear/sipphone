#ifndef __YLSTL_STATBLE_SORT_HPP__
#define __YLSTL_STATBLE_SORT_HPP__

#include "utility.hpp"

namespace yl
{

//detail
template<typename Iter, typename Comp, typename Dist, typename Type>
void _stable_sort(Iter begin, Iter end, Comp op, Dist dist, Type &)
{
    //SYZ_TRACE("Dist [%d]", dist);
    if (begin == end ||
            dist <= 1)
    {
        return;
    }
    else if (dist == 2)
    {
        if (op(*(begin + 1), *begin))
        {
            ::yl::swap(*begin, *(begin + 1));
        }
    }
    else
    {
        Iter mid = begin + dist / 2;
        //sub sort
        _stable_sort(begin, mid, op, mid - begin, *begin);
        _stable_sort(mid, end, op, end - mid, *mid);

        //如果已经有序，则不继续排序
        if (!op(*mid, *(mid - 1)))
        {
            return;
        }

        //merge
        _TempBuf<Type, Dist> buf(dist);
        typename _TempBuf<Type, Dist>::iterator pos = buf.begin();
        Iter first = begin;
        Iter second = mid;
        while (first != mid
                && second != end)
        {
            if (op(*second, *first))
            {
                *pos++ = *second++;
            }
            else
            {
                *pos++ = *first++;
            }
        }

        while (first != mid)
        {
            *pos++ = *first++;
        }
        while (second != end)
        {
            *pos++ = *second++;
        }

        //拷贝回原来的空间
        pos = buf.begin();
        first = begin;
        while (first != end)
        {
            *first++ = *pos++;
        }
    }
}

template<typename Iter, typename Dist, typename Type>
void _stable_sort(Iter begin, Iter end, Dist dist, Type &)
{
    if (begin == end)
    {
        return;
    }
    _stable_sort(begin, end, yl::less<Type>(), end - begin, *begin);
}

//interface
template<typename Iter>
void stable_sort(Iter begin, Iter end)
{
    if (begin == end)
    {
        return;
    }
    _stable_sort(begin, end, end - begin, *begin);
}

template<typename Iter, typename Comp>
void stable_sort(Iter begin, Iter end, Comp op)
{
    if (begin == end)
    {
        return;
    }
    _stable_sort(begin, end, op, end - begin, *begin);
}




}//namespace yl
#endif //__YLSTL_STATBLE_SORT_HPP__


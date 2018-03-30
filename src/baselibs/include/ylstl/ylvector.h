#ifndef _YEALINK_VECTOR_HEADER_
#define _YEALINK_VECTOR_HEADER_

//comment this to use std::vector
#define BASE_YL_VECTOR

#ifndef BASE_YL_VECTOR
#include <vector>
#include <string.h>//#include <cstring>
#include <algorithm>

template <typename T>
class YLVector
    : public std::vector<T>
{
public:
    typedef std::vector<T> detail_containtor;

    YLVector()
        : detail_containtor()
    {
    }
    explicit YLVector(unsigned int reserveSize)
        : detail_containtor(reserveSize)
    {
    }

    YLVector(const YLVector<T> & rhs)
        : detail_containtor(rhs)
    {
    }
    ~YLVector()
    {
    }

    inline int GetSize() const
    {
        return this->size();
    }
    template<typename _TY>
    //在指定位置插入
    void insertAt(int index, const _TY & value)
    {
        this->insert(this->begin() + index, value);
    }

    void removeAt(int index)
    {
        this->erase(this->begin() + index);
    }

    void set(size_t index, const T & value)
    {
        if (this->size() <= index)
        {
            this->resize(index + 1);
        }
        this->at(index) = value;
    }
};

template <class _ty, class _ptr>
void Sort(YLVector<_ty> & ylVector, _ptr fun, int nBegin = -1, int nEnd = -1)
{
    if (nBegin == -1)
    {
        nBegin = 0;
    }

    if (nEnd == -1)
    {
        nEnd = ylVector.size() - 1;
    }

    if (nBegin >= nEnd)
    {
        return;
    }

    int nCurrentPos = (nBegin + nEnd) / 2;
    _ty key = ylVector[nCurrentPos];

    for (size_t i = nBegin, j = nEnd; i < j;)
    {
        while (i < nCurrentPos && fun(ylVector[i], key))
        {
            i++;
        }

        if (i < nCurrentPos)
        {
            std::swap(ylVector[nCurrentPos], ylVector[i]);//ylVector.swap(nCurrentPos, i);
            nCurrentPos = i;
        }

        while (j > 0 && !(j < nCurrentPos || fun(ylVector[j], key)))
        {
            j--;
        }

        if (nCurrentPos < j)
        {
            std::swap(ylVector[nCurrentPos], ylVector[j]);//ylVector.swap(nCurrentPos, j);
            nCurrentPos = j;
        }

    }

    if (nCurrentPos - nBegin > 1)
    {
        Sort(ylVector, fun, nBegin, nCurrentPos - 1);
    }

    if (nEnd - nCurrentPos > 1)
    {
        Sort(ylVector, fun, nCurrentPos + 1, nEnd);
    }
}

template<typename Type>
inline void ExpandYLVector(YLVector<Type> & vec, int size)
{
    if (size > vec.size())
    {
        vec.resize(size);
    }
}

#else

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#define  DEFAULT_SIZE 0
#include "ylrandomiterator.h"

template <typename T>
class YLVector
{
    struct CVectorAssoc
    {
        T value;
        CVectorAssoc()
        {
        }
    };
protected:
    CVectorAssoc ** pDynamicPtr;
    //CVectorAssoc *;
    int  m_currentElementCount;
    int m_totalElementCount;
public:
    typedef random_iterator<T, YLVector<T> > iterator;
    typedef const_random_iterator<T, YLVector<T> > const_iterator;
public:
    YLVector()
    {
        m_currentElementCount = 0;
        pDynamicPtr = NULL;
        m_totalElementCount = DEFAULT_SIZE;
        Allocate(m_totalElementCount);
    }
    explicit YLVector(unsigned int reserveSize)
    {
        pDynamicPtr = NULL;
        m_currentElementCount = 0;
        m_totalElementCount = reserveSize;
        Allocate(m_totalElementCount);
    }

    YLVector(const YLVector<T> & rhs)
    {
        if (this != &rhs)
        {
            pDynamicPtr = NULL;
            m_totalElementCount = rhs.m_totalElementCount;
            m_currentElementCount = rhs.m_currentElementCount;
            Allocate(m_totalElementCount);
            Assign(rhs.pDynamicPtr, rhs.m_currentElementCount);
        }
    }
    ~YLVector()
    {
        clear();
    }
    void clear()
    {
        if (pDynamicPtr)
        {
            for (int i = 0; i < m_currentElementCount; i ++)
            {
                delete pDynamicPtr[i];
                pDynamicPtr[i] = NULL;
            }
            delete [] pDynamicPtr;
            pDynamicPtr = NULL;
            m_currentElementCount = 0;
        }
        m_totalElementCount = DEFAULT_SIZE;
    }
    template<typename _TY>
    void push_back(const _TY & value)
    {

        ReAllocate();
        if (pDynamicPtr)
        {
            CVectorAssoc * pAssoc = new CVectorAssoc();
            if (pAssoc)
            {
                pAssoc->value = value;
                pDynamicPtr[m_currentElementCount] = pAssoc;
                m_currentElementCount++;
            }
        }
    }
    int size() const
    {
        return m_currentElementCount;
    }
    inline int GetSize() const
    {
        return size();
    }
    template<typename _TY>
    //在指定位置插入
    void insertAt(int index, const _TY & value)
    {
        if (index < 0 || index >= m_currentElementCount)//超过范围
        {
            return;
        }
        //如果超过重新分配
        ReAllocate();
        int iPos = index;
        CVectorAssoc * pAssoc = new CVectorAssoc();
        if (pAssoc)
        {
            pAssoc->value = value;
            m_currentElementCount++;
        }
        while (iPos < m_totalElementCount)
        {
            CVectorAssoc * pTemp = pDynamicPtr[iPos];
            pDynamicPtr[iPos] = pAssoc;
            pAssoc = pTemp;
            iPos++;
        }
    }
    void removeAt(int index)
    {
        if (index >= 0 && index < m_currentElementCount)
        {
            int moveSize = m_currentElementCount - index - 1;
            if (moveSize > 0)
            {

                char * szTemp = new char[moveSize * sizeof(CVectorAssoc *)];
                memset(szTemp, 0, moveSize);
                CVectorAssoc * pAssoc = pDynamicPtr[index];
                memcpy(szTemp, pDynamicPtr + index + 1, moveSize * sizeof(CVectorAssoc *));

                memcpy(pDynamicPtr +  index, szTemp, moveSize * sizeof(CVectorAssoc *));
                delete [] szTemp;
                delete pAssoc;
                pAssoc = NULL;
                szTemp = NULL;
            }
            else if (moveSize == 0)
            {
                CVectorAssoc * pAssoc = pDynamicPtr[index];
                delete pAssoc;
                pAssoc = NULL;
            }
            m_currentElementCount--;
        }
    }


    T & at(int index)
    {
        return (*this)[index];
    }
    T & operator[](int index)
    {
        assert(NULL != pDynamicPtr);
        return pDynamicPtr[index]->value;
        //if (index >= 0 && index < m_currentElementCount)
        //{
        //    if (pDynamicPtr[index])
        //    {
        //        return pDynamicPtr[index]->value;
        //    }
        //}
    }
    const T & at(int index) const
    {
        return (*this)[index];
    }
    const T & operator[](int index) const
    {
        assert(NULL != pDynamicPtr);
        return pDynamicPtr[index]->value;
        //if (index >= 0 && index < m_currentElementCount)
        //{
        //    if (pDynamicPtr[index])
        //    {
        //        return pDynamicPtr[index]->value;
        //    }
        //}
    }
    iterator begin()
    {
        return iterator(*this, 0);
    }
    const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }
    iterator end()
    {
        return iterator(*this, size());
    }
    const_iterator end() const
    {
        return const_iterator(*this, size());
    }

    YLVector & operator= (const YLVector & rhs)
    {
        if (this != &rhs)
        {
            clear();
            m_totalElementCount = rhs.m_totalElementCount;
            m_currentElementCount = rhs.m_currentElementCount;
            Allocate(m_totalElementCount);
            Assign(rhs.pDynamicPtr, rhs.m_currentElementCount);
        }
        return *this;
    }

    void swap(int left, int right)
    {
        if (left >= 0 && left < m_currentElementCount
                && right >= 0 && right < m_currentElementCount)
        {
            CVectorAssoc * pTemp;
            pTemp = pDynamicPtr[left];
            pDynamicPtr[left] = pDynamicPtr[right];
            pDynamicPtr[right] = pTemp;
        }
    }
private:
    void ReAllocate()//超过重新分配
    {
        if (pDynamicPtr == NULL)
        {
            if (m_totalElementCount == 0)
            {
                m_totalElementCount =  1;
            }
            Allocate(m_totalElementCount);
            return;
        }
        if (m_currentElementCount >= m_totalElementCount - 1)
        {
            if (m_totalElementCount == 0)
            {
                m_totalElementCount =  1;
            }
            if (m_totalElementCount == 1)
            {
                m_totalElementCount = 2;
            }
            else
            {
                m_totalElementCount += m_totalElementCount / 2;
            }
            CVectorAssoc ** ptr = pDynamicPtr;
            Allocate(m_totalElementCount);
            if (pDynamicPtr == 0)
            {
                pDynamicPtr = ptr;
                return;
            }
            else
            {
                Assign(ptr, m_currentElementCount, false);
                memset(ptr, 0, m_currentElementCount * sizeof(CVectorAssoc *));
                delete [] ptr;
            }
        }
    }
    void Assign(CVectorAssoc ** pSrcDynamicPtr, unsigned int uSize, bool bDeepCopy = true)
    {
        if (pDynamicPtr && pSrcDynamicPtr && uSize <= (unsigned int)m_currentElementCount)
        {
            if (bDeepCopy)//深拷贝
            {
                for (int i = 0; i < (int)uSize; i ++)
                {
                    CVectorAssoc * pAssoc = new CVectorAssoc();
                    if (pAssoc)
                    {
                        pAssoc ->value = pSrcDynamicPtr[i]->value;
                    }
                    pDynamicPtr[i] = pAssoc;
                }
            }
            else
            {
                memcpy(pDynamicPtr, pSrcDynamicPtr, uSize * sizeof(CVectorAssoc *));
            }
        }
    }
    void Allocate(unsigned int  uAllocationSize)
    {
        pDynamicPtr = new CVectorAssoc * [uAllocationSize];
        if (pDynamicPtr)
        {
            memset(pDynamicPtr, 0, uAllocationSize * sizeof(CVectorAssoc *));
        }
    }
};


//指针和整型类型的数组操作
template <>
class YLVector<int>
{
protected:
    int * pDynamicPtr;
    //CVectorAssoc *;
    unsigned int  m_currentElementCount;
    unsigned int m_totalElementCount;
public:
    typedef random_iterator<int, YLVector<int> > iterator;
    typedef const_random_iterator<int, YLVector<int> > const_iterator;
public:
    YLVector()
    {
        m_currentElementCount = 0;
        pDynamicPtr =  NULL;
        m_totalElementCount = DEFAULT_SIZE;
        Allocate(m_totalElementCount);
    }
    YLVector(unsigned int reserveSize)
    {
        pDynamicPtr =  NULL;
        m_currentElementCount = 0;
        m_totalElementCount = reserveSize;
        Allocate(m_totalElementCount);
    }

    YLVector(const YLVector<int> & rhs)
    {
        if (this != &rhs)
        {
            pDynamicPtr = NULL;
            m_totalElementCount = rhs.m_totalElementCount;
            m_currentElementCount = rhs.m_currentElementCount;
            Allocate(m_totalElementCount);
            Assign(rhs.pDynamicPtr, rhs.m_currentElementCount);
        }
    }
    ~YLVector()
    {

        clear();
    }
    void clear()
    {
        if (pDynamicPtr)
        {

            delete [] pDynamicPtr;
            pDynamicPtr = NULL;
        }
        m_currentElementCount = 0;
        m_totalElementCount = DEFAULT_SIZE;
    }
    void push_back(int value)
    {
        ReAllocate();
        if (pDynamicPtr)
        {
            pDynamicPtr[m_currentElementCount] = value;
            m_currentElementCount++;
        }
    }
    //在指定位置插入
    void insertAt(int index, int value)
    {
        if (index < 0 || index >= (int)m_currentElementCount)//超过范围
        {
            return;
        }
        //如果超过重新分配
        ReAllocate();
        int iPos = index;
        m_currentElementCount++;
        while (iPos < (int)m_totalElementCount)
        {
            int iTemp = pDynamicPtr[iPos];
            pDynamicPtr[iPos] = value;
            value = iTemp;
            iPos++;
        }

    }
    int size() const
    {
        return m_currentElementCount;
    }
    inline int GetSize() const
    {
        return size();
    }
    void removeAt(int index)
    {
        if (index >= 0 && index < (int)m_currentElementCount)
        {
            int moveSize = m_currentElementCount - index - 1;
            if (moveSize > 0)
            {
                int * szTemp = new int[moveSize];
                memset(szTemp, 0, moveSize);
                memcpy(szTemp, pDynamicPtr + index + 1, moveSize * sizeof(int));

                memcpy(pDynamicPtr + index, szTemp, moveSize * sizeof(int));
                delete [] szTemp;
                szTemp = NULL;
            }
            m_currentElementCount--;
        }
    }
    int & at(int index)
    {
        return (*this)[index];
    }
    int & operator[](int index)
    {
        if (index >= 0 && index < (int)m_currentElementCount)
        {
            return pDynamicPtr[index];
        }
        return pDynamicPtr[0];
    }
    const int & at(int index) const
    {
        return (*this)[index];
    }
    const int & operator[](int index) const
    {
        if (index >= 0 && index < (int)m_currentElementCount)
        {
            return pDynamicPtr[index];
        }
        return pDynamicPtr[0];
    }
    iterator begin()
    {
        return iterator(*this, 0);
    }
    const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }
    iterator end()
    {
        return iterator(*this, size());
    }
    const_iterator end() const
    {
        return const_iterator(*this, size());
    }

    YLVector & operator= (const YLVector & rhs)
    {
        if (this != &rhs)
        {
            if (m_totalElementCount <
                    rhs.m_totalElementCount) //已分配的空间小于待赋值才进行重新分配
            {
                delete [] pDynamicPtr;
                pDynamicPtr = NULL;
                m_totalElementCount = rhs.m_totalElementCount;
                Allocate(m_totalElementCount);
            }
            m_currentElementCount = rhs.m_currentElementCount;
            Assign(rhs.pDynamicPtr, rhs.m_currentElementCount);
        }
        return *this;
    }

    void swap(int left, int right)
    {
        if (left >= 0 && left < (int)m_currentElementCount
                && right >= 0 && right < (int)m_currentElementCount)
        {
            int nTemp;
            nTemp = pDynamicPtr[left];
            pDynamicPtr[left] = pDynamicPtr[right];
            pDynamicPtr[right] = nTemp;
        }
    }
private:
    void ReAllocate()//超过重新分配
    {
        if (pDynamicPtr == NULL)
        {
            if (m_totalElementCount == 0)
            {
                m_totalElementCount =  1;
            }
            Allocate(m_totalElementCount);
            return;
        }
        if (m_currentElementCount >= m_totalElementCount - 1)
        {
            if (m_totalElementCount == 0)
            {
                m_totalElementCount =  1;
            }
            if (m_totalElementCount == 1)
            {
                m_totalElementCount = 2;
            }
            else
            {
                m_totalElementCount += m_totalElementCount / 2;
            }
            int * ptr = pDynamicPtr;
            Allocate(m_totalElementCount);
            if (pDynamicPtr == 0)
            {
                pDynamicPtr = ptr;
                return;
            }
            else
            {
                Assign(ptr, m_currentElementCount);
                delete [] ptr;
            }
        }
    }
    void Assign(int * pSrcDynamicPtr, unsigned int uSize, bool bDeepCopy = true)
    {
        if (pDynamicPtr && pSrcDynamicPtr && uSize <= m_currentElementCount)
        {
            memcpy(pDynamicPtr, pSrcDynamicPtr, uSize * sizeof(int));
        }
    }
    void Allocate(unsigned int  uAllocationSize)
    {
        if (uAllocationSize > 0)
        {
            pDynamicPtr = new int [uAllocationSize];
        }
        if (pDynamicPtr)
        {
            memset(pDynamicPtr, 0, uAllocationSize * sizeof(int));
        }
    }
};


template <typename T>
class YLVector<T *>: public YLVector<intptr_t>
{
public:
    typedef random_iterator<T *, YLVector<T *> > iterator;
    typedef const_random_iterator<T *, YLVector<T *> > const_iterator;
public:
    void push_back(T  * value)
    {
        YLVector<intptr_t>::push_back((intptr_t)value);
    }

    T *& at(int index)
    {
        return (T *&)YLVector<intptr_t>::at(index);
    }
    T *& operator[](int index)
    {
        return (T *&)(YLVector<intptr_t>::operator[](index));
    }
    const T *& at(int index) const
    {
        return (T *&)YLVector<intptr_t>::at(index);
    }
    const T *& operator[](int index) const
    {
        return (T *&)(YLVector<intptr_t>::operator[](index));
    }

    iterator begin()
    {
        return iterator(*this, 0);
    }
    const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }
    iterator end()
    {
        return iterator(*this, size());
    }
    const_iterator end() const
    {
        return const_iterator(*this, size());
    }

    void insertAt(int index, T * value)
    {
        YLVector<intptr_t>::insertAt(index, (intptr_t)value);
    }
    YLVector(unsigned int reserveSize): YLVector<intptr_t>(reserveSize) {}
    YLVector(): YLVector<intptr_t>() {}
    YLVector(const YLVector<intptr_t> & rhs): YLVector<intptr_t>(rhs) {}
};

template <class _ty, class _ptr>
void Sort(YLVector<_ty> & ylVector, _ptr fun, int nBegin = -1, int nEnd = - 1)
{
    if (nBegin == -1)
    {
        nBegin = 0;
    }

    if (nEnd == -1)
    {
        nEnd = ylVector.size() - 1;
    }

    if (nBegin >= nEnd)
    {
        return;
    }

    int nCurrentPos = (nBegin + nEnd) / 2;
    _ty key = ylVector[nCurrentPos];

    for (size_t i = nBegin, j = nEnd; i < j;)
    {
        while (i < nCurrentPos && fun(ylVector[i], key))
        {
            i++;
        }

        if (i < nCurrentPos)
        {
            ylVector.swap(nCurrentPos, i);
            nCurrentPos = i;
        }

        while (j > 0 && !(j < nCurrentPos || fun(ylVector[j], key)))
        {
            j--;
        }

        if (nCurrentPos < j)
        {
            ylVector.swap(nCurrentPos, j);
            nCurrentPos = j;
        }

    }

    if (nCurrentPos - nBegin > 1)
    {
        Sort(ylVector, fun, nBegin, nCurrentPos - 1);
    }

    if (nEnd - nCurrentPos > 1)
    {
        Sort(ylVector, fun, nCurrentPos + 1, nEnd);
    }
}

template<typename Type>
inline void ExpandYLVector(YLVector<Type> & vec, int size)
{
    while (vec.size() < size)
    {
        vec.push_back(Type());
    }
}

#endif // BASE_YL_VECTOR

#endif


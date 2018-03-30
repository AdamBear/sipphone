#ifndef _YEALINK_HASHTABLE_HEADER_
#define _YEALINK_HASHTABLE_HEADER_
//comment this to use std::unordered_map / std::unordered_multimap
#define BASE_YL_HASHMAP

#ifndef BASE_YL_HASHMAP
#include <utility>
#include "ylstring.h"

#if WIN32
#include <unordered_map>
#include <unordered_set>
using namespace std;
#else
#if(__cplusplus == 201103L)
#include <unordered_map>
#include <unordered_set>
using namespace std;
#else
#include <tr1/unordered_map>
#include <tr1/unordered_set>
using namespace std::tr1;
#endif //(__cplusplus == 201103L)
#endif //WIN32


template <class _key>
struct INT_HASH
{
    unsigned int operator()(const _key & key) const
    {
        return ((unsigned int)(void *)(unsigned int)key) >> 4;
    }
};

struct STRING_HASH
{
    unsigned int operator()(const yl::string & strKey) const
    {
        const char * str = strKey.c_str();
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        while (*str)
        {
            hash = hash * seed + (*str++);
        }

        return (hash & 0x7FFFFFFF);
    }
};

template<> //特化string的INT_HASH
struct INT_HASH<yl::string> : public STRING_HASH
{
public:
    unsigned int operator()(const yl::string & strKey) const
    {
        return STRING_HASH::operator()(strKey);
    }
};

#if(__cplusplus == 201103L)
template<class KEY, class VALUE, class _Hash = INT_HASH<KEY> >
class YLHashMap : public unordered_map<KEY, VALUE, _Hash>
#else
template<class KEY, class VALUE, class _Hash = INT_HASH<KEY> >
class YLHashMap : public unordered_map<KEY, VALUE, _Hash>
#endif
{
public:
#if(__cplusplus == 201103L)
    typedef unordered_map<KEY, VALUE, _Hash> detail_map
#else
    typedef unordered_map<KEY, VALUE, _Hash> detail_map;
#endif

    typedef typename detail_map::size_type       size_type;

public:
    explicit
    YLHashMap(size_type __n = 10)
#ifndef _WIN32
        : detail_map(__n)
#endif // _WIN32
    {
#ifdef _WIN32
        //TODO. Fix Me
        _Mybase::rehash(__n);
#endif // _WIN32
    }

    template<typename _InputIterator>
    YLHashMap(_InputIterator __f, _InputIterator __l,
              size_type __n = 10)
        : detail_map(__f, __l, __n)
    { }

    void insert(KEY key, VALUE value)
    {
        put(key, value);
    }

    void insert(std::pair<KEY, VALUE> _pair)
    {
        put(_pair.first, _pair.second);
    }

    void put(KEY key, VALUE value)
    {
        (*this)[key] = value;
    }

};



#if(__cplusplus == 201103L)
template<class KEY, class VALUE, class _Hash = INT_HASH<KEY> >
class YLHashMultiMap : public unordered_multimap<KEY, VALUE, _Hash>
#else
template<class KEY, class VALUE, class _Hash = INT_HASH<KEY> >
class YLHashMultiMap : public unordered_multimap<KEY, VALUE, _Hash>
#endif
{
public:
#if(__cplusplus == 201103L)
    typedef unordered_multimap<KEY, VALUE, _Hash> detail_map;
#else
    typedef unordered_multimap<KEY, VALUE, _Hash> detail_map;
#endif

    typedef typename detail_map::size_type       size_type;

public:
    explicit
    YLHashMultiMap(size_type __n = 10)
#ifndef _WIN32
        : detail_map(__n)
#endif // _WIN32
    {
#ifdef _WIN32
        //TODO. Fix Me
        _Mybase::rehash(__n);
#endif // _WIN32
    }

    template<typename _InputIterator>
    YLHashMultiMap(_InputIterator __f, _InputIterator __l,
                   size_type __n = 10)
        : detail_map(__f, __l, __n)
    { }

    void insert(KEY key, VALUE value)
    {
        detail_map::insert(std::make_pair<KEY, VALUE>(key, value));
    }

    void insert(std::pair<KEY, VALUE> _pair)
    {
        detail_map::insert(_pair);
    }

    void put(KEY key, VALUE value)
    {
        detail_map::insert(std::make_pair<KEY, VALUE>(key, value));
    }
};

#else
//#include <stdlib.h>

#include <new>
#include <assert.h>

#include "ylstring.h"

#include <stdio.h>
#include <string.h>

#include "memoryblock.h"

#define HWINLINE inline
struct __ITERATOR { };
typedef __ITERATOR * ITERATOR;

#define before_start_pos ((ITERATOR)-1L)
#define SIZE_T_MAX  UINT_MAX    // 容器最大的容量

template<class TYPE, class ARG_TYPE>
inline bool container_compare(const TYPE * pElement1, const ARG_TYPE * pElement2)
{
    assert(NULL != pElement1);
    assert(NULL != pElement2);

    return *pElement1 == *pElement2;
}
template <class _key>
struct INT_HASH
{
    unsigned int operator()(const _key & key) const
    {
        return ((unsigned int)(void *)(unsigned int)key) >> 4;
    }
};

struct STRING_HASH
{
    unsigned int operator()(const yl::string & strKey) const
    {
        const char * str = strKey.c_str();
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        while (*str)
        {
            hash = hash * seed + (*str++);
        }

        return (hash & 0x7FFFFFFF);
    }
};

template<> //特化string的INT_HASH
struct INT_HASH<yl::string>: public STRING_HASH
{
public:
    unsigned int operator()(const yl::string & strKey) const
    {
        return STRING_HASH::operator()(strKey);
    }
};

template<class TYPE>
inline void container_construct(TYPE * pElements, int nCount)
{
    assert(nCount == 0 || NULL != pElements);

    // 内存区域初始化为0
    memset((void *)pElements, 0, nCount * sizeof(TYPE));

    // 调用所有的构造函数
    for (; nCount--; pElements++)
    {
        // 必须包含new.h
        ::new ((void *)pElements) TYPE;
    }
}

// 析构容器的所有成员
template<class TYPE>
inline void container_destruct(TYPE * pElements, int nCount)
{
    assert(nCount == 0 || NULL != pElements);

    // 调用析构函数
    for (; nCount--; pElements++)
    {
        pElements->~TYPE();
    }
}

#define assert_valid(x)     ((x)->assertValid())

// HASHMAP类
template<class KEY, class VALUE, class FUN = INT_HASH<KEY> >
class YLHashMap
{
private:
    // 使用拉地址链算法,当多个主键具有相同的HASH时,将其列入链表
    struct TAssoc
    {
        TAssoc * pNext;
        unsigned int nHashValue;
        KEY first;
        VALUE second;
        void append(TAssoc * pNode)
        {
            TAssoc * pTail = this;
            // 找到链表尾部或者下一个Key区域之前
            while (pTail->pNext && pTail->pNext->first == pTail->first)
            {
                pTail = pTail->pNext;
            }
            // 插入节点
            pNode->pNext = pTail->pNext;
            pTail->pNext = pNode;
        }
    };

    //迭代器
    struct _YLHashIterator
    {
        TAssoc * pNode; //数据节点
        YLHashMap * pRelatedMap; //关联的哈希表

        void SetNode(TAssoc * pObjNode)
        {

            pNode = pObjNode;
        }
        _YLHashIterator(TAssoc * pAssoc, YLHashMap * pHashmap)
        {
            SetNode(pAssoc);
            pRelatedMap = pHashmap;
        }
        _YLHashIterator()
        {
            SetNode(NULL);
            pRelatedMap = NULL;
        }
        ~_YLHashIterator()
        {

        }
        _YLHashIterator & operator++()
        {
            SetNode(pRelatedMap->getNext(pNode));
            return *this;
        }

        _YLHashIterator operator++(int)
        {
            // postincrement
            _YLHashIterator _Tmp = *this;
            ++*this;
            return (_Tmp);
        }

        // test for iterator inequality
        bool operator!=(const _YLHashIterator & _Right) const
        {
            return (pNode != _Right.pNode);
        }
        bool operator==(const _YLHashIterator & _Right) const
        {
            return (pNode == _Right.pNode);
        }

        TAssoc * operator->() const
        {
            // return pointer to class object
            return pNode;
        }
    };

public:

    typedef _YLHashIterator iterator;

    YLHashMap(bool bMultiValue = false, int nBlockSize = 10, int nHashTableSize = 100);

    YLHashMap(const YLHashMap & rhs);

    unsigned int size() const;

    VALUE & operator[](KEY key);

    bool empty() const
    {
        return size() == 0;
    }

    unsigned int  count(const KEY & key)
    {
        unsigned int hash = 0;
        TAssoc * pAssoc = getAssocAt(key, hash);
        unsigned int count = 0;
        while (pAssoc)
        {
            if (pAssoc ->first != key)
            {
                break;
            }
            count ++;
            pAssoc = pAssoc->pNext;
        }
        return count;
    }

    void clear();


    iterator begin()
    {
        TAssoc * pAssoc  = NULL;
        if (m_nCount > 0)
        {
            pAssoc = GetFirstValidAssoc();
        }
        iterator iter(pAssoc, this);
        return iter;
    }
    iterator find(const  KEY & _key)
    {
        unsigned int nHash;
        TAssoc * pAssoc = getAssocAt(_key, nHash);

        iterator _tmp(pAssoc, this);
        return _tmp;
    }
    iterator end()
    {
        iterator iter(NULL, this);
        return iter;
    }

    iterator erase(iterator & _where)
    {
        if (_where != end() && m_pHashTable != NULL)
        {
            TAssoc ** ppAssocPrev;

#ifdef ISO_CPLUSPLUS
            ppAssocPrev = &m_pHashTable[fun<ARG_KEY>(_where.pNode->first) % m_nHashTableSize];
#else
            ppAssocPrev = &m_pHashTable[fun(_where.pNode->first) % m_nHashTableSize];
#endif //ISO_CPLUSPLUS

            TAssoc * pAssoc;
            for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
            {
                if (pAssoc == _where.pNode)
                {
                    *ppAssocPrev = pAssoc->pNext;  // remove from list
                    iterator temp(getNext(pAssoc), this);
                    freeAssoc(pAssoc);

                    return temp;
                }
                ppAssocPrev = &pAssoc->pNext;
            }
        }
        return end();

    }
    iterator erase(const KEY & _key)
    {
        iterator iter =  find(_key);
        return erase(iter);
    }
    unsigned int capacity() const;

    void put(KEY _key, VALUE _value);
    void insert(KEY _key, VALUE _value)
    {
        put(_key, _value);
    }
#ifndef UNIT_TEST
protected:
#else
public:
#endif
    void init(unsigned int hashSize, bool bAllocNow = true);

    void  ResetByMap(const YLHashMap & hashMap);

    TAssoc * getNext(TAssoc * pAssocPrev)
    {
        TAssoc * pAssocNext = NULL;
        if (pAssocPrev !=  NULL)
        {
            if ((pAssocNext = pAssocPrev->pNext) == NULL)
            {
                for (unsigned int nBucket = pAssocPrev->nHashValue + 1;     nBucket < m_nHashTableSize; nBucket++)
                {
                    if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
                    {
                        break;
                    }
                }
            }
        }
        return  pAssocNext;
    }

    TAssoc * GetFirstValidAssoc()
    {
        TAssoc * pAssocRet = NULL;
        for (unsigned int nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
        {
            if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
            {
                break;
            }
        }
        return pAssocRet;
    }
#ifndef UNIT_TEST
protected:
#else
public:
#endif

    bool m_bMultyValue; //是否允许一个键对应多个值
    TAssoc    **    m_pHashTable;
    unsigned int    m_nHashTableSize;
    int             m_nCount;
    TAssoc     *    m_pFreeList;
    struct TMemPlex  * m_pBlocks;
    int             m_nBlockSize;

    FUN fun;
    //HashFun fun;

    TAssoc * newAssoc();
    void freeAssoc(TAssoc *);
    TAssoc * getAssocAt(const  KEY &, unsigned int &) const;


public:
    YLHashMap & operator= (const YLHashMap & rhs);
    ~YLHashMap();
    void assertValid() const;
};

template<class KEY, class VALUE, class HashFunc>
void YLHashMap<KEY, VALUE, HashFunc>::put(KEY _key, VALUE _value)
{
    if (m_bMultyValue)//多值时插入到后面
    {
        //
        unsigned int nHash;
        TAssoc * pAssoc = getAssocAt(_key, nHash);
        if (pAssoc == NULL)
        {
            (*this)[_key] = _value;
        }
        else
        {
            TAssoc * pAssocNew = newAssoc();
            pAssocNew->nHashValue = nHash;
            pAssocNew->first = _key;
            pAssocNew->second = _value;
            pAssoc->append(pAssocNew);
        }
    }
    else
    {
        (*this)[_key] = _value;
    }
}
template<class KEY, class VALUE, class HashFunc>
void YLHashMap<KEY, VALUE, HashFunc>::ResetByMap(const YLHashMap<KEY, VALUE, HashFunc> & hashMap)
{
    init(hashMap.m_nHashTableSize, true);
    if (hashMap.m_pHashTable && m_pHashTable)
    {
        for (int i  = 0; i < m_nHashTableSize; i ++)
        {
            TAssoc * pAssoc = hashMap.m_pHashTable[i];
            while (pAssoc)
            {
                (*this)[pAssoc->first] = pAssoc->second;
                pAssoc = pAssoc->pNext;
            }
        }
    }
}
// INLINE函数
template<class KEY, class VALUE, class HashFunc>
inline unsigned int  YLHashMap<KEY, VALUE, HashFunc>::size() const
{
    return m_nCount;
}

template<class KEY, class VALUE, class HashFunc>
inline unsigned int YLHashMap<KEY, VALUE, HashFunc>::capacity() const
{
    return m_nHashTableSize;
}
template<class KEY, class VALUE, class HashFunc>
YLHashMap<KEY, VALUE, HashFunc> & YLHashMap<KEY, VALUE, HashFunc>::operator=
(const YLHashMap<KEY, VALUE, HashFunc> & rhs)
{
    if (this != &rhs)
    {
        clear();
        init(rhs.m_nHashTableSize, true);
        ResetByMap(rhs);
    }
    return *this;
}

template<class KEY, class VALUE, class HashFunc>
YLHashMap<KEY, VALUE, HashFunc>::YLHashMap(const YLHashMap<KEY, VALUE, HashFunc>  & rhs)
{
    if (this != &rhs)
    {
        m_pHashTable = NULL;
        m_nHashTableSize = 10000;  //  默认HASH大小
        m_nCount = 0;
        m_pFreeList = NULL;
        m_pBlocks = NULL;
        m_nBlockSize = rhs.m_nBlockSize;

        m_bMultyValue = false;
        ResetByMap(rhs);
    }
}

// OUTLINE函数
template<class KEY, class VALUE, class HashFunc>
inline YLHashMap<KEY, VALUE, HashFunc>::YLHashMap(bool bMultiValue, int nBlockSize,
        int nHashTableSize)
{
    assert(nBlockSize > 0);

    m_pHashTable = NULL;
    m_nHashTableSize = nHashTableSize;  //  默认HASH大小
    m_nCount = 0;
    m_pFreeList = NULL;
    m_pBlocks = NULL;
    m_nBlockSize = nBlockSize;

    m_bMultyValue = bMultiValue;
}

template<class KEY, class VALUE, class HashFunc>
inline void YLHashMap<KEY, VALUE, HashFunc>::init(
    unsigned int nHashSize, bool bAllocNow)
{
    assert_valid(this);
    assert(m_nCount == 0);
    assert(nHashSize > 0);

    if (m_pHashTable != NULL)
    {
        delete[] m_pHashTable;
        m_pHashTable = NULL;
    }

    if (bAllocNow)
    {
        m_pHashTable = new TAssoc* [nHashSize];
        memset(m_pHashTable, 0, sizeof(TAssoc *) * nHashSize);
    }

    m_nHashTableSize = nHashSize;
}

template<class KEY, class VALUE, class HashFunc>
inline YLHashMap<KEY, VALUE, HashFunc>::~YLHashMap()
{
    //  removeAll();
    clear();
    //assert(m_nCount == 0);
}

template<class KEY, class VALUE, class HashFunc>
inline void YLHashMap<KEY,  VALUE, HashFunc>::clear()
{
    assert_valid(this);

    if (m_pHashTable != NULL)
    {
        // destroy elements (values and keys)
        for (unsigned int nHash = 0; nHash < m_nHashTableSize; nHash++)
        {
            TAssoc * pAssoc;
            for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
                    pAssoc = pAssoc->pNext)
            {
#ifdef ISO_CPLUSPLUS
                container_destruct<VALUE>(&pAssoc->second, 1);
                container_destruct<KEY>(&pAssoc->first, 1);
#else
                container_destruct(&pAssoc->second, 1);
                container_destruct(&pAssoc->first, 1);
#endif
            }
        }
    }

    delete[] m_pHashTable;
    m_pHashTable = NULL;

    m_nCount = 0;
    m_pFreeList = NULL;
    m_pBlocks->freeDataChain();
    m_pBlocks = NULL;
}

template<class KEY, class VALUE, class HashFunc>
inline typename YLHashMap<KEY, VALUE, HashFunc>::TAssoc *
YLHashMap<KEY, VALUE, HashFunc>::newAssoc()
{
    if (m_pFreeList == NULL)
    {
        TMemPlex * newBlock = TMemPlex::create(m_pBlocks, m_nBlockSize, sizeof(TAssoc));
        TAssoc * pAssoc = (TAssoc *) newBlock->data();

        pAssoc += m_nBlockSize - 1;
        for (int i = m_nBlockSize - 1; i >= 0; i--, pAssoc--)
        {
            pAssoc->pNext = m_pFreeList;
            m_pFreeList = pAssoc;
        }
    }
    assert(m_pFreeList != NULL);

    TAssoc * pAssoc = m_pFreeList;
    m_pFreeList = m_pFreeList->pNext;
    m_nCount++;

    assert(m_nCount > 0);

#ifdef ISO_CPLUSPLUS
    container_construct<KEY>(&pAssoc->first, 1);
    container_construct<VALUE>(&pAssoc->second, 1);
#else
    container_construct(&pAssoc->first, 1);
    container_construct(&pAssoc->second, 1);
#endif //ISO_CPLUSPLUS
    pAssoc->pNext = NULL;
    return pAssoc;
}

template<class KEY, class VALUE, class HashFunc>
inline void YLHashMap<KEY, VALUE, HashFunc>::freeAssoc(
    typename YLHashMap<KEY, VALUE, HashFunc>::TAssoc * pAssoc)
{
#ifdef ISO_CPLUSPLUS
    container_destruct<VALUE>(&pAssoc->second, 1);
    container_destruct<KEY>(&pAssoc->first, 1);
#else
    container_destruct(&pAssoc->second, 1);
    container_destruct(&pAssoc->first, 1);
#endif //ISO_CPLUSPLUS

    pAssoc->pNext = m_pFreeList;
    m_pFreeList = pAssoc;

    m_nCount--;
    assert(m_nCount >= 0);

    if (m_nCount == 0)
    {
        clear();
    }
}

template<class KEY, class VALUE, class HashFunc>
inline typename YLHashMap<KEY, VALUE, HashFunc>::TAssoc *
YLHashMap<KEY, VALUE, HashFunc>::getAssocAt(const KEY & key, unsigned int & nHash) const
{
#ifdef ISO_CPLUSPLUS
    nHash = fun<ARG_KEY>(key) % m_nHashTableSize;
#else
    nHash =  fun(key) % m_nHashTableSize;
#endif //ISO_CPLUSPLUS

    if (m_pHashTable == NULL)
    {
        return NULL;
    }

    int i = 0;
    TAssoc * pAssoc;
    for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        ++i;
        if (container_compare(&pAssoc->first, &key))
        {
            return pAssoc;
        }
    }
    return NULL;
}

template<class KEY, class VALUE, class HashFunc>
inline VALUE & YLHashMap<KEY, VALUE, HashFunc>::operator[](KEY key)
{
    assert_valid(this);

    unsigned int nHash;
    TAssoc * pAssoc = NULL;
    if ((pAssoc = getAssocAt(key, nHash)) == NULL)
    {
        if (m_pHashTable == NULL)
        {
            init(m_nHashTableSize);
        }

        // 如果该元素不存在,则将其添加到HASH中去
        pAssoc = newAssoc();
        pAssoc->nHashValue = nHash;
        pAssoc->first = key;

        pAssoc->pNext = m_pHashTable[nHash];
        m_pHashTable[nHash] = pAssoc;
    }
    return pAssoc->second;  // return new reference
}

template<class KEY, class VALUE, class HashFunc>
inline void YLHashMap<KEY, VALUE, HashFunc>::assertValid() const
{
    assert(m_nHashTableSize > 0);
    assert(m_nCount == 0 || m_pHashTable != NULL);
}

// HASHMULTIMAP类
template<class KEY, class VALUE, class FUN = INT_HASH<KEY> >
class YLHashMultiMap : public YLHashMap<KEY, VALUE, FUN>
{
public:
    typedef YLHashMap<KEY, VALUE, FUN> detail_map;

    explicit YLHashMultiMap(int nBlockSize = 10, int nHashTableSize = 100)
        : detail_map(true, nBlockSize, nHashTableSize) { }

    YLHashMultiMap(const YLHashMultiMap & rhs);
};

template<class KEY, class VALUE, class HashFunc>
YLHashMultiMap<KEY, VALUE, HashFunc>::YLHashMultiMap(const YLHashMultiMap<KEY, VALUE, HashFunc>  & rhs)
{
    if (this != &rhs)
    {
        this->m_pHashTable = NULL;
        this->m_nHashTableSize = 10000;  //  默认HASH大小
        this->m_nCount = 0;
        this->m_pFreeList = NULL;
        this->m_pBlocks = NULL;
        this->m_nBlockSize = rhs.m_nBlockSize;

        this->m_bMultyValue = true;
        ResetByMap(rhs);
    }
}

#endif // BASE_YL_HASHMAP

#endif

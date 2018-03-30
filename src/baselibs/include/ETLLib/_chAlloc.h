#ifndef __chALLOC_H__
#define __chALLOC_H__

#ifndef __ETL_H__
#error _chAlloc.h only can be include in ETL.h
#endif

namespace ETL_AllocHelper
{
class baseStaticPoolAlloc;
template <class typeObj>
inline void SwapObject(typeObj & a, typeObj & b)
{
    typeObj c;
    c = a;
    a = b;
    b = c;
}

template <class typeObj>
inline void SwapObjects(typeObj * pObjects, int nCount)
{
    chASSERT(pObjects != NULL && nCount > 0);

    typeObj * pBeginObject = pObjects;
    typeObj * pEndObject = pObjects + nCount - 1;
    // then call the swap(a, b)
    while (pBeginObject < pEndObject)
    {
        SwapObject(*(pBeginObject++), *(pEndObject--));
    }
}

template<class typeObj>
inline void SwapObjectArray(typeObj * pObj1, typeObj * pObj2, int nCount)
{
    chASSERT(pObj1 != NULL && pObj2 != NULL && nCount >= 1);
    if (pObj1 != pObj2)
    {
        typeObj tObj;
        while (nCount--)
        {
            tObj = *pObj1;
            *pObj1++ = *pObj2;
            *pObj2++ = tObj;
        }
    }
}

#define chConstructObject(type, pObject)        new((LPVOID)(pObject)) type
//#define chDestructObject(type, pObject)           (pObject)->~type()
#define chDestructObject(type, pObject)         DestructObject<type>(pObject)

// CONSTRUCT and DESTRUCT MUST NOT reset memory content for extra memset will cause
#define CONSTRUCT_OBJECTS(type, pData, nSize)           memset(pData, 0, sizeof(type) * (nSize))
#define DESTRUCT_OBJECTS(type, pData, nSize)            // NOTHING TO DO
#define MOVE_OBJECTS(type, pDest, pSrc, nCount)         memmove(pDest, pSrc, (nCount) * sizeof(type))
#define COPY_OBJECTS(type, pDest, pSrc, nCount)         memcpy(pDest, pSrc, (nCount) * sizeof(type))

template<class typeObj>
inline void ConstructObject(typeObj * pObjects)
{
    chASSERT(pObjects != NULL);
    new ((LPVOID)pObjects) typeObj;
}

template<class typeObj>
inline void DestructObject(typeObj * pObjects)
{
    chASSERT(pObjects != NULL);
    pObjects->~typeObj();
}

template<class typeObj>
inline void ConstructObjects(typeObj * pObjects, int nCount)
{
    chASSERT(pObjects != NULL && nCount > 0);
    if (sizeof(typeObj) <= sizeof(int))
    {
        memset(pObjects, 0, sizeof(typeObj) * nCount);
    }
    // then call the constructor(s)
    while (nCount--)
    {
        new ((LPVOID)(pObjects++)) typeObj;
    }
}

template<class typeObj>
inline void DestructObjects(typeObj * pObjects, int nCount)
{
    chASSERT(pObjects != NULL && nCount > 0);

    // call the destructor(s)
    while (nCount--)
    {
        (pObjects++)->~typeObj();
    }
}

template<class typeObj>
inline void CopyObjects(typeObj * pDest, const typeObj * pSrc, INT nCount)
{
    chASSERT(pDest != NULL && pSrc != NULL && nCount >= 0);

    // default is element-copy using assignment
    if (pSrc < pDest)
    {
        pDest += nCount;
        pSrc += nCount;
        while (nCount --)
        {
            *(--pDest) = *(--pSrc);
        }
    }
    else if (pSrc > pDest)
    {
        while (nCount--)
        {
            *pDest++ = *pSrc++;
        }
    }
}

template<class typeObj>
inline void MoveObjects(typeObj * pDest, typeObj * pSrc, int nCount)
{
    chASSERT(pDest != NULL && pSrc != NULL && nCount >= 0);
    if (pSrc < pDest)
    {
        pDest += nCount;
        pSrc += nCount;
        while (nCount > 0)
        {
            --pDest;
            --pSrc;
            chConstructObject(typeObj, pDest);
            *pDest = *pSrc;
            chDestructObject(typeObj, pSrc);
            nCount--;
        }
    }
    else if (pSrc > pDest)
    {
        while (nCount > 0)
        {
            chConstructObject(typeObj, pDest);
            *pDest++ = *pSrc;
            chDestructObject(typeObj, pSrc++);
            nCount--;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// class chBlock
template<class typeObj>
class chBlock
{
public:
    BOOL alloc(int nObjCount)
    {
        chASSERT(m_pObject == NULL && nObjCount >= 0);
        if (nObjCount > 0)
        {
            m_pObject = (typeObj *)::realloc(m_pObject, nObjCount * sizeof(typeObj));
            chASSERT(m_pObject != NULL);
            m_nCount = nObjCount;
        }
        return m_pObject != NULL;
    }
    void free()
    {
        chASSERT(m_pObject != NULL);
        if (m_pObject != NULL)
        {
            ::free(m_pObject);
            m_pObject = NULL;
            m_nCount = 0;
        }
    }
    void reset()
    {
        m_pObject = NULL;
        m_nCount = 0;
    }
    void memset(int nData)
    {
        if (m_pObject != NULL)
        {
            ::memset(m_pObject, nData, m_nCount);
        }
    }
    void copy_to(typeObj * pObject)
    {
        if (m_pObject != NULL)
        {
            ::memcpy(pObject, m_pObject, m_nCount * sizeof(typeObj));
        }
    }
    typeObj & operator [](const int iIndex) const
    {
        chASSERT(m_pObject != NULL && iIndex >= 0 && iIndex < m_nCount);
        return m_pObject[iIndex];
    }
    typeObj * get_block_buffer() const
    {
        return m_pObject;
    }
    int get_block_size() const
    {
        return m_nCount * sizeof(typeObj);
    }
    int get_object_count() const
    {
        return m_nCount;
    }
public:
    chBlock(typeObj * pObject, int nSize)
    {
        if (pObject != NULL && nSize != 0)
        {
            m_pObject = pObject;
            m_nCount = nSize;
        }
        else
        {
            m_pObject = NULL;
            m_nCount = 0;
        }
    }
    chBlock()
    {
        m_pObject = NULL;
        m_nCount = 0;
    }
private:
    typeObj  *  m_pObject;
    int         m_nCount;
};

typedef chBlock<BYTE>       chByteBlock;

//////////////////////////////////////////////////////////////////////////
// class baseDynamicNodeAlloc
// allloc some big pools for list, and one big pool contain many nodes
class baseDynamicNodeAlloc
{
    typedef pointer_chain_pNext             type_block;
    typedef list_chain_pointer              type_chain;
protected:
    void cleanup_nodes();
    LPVOID borrow_node(int nNodeSize, int nRowCount);
    void return_node(LPVOID pData);
private:
    type_chain * AllocHashBlock(int nNodeSize, int nRowCount);
protected:
    baseDynamicNodeAlloc();
    ~baseDynamicNodeAlloc();
protected:
    int                 m_nBorrowedCount;
    type_chain     *    m_pFirstFreeNode;
    type_block     *    m_pFirstFreeBlock;
};

//////////////////////////////////////////////////////////////////////////
// class chAlloc_Node_dynamic
// allloc some big pools for list, and one big pool contain many nodes
template <class typeNode, int rowSize>
class chAlloc_Node_dynamic : public baseDynamicNodeAlloc
{
    typedef typeNode                                type_node;
public:
    typedef type_node                               TNode;
public:
    void CleanupNodes()
    {
        cleanup_nodes();
    }
    type_node * BorrowNode()
    {
        type_node * pNode = (type_node *)borrow_node(sizeof(type_node), rowSize);
        if (pNode != NULL)
        {
            chConstructObject(type_node, pNode); // construct it
        }
        return pNode;
    }
    void ReturnNode(type_node * pObj)
    {
        chDestructObject(type_node, pObj); // destruct it
        return_node(pObj);
    }
};

//////////////////////////////////////////////////////////////////////////
// class baseStaticNodeAlloc
// declare a big buffer in size of nRowCount * sizeof(typeNode) for list
class baseStaticNodeAlloc
{
    typedef list_chain_offset           type_chain;
protected:
    void cleanup_nodes();
    LPVOID borrow_node();
    void return_node(LPVOID pData);
private:
    type_chain * get_FirstFreeNode();
    void set_FirstFreeNode(type_chain * pNode);
protected:
    baseStaticNodeAlloc(int sizeNode, int nNodeCount);
    ~baseStaticNodeAlloc();
public:
    int                 m_nBorrowedCount;
    POINTER             m_iOffsetFirstFreeNode;
};

//////////////////////////////////////////////////////////////////////////
// class chAlloc_Node_static
// declare a big buffer in size of nRowCount * sizeof(typeNode) for list
template <class typeNode, int itemCount>
class chAlloc_Node_static : public baseStaticNodeAlloc
{
    typedef typeNode                        type_node;
public:
    typedef type_node                       TNode;
public:
    void CleanupNodes()
    {
        baseStaticNodeAlloc::cleanup_nodes();
    }
    type_node * BorrowNode()
    {
        type_node * pNode = (type_node *)baseStaticNodeAlloc::borrow_node();
        if (pNode != NULL)
        {
            chConstructObject(type_node, pNode); // construct it
        }
        return pNode;
    }
    void ReturnNode(type_node * pNode)
    {
        chDestructObject(type_node, pNode); // destruct it
        baseStaticNodeAlloc::return_node(pNode);
    }
protected:
    chAlloc_Node_static() : baseStaticNodeAlloc(sizeof(type_node), itemCount)
    {
    }
protected:
    BYTE                bufferPool[sizeof(type_node) * itemCount];
};

//////////////////////////////////////////////////////////////////////////
// class chAlloc_Node_share
// declare a offset positon of a chAlloc_Node_static object, for many list share one pool
template <class typeNode, int itemCount>
class chAlloc_Node_share
{
    typedef chAlloc_Node_static<typeNode, itemCount>            type_static_pool;
public:
    typedef typeNode                                            TNode;
public:
    void InitPool(type_static_pool * pPool)
    {
        set_pStaticPool(pPool);
    }
public:
    void CleanupNodes()
    {
        get_pStaticPool()->cleanup_pool();
    }
    typeNode * BorrowNode()
    {
        return get_pStaticPool()->BorrowNode();
    }
    void ReturnNode(typeNode * pObj)
    {
        get_pStaticPool()->ReturnNode(pObj);
    }
private:
    type_static_pool * get_pStaticPool()
    {
        return m_iOffsetStaticPool == MAX_POINTER ? NULL : (type_static_pool *)((
                    POINTER)this + m_iOffsetStaticPool);
    }
    void set_pStaticPool(type_static_pool * pPool)
    {
        chASSERT(get_pStaticPool() == NULL && pPool != NULL);
        m_iOffsetStaticPool = pPool == NULL ? MAX_POINTER : (POINTER)pPool - (POINTER)this;
    }
protected:
    chAlloc_Node_share()
    {
        m_iOffsetStaticPool = MAX_POINTER;
    }
    ~chAlloc_Node_share()
    {
    }
private:
    POINTER             m_iOffsetStaticPool;
};



//////////////////////////////////////////////////////////////////////////
// class baseStaticPoolAlloc
// declare a big buffer in size of sizePool for list
class baseStaticPoolAlloc
{
protected:
    typedef list_chain_offset   type_chain;
    typedef poll_chain          type_cell;
public:
    BOOL init_pool(LPVOID pPollData, int nPoolSize);

    void cleanup_pool();
    LPVOID alloc(int nAllocSize);
    void free(LPVOID pData);
    int get_available_bytes(LPVOID pData);
    type_cell * find_big_enough_cell(int nBodySize);
protected:
    void pop_free_cell(type_cell * pCell);
    void push_free_cell(type_cell * pCell);

    bool try_join_right_cell(type_cell * pCell);
    void disjoin_big_cell(type_cell * pBigCell, int nAllocSize);
    bool is_valid_cell(type_cell * pCell);

    type_cell * begin_cell();
    type_cell * end_cell();

public:
    baseStaticPoolAlloc();
    ~baseStaticPoolAlloc();

protected:
    int         m_nPoolSize;
    int         m_offsetBeginCell;
    type_chain  m_vRootChain;

};

//////////////////////////////////////////////////////////////////////////
// class baseSharePoolAlloc
class baseSharePoolAlloc
{
protected:
    void InitPool(baseStaticPoolAlloc * pPool);
public:
    void cleanup_pool();
    LPVOID alloc(int nAllocSize);
    void free(LPVOID pData);
public:
    baseStaticPoolAlloc * get_pPool();
    void set_pPool(baseStaticPoolAlloc * pPool);
protected:
    baseSharePoolAlloc();
    ~baseSharePoolAlloc();
private:
    POINTER             m_iOffsetStaticPool;
};

//////////////////////////////////////////////////////////////////////////
// class chAlloc_Pool_static
template<class typeNode, int sizePool>
class chAlloc_Pool_static : public baseSharePoolAlloc
{
    typedef typeNode                                type_node;
public:
    typedef type_node                               TNode;
public:
    void CleanupNodes()
    {
        baseStaticPoolAlloc::cleanup_pool();
    }
    type_node * BorrowNode(int nExtraSize = 0)
    {
        chASSERT(nExtraSize >= 0);
        type_node * pNode = (type_node *)baseStaticPoolAlloc::alloc(sizeof(type_node) + nExtraSize);
        if (pNode != NULL)
        {
            chConstructObject(type_node, pNode);
        }
        return pNode;
    }
    void ReturnNode(type_node * pNode)
    {
        chASSERT(pNode != NULL);
        chDestructObject(type_node, pNode);
        baseStaticPoolAlloc::free(pNode);
    }
};


//////////////////////////////////////////////////////////////////////////
// class chAlloc_Pool_share
// declare a offset positon of a chAlloc_Node_static object, for many list share one pool
template <class typeNode>
class chAlloc_Pool_share : public baseSharePoolAlloc
{
    typedef typeNode                        type_node;
public:
    typedef type_node                       TNode;
public:
    void CleanupNodes()
    {
        baseSharePoolAlloc::cleanup_pool();
    }
    type_node * BorrowNode(int nExtraSize = 0)
    {
        chASSERT(nExtraSize >= 0);
        type_node * pNode = (type_node *)baseSharePoolAlloc::alloc(sizeof(type_node) + nExtraSize);
        if (pNode != NULL)
        {
            chConstructObject(type_node, pNode);
        }
        return pNode;
    }
    void ReturnNode(type_node * pNode)
    {
        chASSERT(pNode != NULL);
        chDestructObject(type_node, pNode);
        baseSharePoolAlloc::free(pNode);
    }
};

};
using namespace ETL_AllocHelper;

#if IF_EXPORT_SYMBOLS
#include "_chAlloc.cpp"
#endif

#endif //__chALLOC_H__

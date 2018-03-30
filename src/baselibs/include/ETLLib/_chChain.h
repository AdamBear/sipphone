#ifndef __chCHAIN_H__
#define __chCHAIN_H__

#ifndef __ETL_H__
#error _chChain.h only can be include in ETL.h
#endif

namespace ETL_ChainHelper
{

#define declare_pointer_chain(type) \
class pointer_chain_##type\
{\
    typedef pointer_chain_##type    type_chain;\
public:\
    type_chain* get_##type() const;\
    void set_##type(const type_chain* pChain);\
public:\
    /*pointer_chain_##type() : p##type(NULL) {}*/\
protected:\
    type_chain*         p##type;\
}

#define declare_offset_chain(type) \
class offset_chain_##type\
{\
    typedef offset_chain_##type type_chain;\
public:\
    type_chain* get_##type() const;\
    void set_##type(const type_chain* pChain);\
public:\
    /*offset_chain_##type() { i##type = 0 - (POINTER)this; }*/\
protected:\
    POINTER         i##type;\
}

declare_pointer_chain(pNext);
declare_pointer_chain(pPrev);
declare_pointer_chain(pRight);
declare_pointer_chain(pLeft);

declare_offset_chain(pNext);
declare_offset_chain(pPrev);
declare_offset_chain(pRight);
declare_offset_chain(pLeft);

//////////////////////////////////////////////////////////////////////////
// class list_chain
template<class typeNext, class typePrev>
class list_chain : public typeNext
    , public typePrev
{
public:
    typedef typeNext                TNext;
    typedef typePrev                TPrev;
};

template<class typeListChain>
inline void link_list_node(typeListChain * pNodePrev, typeListChain * pNodeNext)
{
    if (pNodePrev != NULL)
    {
        pNodePrev->set_pNext(pNodeNext);
    }
    if (pNodeNext != NULL)
    {
        pNodeNext->set_pPrev(pNodePrev);
    }
}

template<class typeListChain>
inline void unlink_list_node(typeListChain * pNode)
{
    chASSERT(pNode != NULL);
    typeListChain * pPrev = static_cast<typeListChain *>(pNode->get_pPrev());
    typeListChain * pNext = static_cast<typeListChain *>(pNode->get_pNext());
    if (pNext != NULL)
    {
        pNext->set_pPrev(pPrev);
        pNode->set_pNext(NULL);
    }
    if (pPrev != NULL)
    {
        pPrev->set_pNext(pNext);
        pNode->set_pPrev(NULL);
    }
}

///////////////////////////////////////////////////////////////
// class list_chain_pointer
class list_chain_pointer : public list_chain<pointer_chain_pNext, pointer_chain_pPrev>
{
};

///////////////////////////////////////////////////////////////
// class list_chain_offset
class list_chain_offset : public list_chain<offset_chain_pNext, offset_chain_pPrev>
{
};

//////////////////////////////////////////////////////////////////////////
// class list_node
template<class typeObj, class typeChain>
class list_node : public typeChain
{
public:
    typedef typeChain               TChain;
    typedef typeObj                 T;
public:
    typeObj         vData;
};

//////////////////////////////////////////////////////////////////////////
// class list_node_pointer
template<class typeObj>
class list_node_pointer : public list_node<typeObj, list_chain_pointer>
{
};

//////////////////////////////////////////////////////////////////////////
// class list_node_offset
template<class typeObj>
class list_node_offset : public list_node<typeObj, list_chain_offset>
{
};

//////////////////////////////////////////////////////////////////////////
// class map_chain
template<class typeNext, class typePrev, class typeRight, class typeLeft>
class map_chain : public typeNext
    , public typePrev
    , public typeRight
    , public typeLeft
{
};

//////////////////////////////////////////////////////////////////////////
// class map_chain_pointer
class map_chain_pointer : public
    map_chain< pointer_chain_pNext, pointer_chain_pPrev, pointer_chain_pRight, pointer_chain_pLeft >
{
};

//////////////////////////////////////////////////////////////////////////
// class map_chain_offset
class map_chain_offset : public
    map_chain<  offset_chain_pNext, offset_chain_pPrev, offset_chain_pRight, offset_chain_pLeft >
{
};

//////////////////////////////////////////////////////////////////////////
// class map_node
template<class typeObj, class typeChain>
class map_node : public typeChain
{
public:
    typedef typeChain               TChain;
    typedef typeObj                 T;
public:
    typeObj                 vData;
};

//////////////////////////////////////////////////////////////////////////
// class pair_type
template <class typeKey, class typeValue>
class pair_type
{
public:
    typedef typeKey                             TKey;
    typedef typeValue                           TValue;
public:
    bool operator == (const pair_type & refOther) const
    {
        return first == refOther.first;
    }
    bool operator != (const pair_type & refOther) const
    {
        return !(first == refOther.first);
    }
    bool operator > (const pair_type & refOther) const
    {
        return first > refOther.first;
    }
    bool operator < (const pair_type & refOther) const
    {
        return first < refOther.first;
    }

    operator const typeKey & () const
    {
        return first;
    }
    pair_type & operator = (const typeKey & key)
    {
        first = key;
        return *this;
    }
public:
    pair_type()
    {
    }

    pair_type(const typeKey & key) : first(key)
    {
    }

    pair_type(const typeKey & key, const typeValue & val) : first(key), second(val)
    {
    }
    ~pair_type()
    {
    }
public:
    typeKey                 first;
    typeValue               second;
};

//////////////////////////////////////////////////////////////////////////
// class map_node_pointer
template <class typeKey, class typeValue>
class map_node_pointer : public map_node< pair_type<typeKey, typeValue>, map_chain_pointer >
{
};

//////////////////////////////////////////////////////////////////////////
// class map_node_offset
template <class typeKey, class typeValue>
class map_node_offset : public map_node< pair_type<typeKey, typeValue>, map_chain_offset >
{
};

//////////////////////////////////////////////////////////////////////////
// class poll_chain
class poll_chain
{
    typedef list_chain_offset       type_chain;
    typedef poll_chain              type_cell;
public:
    type_chain * get_pData() const ;
    static type_cell * get_pCell(type_chain * pChain);
    int get_nPoolSize() const ;
    bool is_free_cell();
    void set_cell_free(bool bFree);
    type_cell * get_pRight() const;
    void set_pRight(type_cell * pCell);
private:
    POINTER get_iOffsetRightCell() const;
    void set_iOffsetRightCell(POINTER iOffset);
private:
    POINTER         iOffsetRightCell;
};





};
using namespace ETL_ChainHelper;


#if IF_EXPORT_SYMBOLS
#include "_chChain.cpp"
#endif
#endif //__chCHAIN_H__

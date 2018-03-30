#ifndef __chLIST_H__
#define __chLIST_H__

#ifndef __ETL_H__
#error _chList.h only can be include in ETL.h
#endif

#include <ylstl/yllist.h>

namespace ETL_ListHelper
{
//////////////////////////////////////////////////////////////////////////
// class list_iterator
template <class typeNode>
class list_iterator
{
    typedef typeNode                        type_node;
    typedef typename type_node::TChain      type_chain;
    typedef typename type_node::T           type_obj;
public:
    bool operator == (const list_iterator & other) const
    {
        return m_pThisNode == other.m_pThisNode;
    }
    bool operator != (const list_iterator & other) const
    {
        return m_pThisNode != other.m_pThisNode;
    }

    type_obj & operator*() const
    {
        return m_pThisNode->vData;
    }

    list_iterator & operator++()
    {
        m_pThisNode = static_cast<type_node *>(m_pThisNode->get_pNext());
        return *this;
    }
    list_iterator & operator--()
    {
        m_pThisNode = static_cast<type_node *>(m_pThisNode->get_pPrev());
        return *this;
    }
    list_iterator operator + (int iOffset) const
    {
        type_node * pNode = m_pThisNode;
        while (iOffset > 0)
        {
            iOffset --;
            pNode = static_cast<type_node *>(pNode->get_pNext());
        }
        while (iOffset < 0)
        {
            iOffset ++;
            pNode = static_cast<type_node *>(pNode->get_pPrev());
        }
        return list_iterator(pNode);
    }
private:
    list_iterator operator++(int)
    {
        list_iterator tmp = *this;
        m_pThisNode = static_cast<type_node *>(m_pThisNode->get_pNext());
        return tmp;
    }
    list_iterator operator--(int)
    {
        list_iterator tmp(m_pThisNode);
        m_pThisNode = static_cast<type_node *>(m_pThisNode->get_pPrev());
        return tmp;
    }
public:
    list_iterator(const type_node * pNode = NULL)
    {
        m_pThisNode = const_cast<type_node *>(pNode);
    }
public:
    type_node   *   m_pThisNode;
};

typedef BOOL (CALLBACK * list_enum_callback)(void * pExtraObject, void * pListItem);
//////////////////////////////////////////////////////////////////////////
// class chMeth_List
template <class typeAlloc>
class chMeth_List : public typeAlloc
{
    typedef typeAlloc                                   type_alloc;
    typedef typename type_alloc::TNode                  type_node;
    typedef typename type_node::TChain                  type_chain;
    typedef typename type_node::T                       type_obj;
public:
    typedef type_obj                                    T;
    typedef list_iterator<type_node>                    iterator;
public:
    iterator begin() const
    {
        return iterator(static_cast<type_node *>(m_vRootChain.get_pNext()));
    }

    iterator end() const
    {
        return iterator((type_node *)&m_vRootChain);
    }

    int size() const
    {
        return m_nElementCount;
    }

    bool empty() const
    {
        return m_nElementCount == 0;
    }

    // element operactor
    void push_back(const type_obj & tObj)
    {
        insert(end(), tObj);
    }

    void push_front(const type_obj & tObj)
    {
        insert(begin(), tObj);
    }

    iterator insert(iterator it, const type_obj & tObj)
    {
        alloc_insert(it) = tObj;
        return --it;
    }

    void pop_back()
    {
        erase(--end());
    }

    void pop_front()
    {
        erase(begin());
    }

    type_obj & back() const
    {
        chASSERT(!empty());
        return *(--end());
    }

    type_obj & front() const
    {
        chASSERT(!empty());
        return *begin();
    }

    iterator find(const type_obj & tObj) const
    {
        iterator it = begin();
        while (it != end())
        {
            if (*it == tObj)
            {
                break;
            }
            ++it;
        }
        return it;
    }

    void erase(iterator it)
    {
        chASSERT(!empty() && it.m_pThisNode != &m_vRootChain);
        if (m_nElementCount > 0 && it.m_pThisNode != &m_vRootChain)
        {
            m_nElementCount --;
            unlink_list_node<type_chain>(it.m_pThisNode);
            typeAlloc::ReturnNode(it.m_pThisNode);
        }
    }

    void clear()
    {
        while (!empty())
        {
            erase(begin());
        }
    }

    chMeth_List & operator = (const chMeth_List & srcList)
    {
        clear();
        iterator it = srcList.begin();
        while (it != srcList.end())
        {
            push_back(*it);
            ++it;
        }
        return *this;
    }
public:
    static iterator value_pointer_2_iterator(const type_obj * pValue)
    {
        return iterator(castOBJECT(pValue, type_node, vData));
    }

    static type_node * value_pointer_2_node(const type_obj * pValue)
    {
        return castOBJECT(pValue, type_node, vData);
    }

    void for_each(void * pExtraObject, list_enum_callback enumtor)
    {
        iterator it = begin();
        while (it != end())
        {
            type_obj & refData = *it;
            if (!(*enumtor)(pExtraObject, &refData))
            {
                break;
            }
            ++it;
        }
    }

    iterator find_value_pointer(const type_obj * pObj) const
    {
        iterator it = begin();
        while (it != end())
        {
            type_obj & refData = *it;
            if (&refData == pObj)
            {
                return it;
            }
            ++it;
        }
        return it;
    }
    void erase_value_pointer(const type_obj * pObj)
    {
        chASSERT(find_value_pointer(pObj) != this->end());
        erase(value_pointer_2_iterator(pObj));
    }
    void erase_value(const type_obj & tObj)
    {
        erase(find(tObj));
    }


    void move_to(iterator itFrom, iterator itTo)
    {
        chASSERT(itFrom != end());
        if (itTo != itFrom)
        {
            type_node * pNode = itFrom.m_pThisNode;
            unlink_list_node<type_chain>(pNode);
            link_list_node<type_chain>(static_cast<type_chain *>(itTo.m_pThisNode->get_pPrev()), pNode);
            link_list_node<type_chain>(pNode, itTo.m_pThisNode);
        }
    }

    void RemoveAll() // remove all alloc data
    {
        clear();
        typeAlloc::CleanupNodes();
    }

    // element operactor
    iterator tail() const
    {
        return --end();
    }

    type_obj & alloc_push_back()
    {
        return alloc_insert(end());
    }

    type_obj & alloc_push_front()
    {
        return alloc_insert(begin());
    }

    type_obj & alloc_insert(iterator it)
    {
        chASSERT(it.m_pThisNode != NULL);
        type_node * pNewNode = typeAlloc::BorrowNode();
        link_list_node<type_chain>(static_cast<type_chain *>(it.m_pThisNode->get_pPrev()), pNewNode);
        link_list_node<type_chain>(pNewNode, it.m_pThisNode);
        m_nElementCount ++;
        return pNewNode->vData;
    }
public:
    chMeth_List()
    {
        m_nElementCount = 0;
        link_list_node<type_chain>(&m_vRootChain, &m_vRootChain);
    }
    chMeth_List(chMeth_List & srcArray)
    {
        m_nElementCount = 0;
        link_list_node<type_chain>(&m_vRootChain, &m_vRootChain);
        operator = (srcArray);
    }
    ~chMeth_List()
    {
        clear();
    }
protected:
    int                 m_nElementCount;
    type_chain          m_vRootChain;
};

#define LIST_ROWSIZE            16

#if 0
//////////////////////////////////////////////////////////////////////////
// class chObjList_static
template <class typeObj, int rowCount = LIST_ROWSIZE>
class chObjList_static : public
    chMeth_List< chAlloc_Node_static< list_node_offset<typeObj>, rowCount> >
{
};

//////////////////////////////////////////////////////////////////////////
// class chObjList_share

template <class typeObj, int sizePool>
class chObjList_share : public
    chMeth_List< chAlloc_Node_share< list_node_offset<typeObj>, sizePool> >
{
};
#endif

//////////////////////////////////////////////////////////////////////////
// class chObjList_pool_share
template <class typeObj>
class chObjList_pool_share : public chMeth_List< chAlloc_Pool_share< list_node_offset<typeObj> > >
{
};

#define chObjList       YLList


/////////////////////////////////////////////////////////////////////////
// class chMeth_PairList
template <class typeAlloc>
class chMeth_PairList : public chMeth_List<typeAlloc>
{
    typedef chMeth_List<typeAlloc>                          type_meth_list;
    typedef typeAlloc                                       type_alloc;
    typedef typename type_alloc::TNode                      type_node;
    typedef typename type_node::T                           type_pair;
    typedef typename type_pair::TKey                        type_key;
    typedef typename type_pair::TValue                      type_value;
public:
    typedef typename type_meth_list::iterator               iterator;
    typedef type_pair                                       TPair;
    typedef type_key                                        TKey;
    typedef type_value                                      TValue;
public:
    iterator find_key(const type_key & key)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (key == rPair.first)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    iterator find_key_pointer(const type_key & pKey)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (pKey == &rPair.first)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    iterator find_value(const type_value & value)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (value == rPair.second)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    iterator find_value_pointer(const type_value * pValue)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (pValue == &rPair.second)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    // element operactor
    void push_back(const type_key & key, const type_value & val)
    {
        type_pair & obj = this->alloc_push_back();
        obj.first = key;
        obj.second = val;
    }

    void push_front(const type_key & key, const type_value & val)
    {
        type_pair & obj = this->alloc_push_back(this->begin());
        obj.first = key;
        obj.second = val;
    }

    iterator insert(iterator it, const type_key & key, const type_value & val)
    {
        type_pair & obj = alloc_insert(it);
        obj.first = key;
        obj.second = val;
        return --it;
    }
};



//////////////////////////////////////////////////////////////////////////
// class chPairList_static
template <class typeKey, class typeValue, int rowCount = LIST_ROWSIZE>
class chPairList_static : public
    chMeth_PairList< chAlloc_Node_static< list_node_offset< pair_type<typeKey, typeValue> >, rowCount> >
{
};

//////////////////////////////////////////////////////////////////////////
// class chPairList_share
template <class typeKey, class typeValue, int sizePool>
class chPairList_share : public
    chMeth_PairList< chAlloc_Node_share< list_node_offset< pair_type<typeKey, typeValue> >, sizePool> >
{
};

//////////////////////////////////////////////////////////////////////////
// class chPairList_pool_share
template <class typeKey, class typeValue>
class chPairList_pool_share : public
    chMeth_PairList< chAlloc_Pool_share< list_node_offset< pair_type<typeKey, typeValue> > > >
{
};


//////////////////////////////////////////////////////////////////////////
// class chMeth_MapList
template <class typeAlloc>
class chMeth_MapList : public chMeth_List<typeAlloc>
{
    typedef chMeth_List<typeAlloc>                          type_meth_list;
    typedef typeAlloc                                       type_alloc;
    typedef typename type_alloc::TNode                      type_node;
    typedef typename type_node::T                           type_pair;
    typedef typename type_pair::TKey                        type_key;
    typedef typename type_pair::TValue                      type_value;
public:
    typedef typename type_meth_list::iterator               iterator;
    typedef type_pair                                       TPair;
    typedef type_key                                        TKey;
    typedef type_value                                      TValue;
public:
    type_value & operator [](const type_key & key)
    {
        iterator it = this->begin();
        type_pair * pPair = NULL;
        while (pPair == NULL)
        {
            type_pair & rPair = *it;
            if (it == this->end())
            {
                pPair = &this->alloc_push_back();
                pPair->first = key;
            }
            else if (rPair.first == key)
            {
                pPair = &rPair;
            }
            else if (key < rPair.first)
            {
                pPair = &alloc_insert(it);
                pPair->first = key;
            }
            else
            {
                ++it;
            }
        }
        return pPair->second;
    }
    void erase(iterator it)
    {
        type_meth_list::erase(it);
    }
public:
    iterator find_key(const type_key & key)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (key == rPair.first)
            {
                break;
            }
            else if (key < rPair.first)
            {
                return this->end();
            }
            ++it;
        }
        return it;
    }
    iterator find_key_pointer(const type_key * pKey)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (pKey == &rPair.first)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    void erase_key(const type_key & key)
    {
        iterator it = find_key(key);
        if (it != this->end())
        {
            type_meth_list::erase(it);
        }
    }
    iterator find_value(const type_value & value)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (value == rPair.second)
            {
                break;
            }
            ++it;
        }
        return it;
    }
    iterator find_value_pointer(const type_value * pValue)
    {
        iterator it = this->begin();
        while (it != this->end())
        {
            type_pair & rPair = *it;
            if (pValue == &rPair.second)
            {
                break;
            }
            ++it;
        }
        return it;
    }
};

//////////////////////////////////////////////////////////////////////////
// class chMapList_dynamic
template <class typeKey, class typeValue, int rowCount = LIST_ROWSIZE>
class chMapList_dynamic : public
    chMeth_MapList< chAlloc_Node_dynamic< list_node_pointer< pair_type<typeKey, typeValue> >, rowCount> >
{
};

//////////////////////////////////////////////////////////////////////////
// class chMapList_static
template <class typeKey, class typeValue, int rowCount = LIST_ROWSIZE>
class chMapList_static : public
    chMeth_MapList< chAlloc_Node_static< list_node_offset< pair_type<typeKey, typeValue> >, rowCount> >
{
};


//////////////////////////////////////////////////////////////////////////
// class chMapList_pool_share
template <class typeKey, class typeValue>
class chMapList_pool_share : public
    chMeth_MapList< chAlloc_Pool_share< list_node_offset< pair_type<typeKey, typeValue> > > >
{
};

#define chMapList               chMapList_dynamic

#if 0
template <class typeAlloc>
class chMeth_Cabin : public chMeth_List<typeAlloc>
{
    typedef typeAlloc                                   type_alloc;
    typedef typename type_alloc::TNode                  type_node;
    typedef typename type_node::TChain                  type_chain;
    typedef typename type_node::T                       type_key;
public:
    typedef type_key                                    T;
    typedef list_iterator<type_node>                    iterator;
public:
    LPVOID request_value_object(const type_key & key, int nValueSize, BOOL & bNeedConstructObject)
    {
        iterator it = this->begin();
        type_key * pKeyData = NULL;
        while (pKeyData == NULL)
        {
            type_key & objKey = *it;
            if (it == this->end())
            {
                pKeyData = &this->alloc_insert(this->end(), nValueSize);
                bNeedConstructObject = TRUE;
                *pKeyData = key;
            }
            else if (objKey == key)
            {
                pKeyData = &objKey;
                bNeedConstructObject = FALSE;
            }
            else if (key < objKey)
            {
                pKeyData = &alloc_insert(it, nValueSize);
                bNeedConstructObject = TRUE;
                *pKeyData = key;
            }
            else
            {
                ++it;
            }
        }
        return get_value_pointer(pKeyData);
    }
    LPVOID find_object(const type_key & key)
    {
        iterator it = this->begin();
        type_key * pKeyData = NULL;
        while (pKeyData == NULL)
        {
            type_key & objKey = *it;
            if (it == this->end())
            {
                return NULL;
            }
            else if (objKey == key)
            {
                return get_value_pointer(&objKey);
            }
            else if (key < objKey)
            {
                return NULL;
            }
            else
            {
                ++it;
            }
        }
        return NULL;
    }
    iterator find_value_pointer(LPVOID pDate)
    {
        type_key * pKeyData = (type_key *)((LPBYTE)pDate - sizeof(type_key));
        return iterator(castOBJECT(pKeyData, type_node, vData));
    }
    static LPVOID get_value_pointer(const type_key * pKeyDate)
    {
        return castOBJECT(pKeyDate, type_node, vData) + 1;
    }
    static int get_value_bytes(LPVOID pValue)
    {
        type_node * pNode = ((type_node *)pValue) - 1;
        poll_chain * pCell = poll_chain::get_pCell(pNode);
        return pCell->get_nPoolSize() - ((POINTER)pValue - (POINTER)pNode);
    }
    void erase_key_object(const type_key & key)
    {
        iterator it = find(key);
        if (it != this->end())
        {
            erase(it);
        }
    }

    type_key & alloc_insert(iterator it, int nValueSize)
    {
        chASSERT(it.m_pThisNode != NULL);
        type_node * pNewNode = typeAlloc::BorrowNode(nValueSize);
        link_list_node<type_chain>(static_cast<type_chain *>(it.m_pThisNode->get_pPrev()), pNewNode);
        link_list_node<type_chain>(pNewNode, it.m_pThisNode);
        this->m_nElementCount ++;
        return pNewNode->vData;
    }
};



//////////////////////////////////////////////////////////////////////////
// class chCabin_static
template <class typeKey, int sizePool>
class chCabin_static : public
    chMeth_Cabin< chAlloc_Pool_static< list_node_offset<typeKey>, sizePool> >
{
};

//////////////////////////////////////////////////////////////////////////
// class chCabin_share
template <class typeKey>
class chCabin_share : public chMeth_Cabin< chAlloc_Pool_share< list_node_offset<typeKey> > >
{
};

#define chCabin             chCabin_dynamic
#endif

};
using namespace ETL_ListHelper;

#if IF_EXPORT_SYMBOLS
#include "_chList.cpp"
#endif

#endif //__chLIST_H__

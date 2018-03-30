#ifndef __chMAP_H__
#define __chMAP_H__

#ifndef __ETL_H__
#error _chMap.h only can be include in ETL.h
#endif

namespace ETL_MapHelper
{

//////////////////////////////////////////////////////////////////////////
// class chMeth_Map
template <class typeAlloc>
class chMeth_Map : public chMeth_List<typeAlloc>
{
    typedef chMeth_List<typeAlloc>                          type_list_meth;
    typedef typeAlloc                                       type_alloc;
    typedef typename type_alloc::TNode                      type_node;
    typedef typename type_node::TChain                      type_chain;
    typedef typename type_node::T                           type_pair;
    typedef typename type_pair::TKey                        type_key;
    typedef typename type_pair::TValue                      type_value;
public:
    typedef typename type_list_meth::iterator               iterator;
public:
    type_value * find_key(const type_key & key)
    {
        for (iterator it = this->begin(); it != this->end(); ++it)
        {
            type_pair & refObj = *it;
            if (key == refObj.first)
            {
                return &refObj.second;
            }
        }
        return NULL;
    }
    type_value * find_key_pointer(const type_key * pKey)
    {
        for (iterator it = this->begin(); it != this->end(); ++it)
        {
            type_pair & refObj = *it;
            if (pKey == &refObj.first)
            {
                return &refObj.second;
            }
        }
        return NULL;
    }
    const type_key * find_value(const type_value & value)
    {
        for (iterator it = this->begin(); it != this->end(); ++it)
        {
            type_pair & refObj = *it;
            if (value == refObj.second)
            {
                return &refObj.first;
            }
        }
        return NULL;
    }
    const type_key * find_value_pointer(const type_value * pValue)
    {
        for (iterator it = this->begin(); it != this->end(); ++it)
        {
            type_pair & refObj = *it;
            if (pValue == &refObj.second)
            {
                return &refObj.first;
            }
        }
        return NULL;
    }
public:
    void erase(iterator it)
    {
        RemoveNode((*it).first);
    }

    void erase(const type_key & key)
    {
        RemoveNode(key);
    }

    type_value & operator [](const type_key & key)
    {
        iterator it = find_or_alloc(key, true);
        return (*it).second;
    }
    iterator find(const type_key & key)
    {
        return find_or_alloc(key, false);
    }
protected:
    void InitMapNode(type_node * pNode)
    {
        pNode->set_pLeft(NULL);
        pNode->set_pRight(NULL);
    }
    iterator find_or_alloc(const type_key & key, bool bAlloc)
    {
        type_node * pRoot = static_cast<type_node *>
                            (&this->m_vRootChain); // use m_vRootNode.vData.pLeft to store root of this tree
        if (pRoot->get_pLeft() == NULL && bAlloc) // no item now
        {
            type_node * pNewNode = value_pointer_2_node(&this->alloc_push_back());
            InitMapNode(pNewNode);
            pRoot->set_pLeft(pNewNode);
            pNewNode->vData.first = key;
            return this->tail();
        }

        type_node * pNode = static_cast<type_node *>(pRoot->get_pLeft());
        while (pNode != NULL)
        {
            if (key == pNode->vData.first)
            {
                return iterator(pNode);
            }
            else if (key < pNode->vData.first) // find left
            {
                if (pNode->get_pLeft() == NULL && bAlloc)
                {
                    type_node * pNewNode = value_pointer_2_node(&this->alloc_push_back());
                    InitMapNode(pNewNode);
                    pNode->set_pLeft(pNewNode);
                    pNewNode->vData.first = key;
                    return this->tail();
                }
                pNode = static_cast<type_node *>(pNode->get_pLeft());
            }
            else // find right
            {
                if (pNode->get_pRight() == NULL && bAlloc)
                {
                    type_node * pNewNode = value_pointer_2_node(&this->alloc_push_back());
                    InitMapNode(pNewNode);
                    pNode->set_pRight(pNewNode);
                    pNewNode->vData.first = key;
                    return this->tail();
                }
                pNode = static_cast<type_node *>(pNode->get_pRight());
            }
        }
        return this->end();
    }
    void RemoveNode(const type_key & key)
    {
        type_node * pRoot = static_cast<type_node *>
                            (&this->m_vRootChain); // use m_vRootChain.vData.pLeft to store root of this tree
        type_node * pPartentNode = pRoot;
        type_node * pNode = static_cast<type_node *>(pRoot->get_pLeft());
        while (pNode != NULL)
        {
            if (key == pNode->vData.first)
            {
                // found and remove
                if (pNode == pPartentNode->get_pLeft())
                {
                    del_left_branch(pPartentNode);
                }
                else
                {
                    del_right_branch(pPartentNode);
                }
                erase(iterator(pNode));
                return ;
            }
            if (key < pNode->vData.first) // find left
            {
                pPartentNode = pNode;
                pNode = static_cast<type_node *>(pNode->get_pLeft());
            }
            else
            {
                pPartentNode = pNode;
                pNode = static_cast<type_node *>(pNode->get_pRight());
            }
        }
    }
private:
    void del_left_branch(type_node * pNode)
    {
        type_node * pRemoveNode = static_cast<type_node *>(pNode->get_pLeft());
        chASSERT(pRemoveNode != NULL);

        if (pRemoveNode->get_pLeft() == NULL)
        {
            pNode->set_pLeft(static_cast<type_node *>(pRemoveNode->get_pRight()));
        }
        else if (pRemoveNode->get_pRight() == NULL)
        {
            pNode->set_pLeft(static_cast<type_node *>(pRemoveNode->get_pLeft()));
        }
        else
        {
            pNode->set_pLeft(pRemoveNode->get_pLeft());

            type_node * pNode = static_cast<type_node *>(pRemoveNode->get_pLeft());
            while (pNode->get_pRight() != NULL)
            {
                pNode = static_cast<type_node *>(pNode->get_pRight());
            }
            pNode->set_pRight(pRemoveNode->get_pRight());
        }
    }
    void del_right_branch(type_node * pNode)
    {
        type_node * pRemoveNode = static_cast<type_node *>(pNode->get_pRight());
        chASSERT(pRemoveNode != NULL);

        if (pRemoveNode->get_pLeft() == NULL)
        {
            pNode->set_pRight(pRemoveNode->get_pRight());
        }
        else if (pRemoveNode->get_pRight() == NULL)
        {
            pNode->set_pRight(static_cast<type_node *>(pRemoveNode->get_pLeft()));
        }
        else
        {
            pNode->set_pRight(pRemoveNode->get_pRight());

            type_node * pNode = static_cast<type_node *>(pRemoveNode->get_pRight());
            while (pNode->get_pLeft() != NULL)
            {
                pNode = static_cast<type_node *>(pNode->get_pLeft());
            }
            pNode->set_pLeft(pRemoveNode->get_pLeft());
        }
    }
public:
    chMeth_Map()
    {
        type_list_meth::m_vRootChain.set_pLeft(NULL);
        type_list_meth::m_vRootChain.set_pRight(NULL);
    }
};


};
using namespace ETL_MapHelper;

#if IF_EXPORT_SYMBOLS
#include "_chMap.cpp"
#endif

#endif //__chMAP_H__

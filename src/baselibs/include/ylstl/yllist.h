#ifndef _YEALINK_LIST_HEADER_
#define  _YEALINK_LIST_HEADER_

#include <stdlib.h>
//uncomment this to use std::list
//#define BASE_YL_LIST

#ifndef BASE_YL_LIST
#include <list>

#define ListIterator iterator

template <typename T>
class YLList : public std::list<T>
{
public:
    typedef std::list<T> detail_containtor;
    typedef typename detail_containtor::iterator    iterator;

public:
    YLList()
        : detail_containtor()
    {
    };
    explicit YLList(unsigned int reserveSize)
        : detail_containtor(reserveSize)
    {
    }
    YLList(const YLList<T> & rhs)
        : detail_containtor(rhs)
    {
    }

    ~YLList()
    {
    };

    iterator GetIterator(const T & value)
    {
        iterator iter = this->begin();
        for (; iter != this->end(); iter++)
        {
            if (*iter == value)
            {
                return iter;
            }
        }
        return (this->end());
    }
};

template <class _ty, class _ptr>
void Sort(YLList<_ty> & ylList, _ptr fun)
{
    ylList.sort(fun);
}

#else

namespace std
{
class bidirectional_iterator_tag;
}
// CHECK_TIERATOR_OPERATOR宏用于测试YLList迭代器是否有误
//#define CHECK_TIERATOR_OPERATOR

#if 0
#ifdef CHECK_TIERATOR_OPERATOR
#define _LIST_ITERATOR
#endif
#else
#ifndef _LIST_ITERATOR
#define _LIST_ITERATOR
#endif
#endif
template <typename T>
class YLList
{
public:
    struct _inner_node_base
    {
    public:
        _inner_node_base * next;
        _inner_node_base * prev;
    public:
        _inner_node_base()
        {
            next = NULL;
            prev = NULL;
        }
        void insertNext(_inner_node_base * pNode)
        {
            if (pNode)
            {
                pNode->prev = this;
                if (next)
                {
                    next->prev = pNode;
                }
                pNode->next = next;
                next = pNode;
            }
        }
        bool insertValue(const T & value)
        {
            _inner_node * pNode = new _inner_node();
            if (pNode)
            {
                pNode->dataEntry = value;
                if (prev)
                {
                    prev->insertNext(pNode);
                    return true;
                }
                else
                {
                    delete pNode;
                    pNode = NULL;
                }
            }
            return false;
        }
        void removeNext()
        {
            if (next)
            {
                next = next->next;
                if (next)
                {
                    next->prev = this;
                }
            }
        }
    };

    struct _inner_node : public _inner_node_base
    {
        T dataEntry;
    };

#ifdef _LIST_ITERATOR
    class ListIterator
    {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef int difference_type;
        typedef int distance_type;  // retained
        typedef value_type * pointer;
        typedef const value_type * const_pointer;
        typedef value_type & reference;
        typedef const value_type & const_reference;
    public:
        _inner_node_base * pRelatedPtr;

        ListIterator(_inner_node_base * pNode)
        {
            pRelatedPtr = pNode;
        }
        ListIterator()
        {
            pRelatedPtr = NULL;
        }
        inline ListIterator operator++(int i)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
            pRelatedPtr = pRelatedPtr->next;
        }
        inline ListIterator & operator++()
        {
            pRelatedPtr = pRelatedPtr->next;
            return *this;
        }
        inline ListIterator & operator--()
        {
            pRelatedPtr = pRelatedPtr->prev;
            return *this;
        }

        inline ListIterator operator--(int)
        {
            ListIterator tmp = *this;
            --*this;
            return tmp;
        }
        inline bool operator!=(const ListIterator & _Right) const
        {
            return (pRelatedPtr != _Right.pRelatedPtr);
        }
        inline bool operator==(const ListIterator & _Right)const
        {
            return (pRelatedPtr == _Right.pRelatedPtr);
        }
#if 0
        inline pointer operator->()
        {
            return &((pRelatedPtr->dataEntry));
        }
#else
        _inner_node_base * operator->()
        {
            return pRelatedPtr;
        }
#endif

#ifdef CHECK_TIERATOR_OPERATOR
        _inner_node_base & operator*()
        {
            return *pRelatedPtr;
        }
    private:
#else
        inline reference operator*()
        {
            return static_cast<_inner_node *>(pRelatedPtr)->dataEntry;
        }
#endif

    };
#endif
private:
    _inner_node_base * head;
    _inner_node_base * tail;

    unsigned int _size;

    void init()
    {
        if (head == NULL)
        {
            head = new _inner_node_base();
        }
        if (tail == NULL)
        {
            tail = new _inner_node_base();
        }

        if (head)
        {
            head->insertNext(tail);
        }
    }
public:
    YLList()
    {
        head = tail = NULL;
        init();

        _size = 0;

    };
    YLList(const YLList<T> & rhs)
    {

        if (this != &rhs)
        {
            head = tail = NULL;
            _size = 0;
            init();
            _inner_node * pTemp = static_cast<_inner_node *>(rhs.head->next);
            for (int i = 0; i < rhs._size; i ++)
            {
                push_back(pTemp->dataEntry);
                pTemp = static_cast<_inner_node *>(pTemp->next);
            }
        }
    }
    YLList & operator= (const YLList & rhs)
    {
        if (this != &rhs)
        {
            clear();
            _inner_node * pTemp = static_cast<_inner_node *>(rhs.head->next);
            for (unsigned int i = 0; i < rhs._size; i ++)
            {
                push_back(pTemp->dataEntry);
                pTemp = static_cast<_inner_node *>(pTemp->next);
            }
        }
        return *this;
    }
    ~YLList()
    {
        clear();
        if (head)
        {
            delete  head;
            head = NULL;
        }
        if (tail)
        {
            delete  tail;
            tail = NULL;
        }
    };

#ifdef _LIST_ITERATOR
    typedef ListIterator iterator;
    typedef ListIterator const_iterator;
#else
    typedef _inner_node * iterator;
    typedef _inner_node * const_iterator;
#endif
    //template <typename T>
    void push_back(const T & value)
    {
        if (tail->insertValue(value))
        {
            _size++;
        }
    }
    void pop_back()
    {
        if (_size > 0)
        {
            _inner_node_base * pNode = tail->prev;
            pNode->prev->removeNext();

            delete  pNode;
            pNode  = NULL;

            _size--;
        }
    }
    T & front()
    {
        return *begin();
        ////if (_size > 0)
        //{
        //    return head->next->dataEntry;
        //}
    }
    const T & front() const
    {
        return *begin();
        ////if (_size > 0)
        //{
        //    return head->next->dataEntry;
        //}
    }
    T & back()
    {
        return *(--end());
        ////if (_size > 0)
        //{
        //    return tail->prev->dataEntry;
        //}
    }
    const T & back() const
    {
        return *(--end());
        ////if (_size > 0)
        //{
        //    return tail->prev->dataEntry;
        //}
    }
    iterator begin()
    {
#ifdef _LIST_ITERATOR
        if (head->next == NULL)
        {
            return iterator(tail);
        }
        return iterator(head->next);
#else
        if (head->next == NULL)
        {
            return tail;
        }
        return head->next;
#endif
    }

    const_iterator begin() const
    {
#ifdef _LIST_ITERATOR
        if (head->next == NULL)
        {
            return const_iterator(tail);
        }
        return const_iterator(head->next);
#else
        if (head->next == NULL)
        {
            return tail;
        }
        return head->next;
#endif
    }

    iterator end()
    {
#ifdef _LIST_ITERATOR
        return iterator(tail);
#else
        return tail;
#endif
    }

    const_iterator end() const
    {
#ifdef _LIST_ITERATOR
        return const_iterator(tail);
#else
        return tail;
#endif
    }

    unsigned int size() const
    {
        return _size;
    }
    void clear()
    {
        while (_size > 0)
        {
            pop_back();
        }
    }
    void push_front(const T & value)
    {
        if (head->next->insertValue(value))
        {
            _size++;
        }
    }
    iterator erase(iterator _Where)
    {
        if (_Where == end())
        {
            return _Where;
        }
        if (_size > 0)
        {
#ifdef _LIST_ITERATOR
            _inner_node_base * pNode = _Where.pRelatedPtr;
#else
            _inner_node_base * pNode = _Where;
#endif
            _inner_node_base * pRet = pNode->next;
            if (pNode)
            {
                pNode->prev->removeNext();
                delete pNode;
                pNode = NULL;
            }
            _size--;
#ifdef _LIST_ITERATOR
            return iterator(pRet);
#else
            return pRet;
#endif

        }
        return end();
    }

    unsigned remove(const T & value)
    {
        unsigned count = 0;
        iterator iter;
        while ((iter = GetIterator(value)) != end())
        {
            ++count;
            erase(iter);
        }
        return count;
    }

    iterator GetIterator(const T & value)
    {
        for (_inner_node_base * pBegin = head->next; pBegin != tail; pBegin = pBegin->next)
        {
            if (pBegin && static_cast<_inner_node *>(pBegin)->dataEntry == value)
            {
#ifdef _LIST_ITERATOR
                return iterator(pBegin);
#else
                return pBegin;
#endif
            }
        }
        return end();
    }
    iterator insert(iterator iter, const T & value)
    {
#ifndef _LIST_ITERATOR//不需要迭代器时需要判断指针begin
        if (iter)
        {
#endif
            if (iter->insertValue(value))
            {
                _size++;
#ifdef _LIST_ITERATOR
                return iterator(iter->prev);
#else
                return iter->prev;
#endif
            }
#ifndef _LIST_ITERATOR //不需要迭代器时需要判断指针end
        }
#endif
        return end();
    }
};

// 合并链表
template <class _ty, class _ptr>
typename YLList<_ty>::iterator MergeList(typename YLList<_ty>::iterator head1,
        typename YLList<_ty>::iterator head2,
        typename YLList<_ty>::iterator end, _ptr fun)
{
    typename YLList<_ty>::iterator tmp;
    typename YLList<_ty>::iterator begin;

#ifdef _LIST_ITERATOR
    begin.pRelatedPtr = head1->prev;
    begin->next = head1.pRelatedPtr;
#else
    begin = head1->prev;
    begin->next = head1;
#endif

    while (head1 != end && head2 != end)
    {
        if (fun(*head1, *head2))
        {
#ifdef _LIST_ITERATOR
            head1->next->prev = head1.pRelatedPtr;
            head1++;
#else
            head1->next->prev = head1;
            head1 = head1->next;
#endif
        }
        else
        {
#ifdef _LIST_ITERATOR
            tmp = head2;
            head2 ++;
            head1->prev->insertNext(tmp.pRelatedPtr);
#else
            tmp = head2;
            head2 = head2->next;
            head1->prev->insertNext(tmp);
#endif
        }
    }

    // head1未尾指向head2
    if (head2 != end)
    {
#ifdef _LIST_ITERATOR
        head1->prev->next = head2.pRelatedPtr;
        head2->prev = head1->prev;
#else
        head1->prev->next = head2;
        head2->prev = head1->prev;
#endif
    }

    //end指向链表未尾
    tmp = begin;
#ifdef _LIST_ITERATOR
    while (tmp->next != end.pRelatedPtr)
    {
        tmp ++;
    }
    end->prev = tmp.pRelatedPtr;

    begin++;
    return begin;
#else
    while (tmp->next != end)
    {
        tmp = tmp->next;
    }
    end->prev = tmp;

    return begin->next;
#endif

}

// 归并排序
template <class _ty, class _ptr>
typename YLList<_ty>::iterator MergeSort(typename YLList<_ty>::iterator head,
        typename YLList<_ty>::iterator begin,
        typename YLList<_ty>::iterator end,
        _ptr fun)
{
    if (head == end)
    {
        return end;
    }
    typename YLList<_ty>::iterator r_head = head;
    typename YLList<_ty>::iterator head1 = head;
    typename YLList<_ty>::iterator head2 = head;

#ifdef _LIST_ITERATOR
    while (head2->next != end.pRelatedPtr && head2->next->next != end.pRelatedPtr)
    {
        head1++;
        head2++;
        head2++;
    }

    if (head1->next == end.pRelatedPtr) /*说明只有一个节点，则返回该节点*/
    {
        return r_head;
    }

    head2 = head1;
    head2++;

    head2->prev = begin.pRelatedPtr;
    head1->next = end.pRelatedPtr;
    head1 = head;

#else
    if (head1->next == end) /*说明只有一个节点，则返回该节点*/
    {
        return r_head;
    }

    while (head2->next != end && head2->next->next != end)
    {
        head1 = head1->next;
        head2 = head2->next->next;
    }

    head2 = head1->next;
    head2->prev = begin;
    head1->next = end;
    head1 = head;
#endif


    /*函数MergeList是对两个有序链表进行归并，返回值是归并后的链表的头结点*/
    r_head = MergeList<_ty, _ptr>(MergeSort<_ty, _ptr>(head1, begin, end, fun),
                                  MergeSort<_ty, _ptr>(head2, begin, end, fun), end, fun);

    return r_head;
}

template <class _ty, class _ptr>
void Sort(YLList<_ty> & ylList, _ptr fun)
{
    typename YLList<_ty>::iterator itHead = ylList.begin()->prev;
    MergeSort<_ty, _ptr>(ylList.begin(), itHead, ylList.end(), fun);
}

#endif // BASE_YL_LIST

#endif


/*
 *
 * LRUCache - a templated LRU cache class that allows specification of
 * key, value and optionally the map container type (defaults to std::map)
 * By using the std::map and a linked list of keys it allows O(1)
 * insert, delete and refresh operations

 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Copy By GitHub
 * Qt
 */

#ifndef _MOHAPS_LRU_CACHE_HPP_
#define _MOHAPS_LRU_CACHE_HPP_

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>
#include <iostream>
#include <ostream>
#include <exception>

namespace lru
{
/**
 * a base lock class that can be overridden
 */
class Lock
{
public:
    virtual ~Lock()
    {
    }
    virtual void lock() = 0;
    virtual void unlock() = 0;
protected:
    Lock()
    {
    }
private:
    Lock(const Lock &);
    const Lock & operator =(const Lock &);
};
/**
 *  Null or noop lock class that is the default synchronization
 *  used in lru cache.
 *
 *  a simple pthread based lock would look like this:
 *  <pre>
 *      class MutexLock : public Lock {
 public:
 MutexLock() { ::pthread_mutex_init(&m_lock, 0); }
 virtual ~MutexLock() { ::pthread_mutex_destroy(&m_lock); }
 void lock() { ::pthread_mutex_lock(&m_lock); }
 void unlock() { ::pthread_mutex_unlock(&m_lock); }
 private:
 pthread_mutex_t m_lock;
 }
 *  </pre>
 *
 */
class NullLock: public Lock
{
public:
    NullLock()
    {
    }
    virtual ~NullLock()
    {
    }
    void lock()
    {
    }
    void unlock()
    {
    }
};
/**
 *  helper class to auto lock and unlock within a scope
 */
class ScopedLock
{
public:
    ScopedLock(Lock & lock) :
        m_lock(lock)
    {
        m_lock.lock();
    }
    virtual ~ScopedLock()
    {
        m_lock.unlock();
    }
private:
    Lock & m_lock;
    ScopedLock(const ScopedLock &);
    const ScopedLock & operator =(const ScopedLock &);
};

// a double linked list node
template<class Key, class Value>
struct Node
{
    Node * prev;
    Node * next;
    Key key;
    Value value;
    Node(const Key & keyObj, const Value & valueObj) :
        prev(0), next(0), key(keyObj), value(valueObj)
    {
    }
    virtual ~Node()
    {
        cleanup();
    }
    void cleanup()
    {
        if (next)
        {
            delete next;
        }
        next = 0;
        prev = 0;
    }
    void unlink()
    {
        if (next)
        {
            next->prev = prev;
        }
        if (prev)
        {
            prev->next = next;
        }
        next = 0;
        prev = 0;
    }
    template<class Visitor>
    void walk(Visitor & visitorFunc)
    {
        visitorFunc(*this);
        if (this->next)
        {
            this->next->walk<Visitor>(visitorFunc);
        }
    }
};

// a doubly linked list class
template<class Key, class Value>
struct List
{
    Node<Key, Value> * head;
    Node<Key, Value> * tail;
    size_t size;
    List() :
        head(0), tail(0), size(0)
    {
    }

    virtual ~List()
    {
        clear();
    }

    void clear()
    {
        if (head)
        {
            delete head;
        }
        head = 0;
        tail = 0;
        size = 0;
    }

    Node<Key, Value> * pop()
    {
        if (!head)
        {
            return 0;
        }
        else
        {
            Node<Key, Value> * newHead = head->next;
            head->unlink();
            Node<Key, Value> * oldHead = head;
            head = newHead;
            size--;
            if (size == 0)
            {
                tail = 0;
            }
            return oldHead;
        }
    }

    Node<Key, Value> * remove(Node<Key, Value> * node)
    {
        if (node == head)
        {
            head = node->next;
        }
        if (node == tail)
        {
            tail = node->prev;
        }
        node->unlink();
        size--;
        return node;
    }

    void push(Node<Key, Value> * node)
    {
        node->unlink();
        if (!head)
        {
            head = node;
        }
        else if (head == tail)
        {
            head->next = node;
            node->prev = head;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
        }
        tail = node;
        size++;
    }
};

// 辅助类，用于做更特殊化的定制
// 比如定制key和value的dump数据
// 根据不同value自己定制何时删除元素、删除元素的个数
template<class Key, class Value, class MapType>
class lruDefaultHelp
{
public:
    lruDefaultHelp(size_t nMaxCounts, size_t nElasticityCounts, size_t nMaxMemSize = 0,
                   size_t nElasticityMemSize = 0)
        : m_maxSize(nMaxCounts)
        , m_elasticity(nElasticityCounts)
    {
    }

    virtual ~lruDefaultHelp()
    {
    }

    // 添加value时会触发
    void OnValueAdd(const Value & objValue)
    {
    }

    // 删除value时会触发
    void OnValueRemove(const Value & objValue)
    {
    }

    // 清楚数据
    void Clear()
    {
    }

    // 是否需要删除冗余元素
    bool IsPrune(const MapType & mapCache,
                 const List<Key, Value>  &  listKeys) const
    {
        return m_maxSize > 0 && mapCache.size() >= (m_maxSize + m_elasticity);
    }

    // 触发删除操作后，剩余的元素个数
    size_t GetSurplusSize(const MapType & mapCache,
                          const List<Key, Value>  & listKeys) const
    {
        return m_maxSize;
    }

    void dumpKeyAndValue(const Key & key, const Value & value) const
    {
    }

    void dumpDebug() const
    {
#ifdef _WIN32
        qDebug() << " (max:" << m_maxSize << ") (elasticity: " << m_elasticity << ")";
#endif
    }

private:
    size_t  m_maxSize;
    size_t  m_elasticity;
};

/**
 *  The LRU Cache class templated by
 *      Key - key type
 *      Value - value type
 *      MapType - an associative container like std::map
 *      LockType - a lock type derived from the Lock class (default: NullLock = no synchronization)
 *      HelpType - 辅助类，用于做更特殊化的定制
 */
template<class Key, class Value,
         class MapType = std::map<Key, Node<Key, Value>*>,
         class LockType = NullLock,
         class HelpType = lruDefaultHelp<Key, Value, MapType> >
class Cache
{
public:
    class KeyNotFound: public std::exception
    {
    public:
        const char * what() const throw ()
        {
            return "KeyNotFound";
        }
    };
    // -- methods
    Cache(size_t nMaxCounts = 64, size_t nElasticityCounts = 10, size_t nMaxMemSize = 0,
          size_t nElasticityMemSize = 0) :
        m_help(nMaxCounts, nElasticityCounts, nMaxMemSize, nElasticityMemSize)
    {
    }
    virtual ~Cache()
    {
    }
    void clear()
    {
        ScopedLock scoped(m_lock);

        typename MapType::iterator iter = m_cache.begin();
        for (; iter != m_cache.end(); ++iter)
        {
            Node<Key, Value> * n = iter->second;
            delete n;
        }

        m_cache.clear();
        m_keys.clear();
        m_help.Clear();
    }
    void insert(const Key & key, const Value & value)
    {
        ScopedLock scoped(m_lock);
        typename MapType::iterator iter = m_cache.find(key);
        if (iter != m_cache.end())
        {
            m_keys.remove(iter->second);
            m_keys.push(iter->second);
        }
        else
        {
            Node<Key, Value> * n = new Node<Key, Value>(key, value);
            m_help.OnValueAdd(value);
            m_cache[key] = n;
            m_keys.push(n);
            prune();
        }
    }
    bool tryGet(const Key & key, Value & value)
    {
        ScopedLock scoped(m_lock);
        typename MapType::iterator iter = m_cache.find(key);
        if (iter == m_cache.end())
        {
            return false;
        }
        else
        {
            m_keys.remove(iter->second);
            m_keys.push(iter->second);
            value = iter->second->value;
            return true;
        }

    }
    const Value & get(const Key & key)
    {
        ScopedLock scoped(m_lock);
        typename MapType::iterator iter = m_cache.find(key);
        if (iter == m_cache.end())
        {
            //throw KeyNotFound();
        }
        m_keys.remove(iter->second);
        m_keys.push(iter->second);
        return iter->second->value;

    }
    void remove(const Key & key)
    {
        ScopedLock scoped(m_lock);
        typename MapType::iterator iter = m_cache.find(key);
        if (iter != m_cache.end())
        {
            Node<Key, Value> * n = m_keys.remove(iter->second);
            m_help.OnValueRemove(n->value);
            m_cache.erase(iter);
            delete n;
        }
    }
    bool contains(const Key & key)
    {
        ScopedLock scoped(m_lock);
        return m_cache.find(key) != m_cache.end();
    }
    MapType getCache()
    {
        return m_cache;
    }

    void dumpDebug() const
    {
        ScopedLock scoped(m_lock);
#ifdef _WIN32
        qDebug() << "Cache Size : " << m_cache.size();
#endif
        m_help.dumpDebug();
        for (Node<Key, Value> * node = m_keys.head; node != NULL; node = node->next)
        {
            m_help.dumpKeyAndValue(node->key, node->value);
        }

    }
protected:
    size_t prune()
    {
        if (m_help.IsPrune(m_cache, m_keys))
        {
            size_t count = 0;
            size_t sizeSurplusSize = m_help.GetSurplusSize(m_cache, m_keys);
            while (m_cache.size() > sizeSurplusSize)
            {
                Node<Key, Value> * n = m_keys.pop();
                m_help.OnValueRemove(n->value);
                m_cache.erase(n->key);
                delete n;
                count++;
            }
            return count;
        }
        else
        {
            return 0;
        }
    }
private:
    mutable LockType    m_lock;
    MapType             m_cache;
    List<Key, Value>    m_keys;
    HelpType            m_help;
private:
    Cache(const Cache &);
    const Cache & operator =(const Cache &);
};

}
#endif

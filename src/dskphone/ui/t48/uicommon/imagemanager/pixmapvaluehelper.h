#ifndef __PIXMAP_VALUE_HELPER_H__
#define __PIXMAP_VALUE_HELPER_H__

template<class Key, class Value, class MapType>
class PixmapValueHelp
{

public:
    PixmapValueHelp(size_t nMaxCounts, size_t nElasticityCounts, size_t nMaxMemSize,
                    size_t nElasticityMemSize)
        : m_nMaxCounts(nMaxCounts)
        , m_nElasticityCounts(nElasticityCounts)
        , m_nMaxMemSize(nMaxMemSize)
        , m_nElasticityMemSize(nElasticityMemSize)
        , m_nCurMemSize(0)
    {
    }

    virtual ~PixmapValueHelp()
    {
    }

    void OnValueAdd(const Value & objValue)
    {
        m_nCurMemSize += GetPixmapMemory(objValue);
    }

    void OnValueRemove(const Value & objValue)
    {
        m_nCurMemSize -= GetPixmapMemory(objValue);
    }

    void Clear()
    {
        m_nCurMemSize = 0;
    }

    bool IsPrune(const MapType & mapCache,
                 const lru::List<Key, Value> & listKeys) const
    {
        // 先判断张数是否满足
        if (m_nMaxCounts > 0 && mapCache.size() >= (m_nMaxCounts + m_nElasticityCounts))
        {
            return true;
        }

        // 判断内存是否满足
        return m_nMaxMemSize > 0 && m_nCurMemSize >= (m_nMaxMemSize + m_nElasticityMemSize);
    }

    size_t GetSurplusSize(const MapType & mapCache,
                          const lru::List<Key, Value> & listKeys) const
    {
        size_t sizeSurplus = 0;
        size_t sizeMemory  = 0;

        for (lru::Node<Key, Value> * node = listKeys.head; node != NULL; node = node->next)
        {
            if (node == NULL)
            {
                break;
            }

            sizeMemory += GetPixmapMemory(node->value);
            if (sizeMemory > m_nMaxMemSize)
            {
                break;
            }

            ++sizeSurplus;
        }

        return sizeSurplus < m_nMaxCounts ? sizeSurplus : m_nMaxCounts;
    }

    void dumpKeyAndValue(const Key & key, const Value & value) const
    {
        UIKERNEL_INFO("Dump Lru Key & Value: Key[%s], ValueSize[%d].", key.c_str(), GetPixmapMemory(value));
    }

    void dumpDebug() const
    {
        UIKERNEL_INFO("Dump Lru Cache: MaxMemSize[%d], ElasticityMemSize[%d], ValueSize[%d].",
                      m_nMaxMemSize, m_nElasticityMemSize, m_nCurMemSize);
    }

private:
    size_t GetPixmapMemory(const Value & objValue) const
    {
        return (objValue.GetPixmap().size().width() * objValue.GetPixmap().size().height()) *
               (objValue.GetPixmap().depth() / 8);
    }

private:
    size_t  m_nCurMemSize;      // 单位B（字节），所有成员的内存大小，使用变量是为了避免每次计算提高效率

    size_t  m_nMaxMemSize;          // 单位B（字节），允许最大内存数
    size_t  m_nElasticityMemSize;       // 单位B（字节），超过最大内存数多少开始删除成员，防止频繁删除导致的性能损失

    size_t  m_nMaxCounts;
    size_t  m_nElasticityCounts;
};

typedef std::string                                                          LRUKeyType;
typedef CImageCachePixmap                                                    LRUValueType;
typedef std::map<LRUKeyType, lru::Node<LRUKeyType, LRUValueType>*>           LRUMapType;

typedef LRUMapType::iterator                                                 ImageCacheIter;
typedef lru::Cache<LRUKeyType, LRUValueType, LRUMapType, lru::NullLock,
        PixmapValueHelp<LRUKeyType, LRUValueType, LRUMapType> >  ImageCache;

#endif // #ifndef __PIXMAP_VALUE_HELPER_H__

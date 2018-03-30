#include "stylebase.h"
#include "imagemanager/modimagemanager.h"

//////////////////////////////////////////////////////////////////////////
// Class myPxCache
myPxCache::PixMap myPxCache::m_mapPx;

//在图片资源缓存中通过图片索引ID及图片路径注册图片，
//如果图片索引ID已注册，会先消除旧的注册信息，再重新
//注册。注册成功返回true,注册失败返回false。
bool myPxCache::RegisterImage(int ePixID, const QString & strImgPath)
{
    QString strFullPath = "";

    if (!Image_ConvertToThemePath(strImgPath, strFullPath))
    {
        strFullPath = strImgPath;
    }

    PixMap::iterator iter = m_mapPx.find(ePixID);

    if (iter != m_mapPx.end())
    {
        //如果已经注册，重新注册
        iter.value().first = strFullPath;
        iter.value().second.clear();
    }
    else
    {
        PixmapPair pxPair(strFullPath, PixmapSharedPtr());
        m_mapPx.insert(ePixID, pxPair);
    }

    return true;
}

//从缓存中清除图片索引ID为ePixID的图片注册信息，同时会
//释放图片资源指针。成功返回true,失败返回false.
bool myPxCache::UnregistrImage(int ePixID)
{
    PixMap::iterator iter = m_mapPx.find(ePixID);
    if (iter != m_mapPx.end())
    {
        m_mapPx.erase(iter);

        return true;
    }
    else
    {
        return false;
    }
}

//通过图片索引ID取得图片资源指针，
//成功返回图片指针，失败返回NULL。
PixmapWeekPtr myPxCache::GetPxCache(int ePixID, bool bReload/* = false*/)
{
    PixmapSharedPtr pxShPtr;

    PixMap::iterator iter = m_mapPx.find(ePixID);

    if (iter != m_mapPx.end())
    {
        pxShPtr = iter.value().second;

        if (bReload)
        {
            //重新加载，清除缓存数据
            pxShPtr.clear();
        }

        if (pxShPtr.isNull())
        {
            // 判断路径是否为空
            if (!iter.value().first.isEmpty())
            {
                pxShPtr =  PixmapSharedPtr(new QPixmap());
                if (!pxShPtr.isNull())
                {
                    if (pxShPtr->load(iter.value().first))
                    {
                        // 读取图片成功
                        iter->second = pxShPtr;
                    }
                }//end if (!pxShPtr.isNull())
            }
        }// end if (pxShPtr.isNull())
    }

    return pxShPtr.toWeakRef();
}

//通过图片索引ID取得图片资源路径，
//成功返回图片路径，失败返回空字符串。
QString myPxCache::GetPxImagePath(int ePixID)
{
    PixMap::const_iterator iter = m_mapPx.find(ePixID);

    if (iter != m_mapPx.end())
    {
        return iter.value().first;
    }
    else
    {
        return "";
    }
}

//释放图片内存，但不会清除其的注册信息。
bool myPxCache::UnloadPxImage(int ePixID)
{
    PixMap::iterator iter = m_mapPx.find(ePixID);

    if (iter != m_mapPx.end())
    {
        iter.value().second.clear();
        return true;
    }
    else
    {
        return false;
    }
}

//图片是否在缓存中注册，是返回true，否返回false。
bool myPxCache::IsImageRegister(int ePixID)
{
    return m_mapPx.contains(ePixID);
}

//图片是否已变加载到内存中，是返回true，否返回false。
bool myPxCache::IsImageLoaded(int ePixID)
{
    PixMap::const_iterator iter = m_mapPx.find(ePixID);

    if (iter != m_mapPx.end())
    {
        return iter.value().second.isNull();
    }
    else
    {
        return false;
    }
}

//缓存中已注册的图片的数量
int myPxCache::Count()
{
    return m_mapPx.size();
}

//清空图片缓存
void myPxCache::ClearPxCache()
{

}

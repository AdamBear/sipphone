#ifndef _IMAGE_INFO_H_
#define _IMAGE_INFO_H_

#include <string>
#include <QPixmap>
#include "imagecachemanagerdefine.h"
//#include "commonunits/basedeletelater.h"

struct Point9Info
{
    //是否水平拉伸
    bool            m_bHStretch;
    //是否竖直拉伸
    bool            m_bVStretch;

    //水平拉伸关键点位置
    int         m_nHStart;
    int         m_nHOver;

    //竖直拉伸关键点位置
    int         m_nVStart;
    int         m_nVOver;

    Point9Info()
    {
        ResetPoint9Info();
    }

    void ResetPoint9Info()
    {
        m_bHStretch = false;
        m_bVStretch = false;

        m_nHStart = 0;
        m_nHOver = 0;
        m_nVStart = 0;
        m_nVOver = 0;
    }
};

//////////////////////////////////////////////////////////////////////////
//  By Qt  需要优化ImageInfo的类体系
// 1、确定缓存、不缓存这类问题是属性不一样还是类不一样
// 2、缓存算法是否在类族中做
//////////////////////////////////////////////////////////////////////////
const QSize G_NO_SCALED(-1, -1);

// 图片信息基类
class CImageBaseInfo// : public CBaseDeleteLater
{
public:
    CImageBaseInfo();

    virtual ~CImageBaseInfo();

    // 是否缓存
    virtual bool IsCache()
    {
        return false;
    }

    // Clear是否删除
    virtual bool IsDeleteWhileClear()
    {
        return true;
    }

    // 图片是否为空
    virtual bool IsNull() = 0;

    // 加载图片
    virtual bool LoadImageInfo(const QString & strImagePath) = 0;
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic) = 0;

    // 将该图片资源转化为QPixmap
    virtual QPixmap GetPixmap() const = 0;

    QString GetImagePath()
    {
        return m_strImagePath;
    }

    void SetImageSize(const QSize & size)
    {
        m_size = size;
    }

    void SetRemoveImage(bool bRemove)
    {
        m_bRemoveImage = bRemove;
    }

    virtual void Clear();

    void SetType(const IMAGE_TYPE eType)
    {
        m_eType = eType;
    }
    IMAGE_TYPE GetType()
    {
        return m_eType;
    }

    static bool LoadPixmapByPath(QPixmap & refPixmap, const QString & strImagePath);

protected:
    bool IsValidImage(const QString & strPath);

public:
    Point9Info  m_Point9Info;

protected:
    bool            m_bRemoveImage;// 析构的时候是否同时删除资源文件
    QString         m_strImagePath;// 文件路径
    QSize           m_size;
    IMAGE_TYPE      m_eType;
};

// 空对象,主要用于异常的时候当做返回值
class CImageEmpty : public CImageBaseInfo
{
public:
    CImageEmpty() {}

    virtual ~CImageEmpty() {}

    virtual bool IsNull()
    {
        return true;
    }

    virtual bool LoadImageInfo(const QString & strImagePath)
    {
        return true;
    }
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic)
    {
        return true;
    }

    virtual QPixmap GetPixmap() const
    {
        return m_NullPixmap;
    }

private:
    QPixmap         m_NullPixmap; // 空Pixmap
};

// 不缓存的图片对象,但会拉伸大小,加载的时候到指定路径下即时读取
class CImageNoCache : public CImageBaseInfo
{
public:
    CImageNoCache();

    virtual ~CImageNoCache();

    virtual bool IsNull();

    virtual bool LoadImageInfo(const QString & strImagePath);
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic);

    virtual QPixmap GetPixmap() const;
};

// 不缓存的图片对象,也不进行拉伸,加载的时候到指定路径下即时读取
class CImageNoCacheNoScaled : public CImageNoCache
{
public:
    CImageNoCacheNoScaled();

    virtual ~CImageNoCacheNoScaled();

    virtual bool LoadImageInfo(const QString & strImagePath);
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic);
};

// 缓存的图片对象,并拉伸到指定大小
class CImageCachePixmap : public CImageBaseInfo
{
public:
    CImageCachePixmap();

    virtual ~CImageCachePixmap();

    // 是否缓存
    virtual bool IsCache()
    {
        return true;
    }

    virtual bool IsNull();

    virtual bool LoadImageInfo(const QString & strImagePath);
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic);

    virtual QPixmap GetPixmap() const;

    QSize GetImageSize();

    bool operator ==(const CImageCachePixmap & other);
    bool operator !=(const CImageCachePixmap & other);
    CImageCachePixmap & operator =(const CImageCachePixmap & other);

private:
    QPixmap         m_pixmap;
};

#endif
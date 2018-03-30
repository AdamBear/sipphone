#ifndef _STYLE_BASE_H_
#define _STYLE_BASE_H_

/*
 * StyleBase
 * 话机上自定义样式的基本文件
 * 用于实现自定义样式的基本数据结构和图片缓存
 */

#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>
#include <QtGui/QPixmap>

typedef QWeakPointer<QPixmap> PixmapWeekPtr;

// This class is used to cache pixmap that is used to draw.
class myPxCache
{
public:
    //在图片资源缓存中通过图片索引ID及图片路径注册图片，
    //如果图片索引ID已注册，会先消除旧的注册信息，再重新
    //注册。注册成功返回true,注册失败返回false。
    static bool RegisterImage(int ePixID, const QString & strImgPath);

    //从缓存中清除图片索引ID为ePixID的图片注册信息，同时会
    //释放图片资源指针。成功返回true,失败返回false.
    static bool UnregistrImage(int ePixID);

    //通过图片索引ID取得图片资源指针，
    //成功返回图片指针，失败返回NULL。
    //bReload==true时每次都重新加载，不
    //用缓存中的数据。
    static PixmapWeekPtr GetPxCache(int ePixID, bool bReload = false);

    //通过图片索引ID取得图片资源路径，
    //成功返回图片路径，失败返回空字符串。
    static QString GetPxImagePath(int ePixID);

    //释放图片内存，但不会清除其的注册信息。
    static bool UnloadPxImage(int ePixID);

    //图片是否在缓存中注册，是返回true，否返回false。
    static bool IsImageRegister(int ePixID);

    //图片是否已变加载到内存中，是返回true，否返回false。
    static bool IsImageLoaded(int ePixID);

    //缓存中已注册的图片的数量
    static int Count();

    //清空图片缓存
    static void ClearPxCache();

public:
    ~myPxCache() {}

private:
    myPxCache() {}
    myPxCache(const myPxCache &) {}

private:
    typedef QSharedPointer<QPixmap> PixmapSharedPtr;
    typedef QPair<QString, PixmapSharedPtr> PixmapPair; //<图片路径，图片指针>
    typedef QMap<int, PixmapPair> PixMap; //图片缓存MAP

private:
    static PixMap m_mapPx;
};
#endif //_STYLE_BASE_H_

#include "imageinfo.h"
#include <QFileInfo>
#include <stdio.h>
#include "dsklog/log.h"
#include <QPixmap>
#include <QPainter>
#include "imagepathconvertor.h"

// 保存合适大小图片的通用处理方式
void SaveCompatibleImage(QImage & img, const QString & strImagePath, const QSize & size)
{
    QImage imageTmp = img.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int iImage_h = imageTmp.height();
    int iImage_w = imageTmp.width();
    if (iImage_w < size.width() || iImage_h < size.height())
    {
        QImage image(size.width(), size.height(), QImage::Format_ARGB32);
        // 配置透明色
        image.fill(qRgba(255, 255, 255, 0));

        QPainter p(&image);
        int x = (size.width() - iImage_w) / 2;
        int y = (size.height() - iImage_h) / 2;
        p.drawImage(x, y, imageTmp);

        image.save(strImagePath, "jpeg");
    }
    else
    {
        imageTmp.save(strImagePath, "jpeg");
    }
}

//////////////////////////////////CImageBaseInfo////////////////////////////////////////
CImageBaseInfo::CImageBaseInfo()
    : m_bRemoveImage(false)
    , m_eType(DIT_NONE)
{

}

CImageBaseInfo::~CImageBaseInfo()
{

}

bool CImageBaseInfo::IsValidImage(const QString & strPath)
{
    return QFile::exists(strPath);
}

void CImageBaseInfo::Clear()
{
    if (IsValidImage(m_strImagePath.toUtf8().data()))
    {
        remove(m_strImagePath.toUtf8().data());
    }
}

bool CImageBaseInfo::LoadPixmapByPath(QPixmap & refPixmap, const QString & strImagePath)
{
    if (!QFile::exists(strImagePath))
    {
        UIKERNEL_INFO("[CImageBaseInfo::LoadPixmapByPath] LoadPixmapByPath File(%s) not Exsit!",
                      strImagePath.toUtf8().data());
        return false;
    }

    if (refPixmap.load(strImagePath))
    {
        return true;
    }

    // 取文件后缀名
    QString strFileType = QFileInfo(strImagePath).suffix().toLower();

    // 图片实际扩展名跟保存扩展名不一致时，容错方案
    if (strFileType != "jpg" && refPixmap.load(strImagePath, "JPG"))
    {
        return true;
    }

    if (strFileType != "jpeg" && refPixmap.load(strImagePath, "JPEG"))
    {
        return true;
    }

    if (strFileType != "png" && refPixmap.load(strImagePath, "PNG"))
    {
        return true;
    }

    if (strFileType != "gif" && refPixmap.load(strImagePath, "GIF"))
    {
        return true;
    }

    if (strFileType != "bmp" && refPixmap.load(strImagePath, "BMP"))
    {
        return true;
    }

    return false;
}

//////////////////////////////////CImageNoCache////////////////////////////////////////
CImageNoCache::CImageNoCache()
{
    m_bRemoveImage = true;
}

CImageNoCache::~CImageNoCache()
{
    if (m_bRemoveImage)
    {
        Clear();
    }
}

bool CImageNoCache::IsNull()
{
    return !IsValidImage(m_strImagePath);
}

bool CImageNoCache::LoadImageInfo(const QString & strImagePath)
{
    QString strFullPath = "";
    if (!GetImagePathConvertor().ConvertToFullPath(strImagePath, strFullPath))
    {
        strFullPath = strImagePath;
    }

    bool bResult = false;
    if (!IsValidImage(strFullPath))
    {
        UIKERNEL_INFO("[CImageNoCache::LoadImageInfo] path[%s] not exist", strFullPath.toUtf8().data());
        return bResult;
    }

    QImage imageTmp(strFullPath);
    // 图片实际扩展名跟保存扩展名不一致时，容错方案
    if (imageTmp.isNull())
    {
        // 取文件后缀名
        QString strFileType = QFileInfo(strFullPath).suffix().toLower();

        if (strFileType != "jpg" && imageTmp.load(strFullPath, "JPG"))
        {
        }
        else if (strFileType != "jpeg" && imageTmp.load(strFullPath, "JPEG"))
        {
        }
        else if (strFileType != "png" && imageTmp.load(strFullPath, "PNG"))
        {
        }
        else if (strFileType != "gif" && imageTmp.load(strFullPath, "GIF"))
        {
        }
        else if (strFileType != "bmp" && imageTmp.load(strFullPath, "BMP"))
        {
        }
    }

    //
    if (imageTmp.isNull())
    {
        UIKERNEL_INFO("[CImageNoCache::LoadImageInfo] CImageNoCache Load (%s) error!",
                      strFullPath.toUtf8().data());
        bResult = false;
    }
    else
    {
        m_strImagePath = strFullPath;
        UIKERNEL_INFO("[CImageNoCache::LoadImageInfo] CImageNoCache ImagePath[%s] imageTmp[%d][%d]",
                      strFullPath.toUtf8().data(), imageTmp.size().width(), imageTmp.size().height());

        SaveCompatibleImage(imageTmp, strFullPath, m_size);

        bResult = true;
    }

    return bResult;
}

bool CImageNoCache::LoadImageInfo(const QString & strImagePath, QPixmap & pic)
{
    QPixmap picTmp = pic.scaled(m_size
                                , (DIT_PROPORTION == m_eType ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio)
                                , Qt::SmoothTransformation);

    m_strImagePath = strImagePath;

    int iPic_h = picTmp.height();
    int iPic_w = picTmp.width();
    if (iPic_w < m_size.width() || iPic_h < m_size.height())
    {
        QPixmap picT(m_size.width(), m_size.height());
        // 配置透明色
        picT.fill(qRgba(255, 255, 255, 0));

        QPainter p(&picT);
        int x = (m_size.width() - iPic_w) / 2;
        int y = (m_size.height() - iPic_h) / 2;
        p.drawPixmap(x, y, picTmp);

        pic = picT;
    }
    else
    {
        pic = picTmp;
    }

    return true;
}

QPixmap CImageNoCache::GetPixmap() const
{
    QPixmap TempPixmap;
    LoadPixmapByPath(TempPixmap, m_strImagePath);
    return TempPixmap;
}
//////////////////////////////////CImageNoCacheNoScaled////////////////////////////////////////
CImageNoCacheNoScaled::CImageNoCacheNoScaled()
{

}

CImageNoCacheNoScaled::~CImageNoCacheNoScaled()
{

}

bool CImageNoCacheNoScaled::LoadImageInfo(const QString & strImagePath)
{
    m_strImagePath = strImagePath;
    return IsValidImage(strImagePath);
}

bool CImageNoCacheNoScaled::LoadImageInfo(const QString & strImagePath, QPixmap & pixScale)
{
    m_strImagePath = strImagePath;
    return IsValidImage(strImagePath);
}

//////////////////////////////////CImageCachePixmap////////////////////////////////////////
CImageCachePixmap::CImageCachePixmap()
{

}

CImageCachePixmap::~CImageCachePixmap()
{
    if (m_bRemoveImage)
    {
        Clear();
    }
}

bool CImageCachePixmap::IsNull()
{
    return m_pixmap.isNull();
}

bool CImageCachePixmap::LoadImageInfo(const QString & strImagePath, QPixmap & pic)
{
    m_pixmap = pic;
    if (m_size.isValid() && !m_pixmap.isNull())
    {
        m_strImagePath = strImagePath;
        QPixmap pixScale = m_pixmap.scaled(m_size
                                           , (DIT_PROPORTION == m_eType ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio)
                                           , Qt::SmoothTransformation);

        int iImage_h = pixScale.height();
        int iImage_w = pixScale.width();
        if (iImage_w < m_size.width() || iImage_h < m_size.height())
        {
            QPixmap pixtmp(m_size.width(), m_size.height());
            // 配置透明色
            pixtmp.fill(Qt::white);


            QPainter p(&pixtmp);

            int x = 0;
            int y = 0;
            x = (m_size.width() - iImage_w) / 2;
            y = (m_size.height() - iImage_h) / 2;

            p.drawPixmap(x, y, pixScale);
            m_pixmap = pixtmp;
        }
        else
        {
            m_pixmap = pixScale;
        }

    }

    pic = m_pixmap;

    return true;
}

bool CImageCachePixmap::LoadImageInfo(const QString & strImagePath)
{
    QString strFullPath = "";
    if (!GetImagePathConvertor().ConvertToFullPath(strImagePath, strFullPath))
    {
        strFullPath = strImagePath;
    }

    if (!IsValidImage(strFullPath))
    {
        UIKERNEL_INFO("[CImageCachePixmap::LoadImageInfo]File(%s) not Exsit!", strFullPath.toUtf8().data());
        return false;
    }

    bool bResult = LoadPixmapByPath(m_pixmap, strFullPath);
    if (bResult && m_size.isValid())
    {
        m_strImagePath = strFullPath;
        QPixmap pixScale = m_pixmap.scaled(m_size
                                           , (DIT_PROPORTION == m_eType ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio)
                                           , Qt::SmoothTransformation);

        int iImage_h = pixScale.height();
        int iImage_w = pixScale.width();
        if (iImage_w < m_size.width() || iImage_h < m_size.height())
        {
            QPixmap pixtmp(m_size.width(), m_size.height());
            // 配置透明色
            pixtmp.fill(Qt::white);

            QPainter p(&pixtmp);

            int x = 0;
            int y = 0;
            x = (m_size.width() - iImage_w) / 2;
            y = (m_size.height() - iImage_h) / 2;

            p.drawPixmap(x, y, pixScale);
            m_pixmap = pixtmp;
        }
        else
        {
            m_pixmap = pixScale;
        }

    }

    return bResult;
}

QPixmap CImageCachePixmap::GetPixmap() const
{
    return m_pixmap;
}

QSize CImageCachePixmap::GetImageSize()
{
    if (m_pixmap.isNull())
    {
        UIKERNEL_INFO("[CImageCachePixmap::GetImageSize()] pixmap null");
        return QSize(58, 58);
    }
    else
    {
        return m_pixmap.size();
    }
}

bool CImageCachePixmap::operator ==(const CImageCachePixmap & other)
{
    return (m_strImagePath == other.m_strImagePath);
}

bool CImageCachePixmap::operator !=(const CImageCachePixmap & other)
{
    return !(*this == other);
}

CImageCachePixmap & CImageCachePixmap::operator =(const CImageCachePixmap & other)
{
    if (this != &other)
    {
        m_pixmap = other.m_pixmap;
        // copy parent var
        m_strImagePath = other.m_strImagePath;
        m_eType = other.m_eType;
        m_size = other.m_size;
        m_bRemoveImage = other.m_bRemoveImage;
    }

    return *this;
}

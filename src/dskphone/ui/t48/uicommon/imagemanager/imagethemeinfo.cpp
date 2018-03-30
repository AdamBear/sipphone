#include "imagethemeinfo.h"
#include "dsklog/log.h"
#include "imagepathconvertor.h"

ImageThemeInfo::ImageThemeInfo()
{
}

ImageThemeInfo::~ImageThemeInfo()
{
    if (m_bRemoveImage)
    {
        Clear();
    }
}

bool ImageThemeInfo::IsNull()
{
    return m_pixmap.isNull();
}

bool ImageThemeInfo::LoadImageInfo(const QString & strImagePath)
{
    QString strFullPath = "";
    if (!GetImagePathConvertor().ConvertToFullPath(strImagePath, strFullPath))
    {
        strFullPath = strImagePath;
    }

    if (!IsValidImage(strFullPath))
    {
        UIKERNEL_INFO("[ImageThemeInfo::LoadImageInfo] File(%s) not Exsit!", strFullPath.toUtf8().data());
        return false;
    }

    m_strImagePath = strImagePath;
    return LoadPixmapByPath(m_pixmap, strFullPath);
}

bool ImageThemeInfo::LoadImageInfo(const QString & strImagePath, QPixmap & pic)
{
    m_strImagePath = strImagePath;
    m_pixmap = pic;
    return (!m_pixmap.isNull());
}

QPixmap ImageThemeInfo::GetPixmap() const
{
    return m_pixmap;
}

bool ImageThemeInfo::operator ==(const ImageThemeInfo & other)
{
    return (m_strImagePath == other.m_strImagePath);
}

bool ImageThemeInfo::operator !=(const ImageThemeInfo & other)
{
    return !(*this == other);
}

ImageThemeInfo & ImageThemeInfo::operator =(const ImageThemeInfo & other)
{
    if (this != &other)
    {
        m_pixmap = other.m_pixmap;
        m_strImagePath = other.m_strImagePath;
    }

    return *this;
}

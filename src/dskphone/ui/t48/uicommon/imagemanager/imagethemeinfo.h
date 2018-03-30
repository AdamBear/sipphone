#ifndef IMAGETHEMEINFO_H
#define IMAGETHEMEINFO_H

#include "imageinfo.h"

class ImageThemeInfo : public CImageBaseInfo
{
public:
    ImageThemeInfo();

    virtual ~ImageThemeInfo();

    // 是否缓存
    virtual bool IsCache()
    {
        return true;
    }

    // Clear是否删除
    virtual bool IsDeleteWhileClear()
    {
        return false;
    }

    virtual bool IsNull();

    virtual bool LoadImageInfo(const QString & strImagePath);
    virtual bool LoadImageInfo(const QString & strImagePath, QPixmap & pic);

    virtual QPixmap GetPixmap() const;

    bool operator ==(const ImageThemeInfo & other);
    bool operator !=(const ImageThemeInfo & other);
    ImageThemeInfo & operator =(const ImageThemeInfo & other);

private:
    QPixmap         m_pixmap;
};

#endif

#include "modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "imagepoint9manager.h"
#include "imagepathconvertor.h"

IMAGE_RECEIVABLE_TYPE Image_IsImageSizeReceivable(const std::string & strPath)
{
    return GetImageCacheManager().IsReceivableRange(strPath);
}

bool Image_AddImage(const std::string & strPath, IMAGE_TYPE eType, bool bDelBeforeImage /*= false*/)
{
    return GetImageCacheManager().AddImage(strPath, eType, bDelBeforeImage);
}

bool Image_AddImage(const std::string & strPath, IMAGE_TYPE eType, QPixmap & pic)
{
    return GetImageCacheManager().AddImage(strPath, eType, pic, true);
}

bool Image_RemoveImage(const std::string & strPath)
{
    return GetImageCacheManager().RemoveImage(strPath);
}

void Image_ClearImage(bool bClearLync /*= true*/)
{
    GetImageCacheManager().ClearImageResource(bClearLync);
}

#if ADD_CACHE_ALGORITHM
void Image_ClearLRUCache()
{
    GetImageCacheManager().ClearLRUCache();
}

bool Image_IsExistInLRUCache(const std::string & strPath, QPixmap & pic)
{
    return GetImageCacheManager().IsExistInLRUCache(strPath, pic);
}

bool Image_InsertLRUCache(const std::string & strPath, QPixmap & pic)
{
    return GetImageCacheManager().InsertLRUCache(strPath, pic);
}
#endif

void Image_ClearImageByImageType(IMAGE_TYPE eType /*= DIT_NONE*/)
{
    GetImageCacheManager().ClearImageResourceByImageType(eType);
}

QPixmap Image_GetImage(LPCSTR strPath, IMAGE_TYPE eType /*= DIT_THEME*/)
{
    if (NULL == strPath)
    {
        QPixmap picNull;
        return picNull;
    }

    return GetImageCacheManager().FindImage(std::string(strPath), eType);
}

bool Image_GetImage(QPixmap & refPixmap, LPCSTR strPath, IMAGE_TYPE eType /*= DIT_NOCACHE*/)
{
    if (NULL == strPath)
    {
        return false;
    }

    refPixmap = GetImageCacheManager().FindImage(std::string(strPath), eType);
    return refPixmap.isNull() ? false : true;
}

bool Image_LoadTheme(LPCSTR strPath)
{
    return ((NULL == strPath) ? false :
            GetImageCacheManager().LoadTheme(toQString(strPath)));
}

bool Image_UnloadTheme()
{
    GetImageCacheManager().UnloadFreeTheme();
    return true;
}

bool Image_LoadPixmapByPath(QPixmap & refPixmap, const QString & strImagePath)
{
    return CImageBaseInfo::LoadPixmapByPath(refPixmap, strImagePath);
}

void Image_Draw_9_Pix(QPainter & refPainter, LPCSTR lpszFilePath, QRect PaintRect)
{
    GetImagePoint9Manager().PaintDraw9Pix(refPainter, lpszFilePath, PaintRect);
}

bool Image_ConvertToThemePath(const QString & strPath, QString & strFullPath)
{
    return GetImagePathConvertor().ConvertToFullPath(strPath, strFullPath);
}

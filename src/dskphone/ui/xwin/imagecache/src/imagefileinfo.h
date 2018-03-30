#ifndef __IMAGE_FILE_INFO_H__
#define __IMAGE_FILE_INFO_H__

#include "ylstring.h"
#include "ETLLib/ETLLib.hpp"

class CImageFileInfo
{
    enum IMAGE_FORMAT
    {
        BMP_FORMAT,
        JPG_FORMAT,
        GIF_FORMAT,
        PNG_FORMAT,
        NVL_FORMAT
    };

public:
    explicit CImageFileInfo();
    ~CImageFileInfo();

    static bool IsImage(LPCSTR lpszFilePath);

public:
    yl::string getImageFormat(const yl::string& imageUrl);
    long getImageSize(const yl::string& path);
    chSize getImageDimension(const yl::string& path);

private:
    int getImageFormatInt(const yl::string& path);
    long getBMPSize(const yl::string& path);
    long getGIFSize(const yl::string& path);
    long getPNGSize(const yl::string& path);
    long getJPGSize(const yl::string& path);
    chSize getBMPDimension(const yl::string& path);
    chSize getPNGDimension(const yl::string& path);
    chSize getJPGDimension(const yl::string& path);
    chSize getGIFDimension(const yl::string& path);
};

#endif // __IMAGE_FILE_INFO_H__

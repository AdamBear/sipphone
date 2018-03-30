#include "imagepathconvertor.h"

#include "picpath.h"
#include "baseui/t4xpicpath.h"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"

namespace
{
const static char * s_strPrefixTheme1 = THEME1_PREFIX;
const static char * s_strPrefixImages = IMAGES_PREFIX;


const static int s_nConvertPathSize(6);
static const char * s_pConvetTo16PicOrPath[] =
{
    PIC_THEME1_BACKGROUND
    , RESOURCE_PATH "system/wallpaper"
    , USER_DATA_PATH "wallpaper"
    , RESOURCE_PATH "customed/wallpaper"
    , RESOURCE_PATH "system/screensaver"
    , USER_DATA_PATH "screensaver"
};

}

CImagePathConvertor::CImagePathConvertor()
{
}


CImagePathConvertor::~CImagePathConvertor()
{
}

CImagePathConvertor & GetImagePathConvertor()
{
    static CImagePathConvertor sCImagePathConvertor;
    return sCImagePathConvertor;
}

bool CImagePathConvertor::ConvertToFullPath(const QString & strPath, QString & strFullPath)
{
    QString strPicPathRoot = GetPicPathRoot().c_str();

    //如果已经是全路径或不包含主题前缀，直接返回
    if (strPath.lastIndexOf(strPicPathRoot) != -1 || !IsContainThemePath(strPath))
    {
        return false;
    }

    //获取主题路径
    strFullPath = strPicPathRoot + strPath;

    return true;
}

bool CImagePathConvertor::ConvertToThemePath(const QString & strPath, QString & strThemePath)
{
    int nPos = strPath.lastIndexOf(s_strPrefixImages);

    if (-1 == nPos)
    {
        nPos = strPath.lastIndexOf(s_strPrefixTheme1);
    }

    if (nPos != -1)
    {
        strThemePath = strPath.mid(nPos);
        return true;
    }

    return false;
}

bool CImagePathConvertor::IsContainThemePath(const QString & strPath)
{
    return (strPath.lastIndexOf(s_strPrefixTheme1) != -1)
           || (strPath.lastIndexOf(s_strPrefixImages) != -1);
}

bool CImagePathConvertor::IfPixNeedConvetTo16Bit(const QString & strPath)
{
    for (int i = 0; i < s_nConvertPathSize; ++i)
    {
        if (-1 != strPath.indexOf(s_pConvetTo16PicOrPath[i]))
        {
            return true;
        }
    }

    return false;
}

QImage CImagePathConvertor::convetPixmapTo16Bit(QImage tmpImage)
{
    return tmpImage.convertToFormat(QImage::Format_RGB16);
}

yl::string CImagePathConvertor::GetPicPathRoot()
{
    return res_getPathByName(RESOURCE_PATH_THREME);
}

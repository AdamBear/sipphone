#ifndef _IMAGE_PATH_CONVERTOR_H_
#define _IMAGE_PATH_CONVERTOR_H_

#include "interfacedefine.h"
#include "picpath.h"

class CImagePathConvertor
{
public:
    CImagePathConvertor();
    virtual ~CImagePathConvertor();

public:
    bool ConvertToFullPath(const QString & strPath, QString & strFullPath);
    bool ConvertToThemePath(const QString & strPath, QString & strThemePath);

protected:
    bool IsContainThemePath(const QString & strPath);

public:
    bool IfPixNeedConvetTo16Bit(const QString & strPath);
    QImage convetPixmapTo16Bit(QImage tmpPixmap);

    yl::string GetPicPathRoot();
};

CImagePathConvertor & GetImagePathConvertor();

#endif //_IMAGE_PATH_CONVERTOR_H_

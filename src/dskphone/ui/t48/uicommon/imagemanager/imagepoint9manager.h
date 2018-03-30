#ifndef _IMAGE_POINT_9_MANAGER_H_
#define _IMAGE_POINT_9_MANAGER_H_

#include "imageinfo.h"

class CImagePoint9Manager
{
public:
    CImagePoint9Manager();
    virtual ~CImagePoint9Manager();

public:
    //根据文件名识别是否是点九图片
    static bool IsPoint9File(const QString & strFileName);

    //获取点九信息
    void GetPoint9Info(CImageBaseInfo * refStructPixmap);

    //绘制点九图片
    void PaintDraw9Pix(QPainter & refPainter, LPCSTR lpszFilePath, QRect PaintRect);
};

CImagePoint9Manager & GetImagePoint9Manager();

#endif //_IMAGE_POINT_9_MANAGER_H_

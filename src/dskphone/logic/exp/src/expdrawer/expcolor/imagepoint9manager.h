#ifdef _USE_SKIA

#include "expdefine.h"

#ifndef _IMAGE_POINT_9_MANAGER_H_
#define _IMAGE_POINT_9_MANAGER_H_

#include "mypainter.h"

//根据文件名识别是否是点九图片
static bool IsPoint9File(const yl::string & strFileName);

//获取点九信息
void GetPoint9Info(CMyImage & refMyImage);

//绘制点九图片
void PaintDraw9PixByImageInfo(CMyPainter & refPainter, CMyImage & refMyImage, CMyRect PaintRect);

#endif //_IMAGE_POINT_9_MANAGER_H_

#endif

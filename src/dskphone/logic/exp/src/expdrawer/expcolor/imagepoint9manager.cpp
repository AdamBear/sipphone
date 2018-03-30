#include "expdefine.h"

#ifdef _USE_SKIA

#include "imagepoint9manager.h"
#define MAX_PAINT_LENGTH        480

bool IsPoint9File(const yl::string & strFileName)
{
    return (-1 != strFileName.find(".9."));
}

void GetPoint9Info(CMyImage & refMyImage)
{
    //获取点九信息

    //先设置初始值设置
    refMyImage.m_Point9Info.ResetPoint9Info();

    //如果不是点九图，直接返回
    if (!IsPoint9File(refMyImage.GetImagePath()))
    {
        return;
    }

    //透明度
    unsigned char a;

    //获取像素信息
    int nWidth = refMyImage.GetWidth();
    int nHeight = refMyImage.GetHeight();

    //遍历寻找水平拉伸点
    void * data = refMyImage.Getbits();
    for (int i = 0; i < nWidth; ++i)
    {
        a = *(unsigned char *)(data + 3);

        //如果还没有找到拉伸点，说明没有拉伸点或处于拉伸点之前
        if (!refMyImage.m_Point9Info.m_bHStretch)
        {
            //此时出现非透明黑色点，说明该点是拉伸点起始位置
            if (0 != a)
            {
                refMyImage.m_Point9Info.m_bHStretch = true;
                refMyImage.m_Point9Info.m_nHStart = i;
            }
        }
        //如果已经找到拉伸点，说明已经位于拉伸起始位置之后
        else
        {
            //此时出现透明点，说明拉伸位置到此结束
            if (0 == a)
            {
                refMyImage.m_Point9Info.m_nHOver = i;
                //不需要再继续遍历了
                break;
            }
        }

        data += 4;
    }

    //遍历寻找竖直拉伸点
    int nDataCountPerLine = nWidth * 4;
    //重置到起点位置
    data = refMyImage.Getbits();
    for (int i = 0; i < nHeight; ++i)
    {
        a = *(unsigned char *)(data + 3);

        //如果还没有找到拉伸点，说明没有拉伸点或处于拉伸点之前
        if (!refMyImage.m_Point9Info.m_bVStretch)
        {
            //此时出现非透明点，说明该点是拉伸点起始位置
            if (0 != a)
            {
                refMyImage.m_Point9Info.m_bVStretch = true;
                refMyImage.m_Point9Info.m_nVStart = i;
            }
        }
        //如果已经找到拉伸点，说明已经位于拉伸起始位置之后
        else
        {
            //此时出现透明点，说明拉伸位置到此结束
            if (0 == a)
            {
                refMyImage.m_Point9Info.m_nVOver = i;
                //不需要再继续遍历了
                break;
            }
        }

        data += nDataCountPerLine;
    }
}

void PaintDraw9PixByImageInfo(CMyPainter & refPainter, CMyImage & refMyImage, CMyRect PaintRect)
{
    //如果整个绘制区域完全位于屏幕之外，则不予绘制
    if (PaintRect.GetLeft() > MAX_PAINT_LENGTH
            || PaintRect.GetLeft() + PaintRect.GetWidth() < 0
            || PaintRect.GetTop() > MAX_PAINT_LENGTH
            || PaintRect.GetTop() + PaintRect.GetHeight() < 0)
    {
        return;
    }

    //如果不是点九图，则按原有方式绘制
    //如果水平竖直方向都不需要绘制，则按原有方式绘制
    if (!IsPoint9File(refMyImage.GetImagePath())
            || !refMyImage.m_Point9Info.m_bHStretch && !refMyImage.m_Point9Info.m_bVStretch)
    {
        refPainter.DrawImage(PaintRect, refMyImage);
        return;
    }

    //分别计算各区域大小
    int nLeftPartWidth = refMyImage.m_Point9Info.m_nHStart - 1;
    int nRightPartWidth = refMyImage.GetWidth() - refMyImage.m_Point9Info.m_nHOver - 1;
    int nMiddlePartWidth = PaintRect.GetWidth() - nLeftPartWidth - nRightPartWidth;

    int nTopPartHeight = refMyImage.m_Point9Info.m_nVStart - 1;
    int nBottomPartHeight = refMyImage.GetHeight() - refMyImage.m_Point9Info.m_nVOver - 1;
    int nMiddlePartHeight = PaintRect.GetHeight() - nTopPartHeight - nBottomPartHeight;

    //只有横向拉伸
    if (refMyImage.m_Point9Info.m_bHStretch && !refMyImage.m_Point9Info.m_bVStretch)
    {
        //绘制左边
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop()
                    , nLeftPartWidth
                    , PaintRect.GetHeight())
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , 1
                      , nLeftPartWidth
                      , PaintRect.GetHeight()));

        //绘制中间
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth
                    , PaintRect.GetTop()
                    , nMiddlePartWidth
                    , PaintRect.GetHeight())
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHStart
                      , 1
                      , refMyImage.m_Point9Info.m_nHOver - refMyImage.m_Point9Info.m_nHStart
                      , PaintRect.GetHeight()));

        //绘制右边
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth + nMiddlePartWidth
                    , PaintRect.GetTop()
                    , nRightPartWidth
                    , PaintRect.GetHeight())
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHOver
                      , 1
                      , nRightPartWidth
                      , PaintRect.GetHeight()));
    }
    //只有纵向拉伸
    else if (!refMyImage.m_Point9Info.m_bHStretch && refMyImage.m_Point9Info.m_bVStretch)
    {
        //绘制上边
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop()
                    , PaintRect.GetWidth()
                    , nTopPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , 1
                      , PaintRect.GetWidth()
                      , nTopPartHeight));

        //绘制中间
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop() + nTopPartHeight
                    , PaintRect.GetWidth()
                    , nMiddlePartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , refMyImage.m_Point9Info.m_nVStart
                      , PaintRect.GetWidth()
                      , refMyImage.m_Point9Info.m_nVOver - refMyImage.m_Point9Info.m_nVStart));

        //绘制下边
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop() + nTopPartHeight + nMiddlePartHeight
                    , PaintRect.GetWidth()
                    , nBottomPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , refMyImage.m_Point9Info.m_nVOver
                      , PaintRect.GetWidth()
                      , nBottomPartHeight));
    }
    else
    {
        //绘制左上
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop()
                    , nLeftPartWidth
                    , nTopPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , 1
                      , nLeftPartWidth
                      , nTopPartHeight));

        //绘制中上
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth
                    , PaintRect.GetTop()
                    , nMiddlePartWidth
                    , nTopPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHStart
                      , 1
                      , refMyImage.m_Point9Info.m_nHOver - refMyImage.m_Point9Info.m_nHStart
                      , nTopPartHeight));

        //绘制右上
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth + nMiddlePartWidth
                    , PaintRect.GetTop()
                    , nRightPartWidth
                    , nTopPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHOver
                      , 1
                      , nRightPartWidth
                      , nTopPartHeight));

        //绘制左中
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop() + nTopPartHeight
                    , nLeftPartWidth
                    , nMiddlePartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , refMyImage.m_Point9Info.m_nVStart
                      , nLeftPartWidth
                      , refMyImage.m_Point9Info.m_nVOver - refMyImage.m_Point9Info.m_nVStart));

        //绘制中间
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth
                    , PaintRect.GetTop() + nTopPartHeight
                    , nMiddlePartWidth
                    , nMiddlePartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHStart
                      , refMyImage.m_Point9Info.m_nVStart
                      , refMyImage.m_Point9Info.m_nHOver - refMyImage.m_Point9Info.m_nHStart
                      , refMyImage.m_Point9Info.m_nVOver - refMyImage.m_Point9Info.m_nVStart));

        //绘制右中
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth + nMiddlePartWidth
                    , PaintRect.GetTop() + nTopPartHeight
                    , nRightPartWidth
                    , nMiddlePartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHOver
                      , refMyImage.m_Point9Info.m_nVStart
                      , nRightPartWidth
                      , refMyImage.m_Point9Info.m_nVOver - refMyImage.m_Point9Info.m_nVStart));

        //绘制左下
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft()
                    , PaintRect.GetTop() + nTopPartHeight + nMiddlePartHeight
                    , nLeftPartWidth
                    , nBottomPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(1
                      , refMyImage.m_Point9Info.m_nVOver
                      , nLeftPartWidth
                      , nBottomPartHeight));

        //绘制中下
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth
                    , PaintRect.GetTop() + nTopPartHeight + nMiddlePartHeight
                    , nMiddlePartWidth
                    , nBottomPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHStart
                      , refMyImage.m_Point9Info.m_nVOver
                      , refMyImage.m_Point9Info.m_nHOver - refMyImage.m_Point9Info.m_nHStart
                      , nBottomPartHeight));

        //绘制右下
        refPainter.DrawImage(
            //目标位置
            CMyRect(PaintRect.GetLeft() + nLeftPartWidth + nMiddlePartWidth
                    , PaintRect.GetTop() + nTopPartHeight + nMiddlePartHeight
                    , nRightPartWidth
                    , nBottomPartHeight)
            //图片
            , refMyImage
            //源位置
            , CMyRect(refMyImage.m_Point9Info.m_nHOver
                      , refMyImage.m_Point9Info.m_nVOver
                      , nRightPartWidth
                      , nBottomPartHeight));
    }
}

#endif

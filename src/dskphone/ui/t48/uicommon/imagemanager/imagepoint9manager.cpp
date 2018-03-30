#include "imagepoint9manager.h"
#include "imagecachemanager.h"
#include "uimanager/rectdefine.h"

CImagePoint9Manager::CImagePoint9Manager()
{
}


CImagePoint9Manager::~CImagePoint9Manager()
{
}

CImagePoint9Manager & GetImagePoint9Manager()
{
    static CImagePoint9Manager sCImagePoint9Manager;
    return sCImagePoint9Manager;
}

bool CImagePoint9Manager::IsPoint9File(const QString & strFileName)
{
    return (strFileName.contains(".9."));
}

void CImagePoint9Manager::GetPoint9Info(CImageBaseInfo * refStructPixmap)
{
    if (NULL == refStructPixmap)
    {
        return;
    }

    //获取点九信息

    //先设置初始值设置
    refStructPixmap->m_Point9Info.ResetPoint9Info();

    //如果不是点九图，直接返回
    if (!IsPoint9File(refStructPixmap->GetImagePath()))
    {
        return;
    }

    //透明度
    unsigned char a;

    //获取像素信息
    QImage tmpImage = refStructPixmap->GetPixmap().toImage();

    int nWidth = tmpImage.width();
    int nHeight = tmpImage.height();

    //遍历寻找水平拉伸点
    unsigned char * data = tmpImage.bits();
    for (int i = 0; i < nWidth; ++i)
    {
        a = *(data + 3);

        //如果还没有找到拉伸点，说明没有拉伸点或处于拉伸点之前
        if (!refStructPixmap->m_Point9Info.m_bHStretch)
        {
            //此时出现非透明黑色点，说明该点是拉伸点起始位置
            if (0 != a)
            {
                refStructPixmap->m_Point9Info.m_bHStretch = true;
                refStructPixmap->m_Point9Info.m_nHStart = i;
            }
        }
        //如果已经找到拉伸点，说明已经位于拉伸起始位置之后
        else
        {
            //此时出现透明点，说明拉伸位置到此结束
            if (0 == a)
            {
                refStructPixmap->m_Point9Info.m_nHOver = i;
                //不需要再继续遍历了
                break;
            }
        }

        data += 4;
    }

    //遍历寻找竖直拉伸点
    int nDataCountPerLine = nWidth * 4;
    //重置到起点位置
    data = tmpImage.bits();
    for (int i = 0; i < nHeight; ++i)
    {
        a = *(data + 3);

        //如果还没有找到拉伸点，说明没有拉伸点或处于拉伸点之前
        if (!refStructPixmap->m_Point9Info.m_bVStretch)
        {
            //此时出现非透明点，说明该点是拉伸点起始位置
            if (0 != a)
            {
                refStructPixmap->m_Point9Info.m_bVStretch = true;
                refStructPixmap->m_Point9Info.m_nVStart = i;
            }
        }
        //如果已经找到拉伸点，说明已经位于拉伸起始位置之后
        else
        {
            //此时出现透明点，说明拉伸位置到此结束
            if (0 == a)
            {
                refStructPixmap->m_Point9Info.m_nVOver = i;
                //不需要再继续遍历了
                break;
            }
        }

        data += nDataCountPerLine;
    }
}

void CImagePoint9Manager::PaintDraw9Pix(QPainter & refPainter, LPCSTR lpszFilePath, QRect PaintRect)
{
    CImageBaseInfo * pImageBaseInfo = GetImageCacheManager().FindImageInfo(lpszFilePath);
    if (NULL == pImageBaseInfo)
    {
        //没有缓存的图片，认为不是主题图片，不予绘制点九
        return;
    }

    //如果整个绘制区域完全位于屏幕之外，则不予绘制
    if (PaintRect.left() > PHONE_WIDTH
            || PaintRect.left() + PaintRect.width() < 0
            || PaintRect.top() > PHONE_HEIGHT
            || PaintRect.top() + PaintRect.height() < 0)
    {
        return;
    }

    QPixmap pm = pImageBaseInfo->GetPixmap();

    //如果不是点九图，则按原有方式绘制
    //如果水平竖直方向都不需要绘制，则按原有方式绘制
    if (!IsPoint9File(lpszFilePath)
            || !pImageBaseInfo->m_Point9Info.m_bHStretch && !pImageBaseInfo->m_Point9Info.m_bVStretch)
    {
        refPainter.drawPixmap(PaintRect, pm);
        return;
    }

    //分别计算各区域大小
    int nLeftPartWidth = pImageBaseInfo->m_Point9Info.m_nHStart - 1;
    int nRightPartWidth = pm.width() - pImageBaseInfo->m_Point9Info.m_nHOver - 1;
    int nMiddlePartWidth = PaintRect.width() - nLeftPartWidth - nRightPartWidth;

    int nTopPartHeight = pImageBaseInfo->m_Point9Info.m_nVStart - 1;
    int nBottomPartHeight = pm.height() - pImageBaseInfo->m_Point9Info.m_nVOver - 1;
    int nMiddlePartHeight = PaintRect.height() - nTopPartHeight - nBottomPartHeight;

    //只有横向拉伸
    if (pImageBaseInfo->m_Point9Info.m_bHStretch && !pImageBaseInfo->m_Point9Info.m_bVStretch)
    {
        //绘制左边
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top()
            , nLeftPartWidth
            , PaintRect.height()
            //图片
            , pm
            //源位置
            , 1
            , 1
            , nLeftPartWidth
            , PaintRect.height());

        //绘制中间
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth
            , PaintRect.top()
            , nMiddlePartWidth
            , PaintRect.height()
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHStart
            , 1
            , pImageBaseInfo->m_Point9Info.m_nHOver - pImageBaseInfo->m_Point9Info.m_nHStart
            , PaintRect.height());

        //绘制右边
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth + nMiddlePartWidth
            , PaintRect.top()
            , nRightPartWidth
            , PaintRect.height()
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHOver
            , 1
            , nRightPartWidth
            , PaintRect.height());
    }
    //只有纵向拉伸
    else if (!pImageBaseInfo->m_Point9Info.m_bHStretch && pImageBaseInfo->m_Point9Info.m_bVStretch)
    {
        //绘制上边
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top()
            , PaintRect.width()
            , nTopPartHeight
            //图片
            , pm
            //源位置
            , 1
            , 1
            , PaintRect.width()
            , nTopPartHeight);

        //绘制中间
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top() + nTopPartHeight
            , PaintRect.width()
            , nMiddlePartHeight
            //图片
            , pm
            //源位置
            , 1
            , pImageBaseInfo->m_Point9Info.m_nVStart
            , PaintRect.width()
            , pImageBaseInfo->m_Point9Info.m_nVOver - pImageBaseInfo->m_Point9Info.m_nVStart);

        //绘制下边
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top() + nTopPartHeight + nMiddlePartHeight
            , PaintRect.width()
            , nBottomPartHeight
            //图片
            , pm
            //源位置
            , 1
            , pImageBaseInfo->m_Point9Info.m_nVOver
            , PaintRect.width()
            , nBottomPartHeight);
    }
    else
    {
        //绘制左上
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top()
            , nLeftPartWidth
            , nTopPartHeight
            //图片
            , pm
            //源位置
            , 1
            , 1
            , nLeftPartWidth
            , nTopPartHeight);

        //绘制中上
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth
            , PaintRect.top()
            , nMiddlePartWidth
            , nTopPartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHStart
            , 1
            , pImageBaseInfo->m_Point9Info.m_nHOver - pImageBaseInfo->m_Point9Info.m_nHStart
            , nTopPartHeight);

        //绘制右上
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth + nMiddlePartWidth
            , PaintRect.top()
            , nRightPartWidth
            , nTopPartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHOver
            , 1
            , nRightPartWidth
            , nTopPartHeight);

        //绘制左中
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top() + nTopPartHeight
            , nLeftPartWidth
            , nMiddlePartHeight
            //图片
            , pm
            //源位置
            , 1
            , pImageBaseInfo->m_Point9Info.m_nVStart
            , nLeftPartWidth
            , pImageBaseInfo->m_Point9Info.m_nVOver - pImageBaseInfo->m_Point9Info.m_nVStart);

        //绘制中间
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth
            , PaintRect.top() + nTopPartHeight
            , nMiddlePartWidth
            , nMiddlePartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHStart
            , pImageBaseInfo->m_Point9Info.m_nVStart
            , pImageBaseInfo->m_Point9Info.m_nHOver - pImageBaseInfo->m_Point9Info.m_nHStart
            , pImageBaseInfo->m_Point9Info.m_nVOver - pImageBaseInfo->m_Point9Info.m_nVStart);

        //绘制右中
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth + nMiddlePartWidth
            , PaintRect.top() + nTopPartHeight
            , nRightPartWidth
            , nMiddlePartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHOver
            , pImageBaseInfo->m_Point9Info.m_nVStart
            , nRightPartWidth
            , pImageBaseInfo->m_Point9Info.m_nVOver - pImageBaseInfo->m_Point9Info.m_nVStart);

        //绘制左下
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left()
            , PaintRect.top() + nTopPartHeight + nMiddlePartHeight
            , nLeftPartWidth
            , nBottomPartHeight
            //图片
            , pm
            //源位置
            , 1
            , pImageBaseInfo->m_Point9Info.m_nVOver
            , nLeftPartWidth
            , nBottomPartHeight);

        //绘制中下
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth
            , PaintRect.top() + nTopPartHeight + nMiddlePartHeight
            , nMiddlePartWidth
            , nBottomPartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHStart
            , pImageBaseInfo->m_Point9Info.m_nVOver
            , pImageBaseInfo->m_Point9Info.m_nHOver - pImageBaseInfo->m_Point9Info.m_nHStart
            , nBottomPartHeight);

        //绘制右下
        refPainter.drawPixmap(
            //目标位置
            PaintRect.left() + nLeftPartWidth + nMiddlePartWidth
            , PaintRect.top() + nTopPartHeight + nMiddlePartHeight
            , nRightPartWidth
            , nBottomPartHeight
            //图片
            , pm
            //源位置
            , pImageBaseInfo->m_Point9Info.m_nHOver
            , pImageBaseInfo->m_Point9Info.m_nVOver
            , nRightPartWidth
            , nBottomPartHeight);
    }
}


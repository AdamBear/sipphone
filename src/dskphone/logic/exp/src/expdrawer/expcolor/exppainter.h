#ifndef _EXP_PAINTER_H
#define _EXP_PAINTER_H

#include "expdefine.h"
#include "exppiccache.h"

#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
#include "lrucache.hpp"

typedef lru::Cache<yl::string, CMyImage>     ExpImageCache;
#endif


class EXPPainter
{
public:
    EXPPainter();
    virtual ~EXPPainter();

public:
    virtual void Init();
    virtual void Uninit();

public:
    virtual CMyImage & GetCanvas()
    {
        return m_imgCanvas;
    }

    //重新加载背景图，每次绘制前需要先调用
    virtual bool ReloadCanvas();
    virtual bool ReloadCanvas(int nRow, int nCol, bool bLoadFull = false);

public:
    //设置壁纸背景和按钮背景
    virtual bool SetBackGround(const yl::string & strBackGround, int nTransparentLevel);

    virtual void PaintBtn(int nRow, int nCol, bool bDown, int nDsskeyBgType
                          , bool bPaintIcon, bool bPaintText
                          , int nIconType, const yl::string & strText
                          , const yl::string & strIcon, int nStatus, int nFlag = DSSKEY_ALIGN_NORMOL);

protected:
    virtual void PaintIcon(int nRow, int nCol, int nIconType, int nFlag = DSSKEY_ALIGN_NORMOL);
    virtual void PaintIcon(int nRow, int nCol, const yl::string & strIcon, int nFlag = DSSKEY_ALIGN_NORMOL);
    virtual void PaintStatus(int nRow, int nCol, int nStatus, int nFlag = DSSKEY_ALIGN_NORMOL);
    virtual void PaintText(int nRow, int nCol, const yl::string & strText, int nFlag = DSSKEY_ALIGN_NORMOL);

#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
    void GetImageByPath(CMyImage & objTargetImage, const yl::string & strPath);
#endif

public:
    //测试模式使用
    virtual void EXPFillWithColor(const EXP_FILL_COLOR & objExpColor);
    virtual void EXPFillWithColor(const EXP_FILL_COLOR & objExpColor, CMyPainter & painterCanvas);
    virtual void EXPFillWithPicture(const yl::string & pStrPicture);
    virtual void EXPFillWithPicture(const yl::string & pStrPicture, CMyPainter & painterCanvas);
    virtual void EXPDrawText(const yl::string & pStrText);

protected:
    EXPPicCache         m_PicCache;

    CMyImage                    m_imgCanvas;
    CMyPainter                  m_painterCanvas;

#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
    ExpImageCache m_ExpImageCache;
#endif
};

#endif //_EXP_PAINTER_H

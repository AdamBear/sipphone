#ifndef _EXP_PIC_CACHE_H
#define _EXP_PIC_CACHE_H

#include "mypainter.h"
#include "ylstl/ylhashmap.h"

typedef YLHashMap<int, CMyImage>                            MapImageCache;
typedef MapImageCache::iterator                 MapImageCacheIter;

class EXPPicCache
{
public:
    EXPPicCache();
    virtual ~EXPPicCache();

public:
    virtual void Init();
    virtual void Uninit();

    //设置壁纸背景和按钮背景
    //初始化绘制缓存中的背景，即在所有固定位置上绘制好按钮背景，以供后续画布加载使用
    virtual bool SetBackGround(const yl::string & strBackGround, int nTransparentLevel);

    //重新加载画布，即将缓存中的图片拷贝覆盖到画布中
    virtual void ReloadCanvas(CMyPainter & painterCanvas, CMyRect rectCopyPart = CMyRect());

    virtual CMyImage & GetIconImage(int nIconType);
    virtual CMyImage & GetStatusImage(int nStatus, bool bNewIcon = false);

    virtual void PaintDownButton(CMyPainter & painterCanvas, CMyRect rectButton);
    void PaintUsingButton(CMyPainter & painterCanvas, CMyRect rectButton);
    void PaintHeightLightButton(CMyPainter & painterCanvas, CMyRect rectButton);

protected:
    virtual void LoadIconImage();

    yl::string MapIconString(int nIconType);
    yl::string MapStatusString(int nStatus, bool bNewIcon = false);
    yl::string MapDssBG(int nTransparentLevel, bool bLeft);

protected:
    MapImageCache       m_imgIconCache;
    MapImageCache       m_imgStatusCache;
    MapImageCache       m_imgNewStatusCache;

    yl::string      m_strBackGround;
    int             m_nTransparentLevel;

    CMyImage        m_imgBackGround;
    CMyPainter      m_painterBackGround;

    CMyImageInfo        *       m_pButtonLeftBG;
    CMyImageInfo        *       m_pButtonRightBG;

    CMyImage        m_imgBtnDown;
    CMyImage        m_imgBtnHeight;
    CMyImage        m_imgBtnUse;
};

#endif //_EXP_PIC_CACHE_H

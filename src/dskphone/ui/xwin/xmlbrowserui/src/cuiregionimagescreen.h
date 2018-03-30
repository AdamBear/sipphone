#ifndef CUI_REGION_IMAGE_SCREEN_H
#define CUI_REGION_IMAGE_SCREEN_H

#include "cuiregionbase.h"

// 前向声明
class AutoResizeLab;
class CScrollPage;

#if XML_BROWSER_SUPPORT_UI

// CUIRegionImage类
class CUIRegionImageScreen : public CUIRegionBase, public xTimerHandler
{

public:
    CUIRegionImageScreen();
    virtual ~CUIRegionImageScreen();

    // 根据数据更新界面
    virtual void Update(void * pData);
    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);

    virtual void loadChildrenElements(xml_node & node);

    // 获取页面的默认softkey数据
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

private:
    // 设置图片的显示模式
    void SetImageMode(IMAGE_MODE eMode);

    // 绘制图片
    void DrawImage(IMAGE_MODE eMode, ImageItem * pImageItem);

    // 获取图片对齐方式
    int GetAlignByImageAlign(IMAGE_VERTICAL_ALIGN eVertical, IMAGE_HORIZONTAL_ALIGN eHorizontal);

    //延时提示
    void OnDelayMessageBox(xThreadTimer* timer, LPVOID pExtraData);

    // 加载图片
    BOOL LoadImage(xPixmap & pmImage, const char * lpszImagePath, UINT uWidth, UINT uHeight,
                   int & sizeImage);

protected:
    xImageView   *  m_pImage; // 显示Title的lab
    IMAGE_MODE      m_eImageMode;
    xThreadTimer    m_timerDelayMessageBox;
    yl::string      m_strNoteContent;
};

#endif

#endif // CUI_REGION_IMAGE_SCREEN_H

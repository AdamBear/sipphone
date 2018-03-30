#ifndef CDLGPREVIEWPICTUREFULLSCREEN_H
#define CDLGPREVIEWPICTUREFULLSCREEN_H

#include "dlgdisplaywallpaper.h"
#include "uimanager/basedialog.h"

class CDlgPreviewPictureFullScreen : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgPreviewPictureFullScreen();
    ~CDlgPreviewPictureFullScreen();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 设置小窗口数据
    //!!!!!没有地方调用
    virtual void SetData(void * pData = NULL);

    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    void OnSoftKeyClick(qSoftButton * pSoftBtn);

    //设置图片列表窗口指针，用于预览窗口跟列表窗口交互
    void SetDisplay(CDlgDisplayWallpaper * val);

    //图片预览
    void Preview();

    //事件过滤，响应按键和鼠标事件
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);


private:
    CDlgDisplayWallpaper * m_display;
    QLabel * m_labPreview;
};

#endif

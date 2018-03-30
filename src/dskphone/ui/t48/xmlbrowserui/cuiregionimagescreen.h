#ifndef CUI_REGION_IMAGE_SCREEN_H
#define CUI_REGION_IMAGE_SCREEN_H

#include <QtGui>
#include "cuiregionbase.h"

// 前向声明
class AutoResizeLab;
class CScrollPage;

// CUIRegionImage类
class CUIRegionImageScreen : public CUIRegionBase
{
    Q_OBJECT

public:
    CUIRegionImageScreen(QWidget * parent = 0, Qt::WFlags flags = 0);
    virtual ~CUIRegionImageScreen();

    // 根据数据更新界面
    virtual void Update(void * pData);
    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

private:
    // 设置图片的显示模式
    void SetImageMode(IMAGE_MODE eMode);

    // 绘制图片
    void DrawImage(IMAGE_MODE eMode, ImageItem * pImageItem);

    // 计算图片的位置
    QRect ImageLabGeometry(const QSize & imageSize, ImageItem * pImageItem);

private slots:
    //延时提示
    void OnDelayMessageBox();

protected:
    QLabel      m_labImage; // 显示Title的lab
    IMAGE_MODE  m_imageMode;

    QTimer m_timerDelayMessageBox;

    QString m_strNoteContent;
};

#endif // CUI_REGION_IMAGE_SCREEN_H

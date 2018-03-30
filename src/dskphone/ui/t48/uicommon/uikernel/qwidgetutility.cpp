#include "qwidgetutility.h"
#include "imagemanager/modimagemanager.h"

qWidgetUtility::qWidgetUtility(void)
{
}

qWidgetUtility::~qWidgetUtility(void)
{
}

void qWidgetUtility::setFontStyle(QWidget * pObject, int nPointSize,
                                  int nWeight/* = QFont::Normal*/, bool bBoldType/* = true*/)
{
    if (pObject == NULL)
    {
        return;
    }

    // 设置字体大小
    QFont font = pObject->font();

    if (nPointSize >= 0)
    {
        font.setPointSize(nPointSize);
    }

    // 设置字体是否加粗
    font.setBold(bBoldType);

    // 设置字体宽度
    if (nWeight >= 0)
    {
        font.setWeight(nWeight);
    }

    ////---font.setFamily(QString::fromUtf8(LETTER_FONT));
    pObject->setFont(font);
}


// 设置widget背景
void qWidgetUtility::SetBackGround(QWidget * pWgt, LPCSTR picPath, bool bScale/* = false*/)
{
    SetBackGround(pWgt, THEME_GET_BMP(picPath), bScale);
}

// 设置widget背景
void qWidgetUtility::SetBackGround(QWidget * pWgt, const QPixmap & pic, bool bScale/* = false*/)
{
    if (pWgt != NULL)
    {
        if (!pic.isNull() && bScale)
        {
            pic.scaled(pWgt->size());
        }

        pWgt->setAutoFillBackground(true);
        QPalette palette = pWgt->palette();
        palette.setBrush(QPalette::Window, QBrush(pic));
        pWgt->setPalette(palette);
    }
}

//映射区域至全局坐标
QRect qWidgetUtility::qMapRectToGlobal(QWidget * pWidget, QRect rcSrc)
{
    QRect rcWidget = rcSrc;

    if (pWidget != NULL)
    {
        QPoint ptTopLeft = pWidget->mapToGlobal(QPoint(rcSrc.left(), rcSrc.top()));
        QPoint ptBottomRight = pWidget->mapToGlobal(QPoint(rcSrc.right(), rcSrc.bottom()));
        rcWidget.setTopLeft(ptTopLeft);
        rcWidget.setBottomRight(ptBottomRight);
    }

    if (!rcWidget.isValid())
    {
        rcWidget.setRect(0, 0, 0, 0);
    }

    return rcWidget;
}

//从全局坐标映射至控件上的位置
QRect qWidgetUtility::qMapRectFromGlobal(QWidget * pWidget, QRect rcSrc)
{
    QRect rcGlobal = rcSrc;

    if (pWidget != NULL)
    {
        QPoint ptTopLeft = pWidget->mapFromGlobal(QPoint(rcSrc.left(), rcSrc.top()));
        QPoint ptBottomRight = pWidget->mapFromGlobal(QPoint(rcSrc.right(), rcSrc.bottom()));
        rcGlobal.setTopLeft(ptTopLeft);
        rcGlobal.setBottomRight(ptBottomRight);
    }

    if (!rcGlobal.isValid())
    {
        rcGlobal.setRect(0, 0, 0, 0);
    }

    return rcGlobal;
}

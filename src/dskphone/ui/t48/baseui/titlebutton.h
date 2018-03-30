
#ifndef __TITLE_BUTTON_H__
#define __TITLE_BUTTON_H__

#include "uikernel/buttonwrapper.h"
#if IF_BUG_26252
#include "uikernel/qscrolltextlabel.h"
#endif

class CTitleButton : public CButtonWrapper
{
public:
    explicit CTitleButton(QWidget * pWidget);
    virtual ~CTitleButton(void);

public:
    void SetDefaultPos();
    void SetIconTop(int nTop);
    void SetTextTop(int nTop);

    void SetIconRect(const QRect & rcIcon);
    void SetIconRect(int nX, int nY, int nWidth, int nHeight);
    void SetTextRect(const QRect & rcText);
    void SetTextRect(int nX, int nY, int nWidth, int nHeight);

    // 画按钮图标
    void PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon, const char * pszIcon);

    // 画按钮文字
    void PaintText(QStylePainter & stylePainter, int nTextAlign, const QRect & rcText,
                   const QString & strText);

    // 画纵向按钮(Icon + 文字)
    void PaintVButton(QStylePainter & stylePainter, const char * pszSelectBg, const char * pszIcon,
                      const QString & strText);

    // 画横向按钮(Icon + 文字)
    void PaintHButton(QStylePainter & stylePainter, const char * pszSelectBg, const char * pszIcon,
                      const QString & strText);
#if IF_BUG_26252
    void SetTextScroll(bool bScroll);
#endif
#ifdef IF_ENABLE_TESTABILITY
public:
    const QString & GetText();
    QString m_strText;
#endif
protected:
    QRect m_rcIcon;
    QRect m_rcText;
#if IF_BUG_26252
    qScrollTextLabel  * m_pTextLabel;  // 显示滚动文本的控件;
    bool                m_bTextScroll; // 是否滚动
#endif
};

#endif

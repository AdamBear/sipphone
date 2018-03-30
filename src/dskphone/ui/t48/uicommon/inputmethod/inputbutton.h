#pragma once

#include <QObject>
#include "uikernel/buttonwrapper.h"

#include <QStylePainter>
#include <QStyleOption>
#include "imagemanager/modimagemanager.h"

class CInputButton: public QObject
{
public:
    CInputButton(QWidget* v_parent = 0);
    virtual ~CInputButton(void);

public:
    inline void SetParent(QWidget* pParent)
    {
        m_pWidget = pParent;
        setParent(pParent);
    }

    virtual void paintInputButton(QStylePainter& stylePainter);

    // 设置按钮的三态图片功能
    void SetNormalText(const QString &qstrText);
    void SetPressText(const QString &qstrText);

    void setNormalTextColor(const QColor& clr);
    void SetPressTextColor(const QColor& clr);

    void SetNormalBackGround(const QColor& clr);
    void SetPressBackGround(const QColor& clr);

    QColor GetNormalBackGround()
    {
        return m_qColorNormalBackGround;
    }
    QColor GetPressBackGround()
    {
        return m_qColorPressBackGround;
    }
#ifndef IF_LYNC
    void SetPressIcon(QString qstrPressIcon);
    void SetNormalIcon(QString qstrPressIcon);

    QString& GetButtonPressIcon()
    {
        return m_strPressIcon;
    }
    QString& GetButtonNormalIcon()
    {
        return m_strNormalcon;
    }
#else
    void SetPressIcon(QString qstrPressIcon);
    void SetPressIconQpixmap(QPixmap pixmap);
#endif

    inline void setIconPic(const QString& icon_pic)
    {
        m_strNormalIconPath = icon_pic;
    }

    inline void setIconPicAlign(int nAlign)
    {
        m_nIconAlign = nAlign;
    }

    inline void setIntBtnAction(const int& iBtnAction)
    {
        m_nBtnAction = iBtnAction;
    }
    inline int getIntBtnAction() const
    {
        return m_nBtnAction;
    }

    virtual bool InputButtonMousePress(const QPoint& mousePressPoint);
    virtual bool InputButtonMouseRelease(const QPoint& mouseReleasePoint);

    virtual void ResetInputButtonDown();
    void RepaintInputButton();

    inline void SetIsResponePress(bool bIsResponePress)
    {
        m_bIsResponePress = bIsResponePress;
    }
    inline bool GetIsResponePress()
    {
        return m_bIsResponePress;
    }

protected:
    QString m_qstrNormalText;
    QString m_qstrPressText;
    QColor m_qColorNormalText;
    QColor m_qColorPressText;

    QColor m_qColorNormalBackGround;
    QColor m_qColorPressBackGround;

#ifndef IF_LYNC
    QString m_strNormalcon;
    QString m_strPressIcon;
#else
    QPixmap m_pmPressIcon;
    QPixmap m_pmNormalIcon;
    QString m_qstrPressIcon;
#endif

public:
    QPixmap m_pmNull;

public:
    QRect       m_rctButton;
    QRect       m_rctGlobalButton;
    QString     m_strNormalIconPath;
    int         m_nBtnAction;
    int         m_nIconAlign;

protected:
    QWidget* m_pWidget;
    //记录当前按钮按下状态
    bool            m_bInputButtonDown;
    //记录是响应按下还是抬起
    bool            m_bIsResponePress;
};

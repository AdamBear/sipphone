/*
 * class qUIPushButton
 *
 * 按钮类, 自画样式, 避免使用样式表
 * Created by: Roland
 * Created at: 2011-3-22
 */

#ifndef QUIPUSHBUTTON_H
#define QUIPUSHBUTTON_H

#include <ETLLib/ETLLib.hpp>
#include <QPushButton>
#include <QColor>

class qUIPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit qUIPushButton(QWidget * parent = 0);
    ~qUIPushButton();

    // 设置按钮的三态图片功能
    inline void set3StatePic(const QString & normal_pic,
                             const QString & enable_pic,
                             const QString & disable_pic);

    // 设置按钮文本对齐方式
    inline void setTextAlign(/*Qt::AlignmentFlag*/ int align/* = 0*/);

    // 设置按钮文本颜色
    void setTextColor(const QColor & clr);

    // 设置按钮透明
    void setTransparent(bool is_transparent = false);

    // 设置按钮上的ICON的图片路径
    inline void setIconPic(const QString & icon_pic);

    inline QString getIconPic() const;

    //取得三态图标
    inline QString getNormalPic() const;

    inline QString getEnablePic() const;

    inline QString getDisalbePic() const;

    // 设置按钮上的ICON图片位置
    inline void setIconPicAlign(int nAlign);

    //设置ICON left偏移量
    inline void setIconLeftOffSet(int offset);
    //获取ICON left偏移量
    inline int getIconLeftOffSet() const;

    //设置ICON Top偏移量
    inline void setIconTopOffSet(int offset);
    //获取ICON Top偏移量
    inline int getIconTopOffSet() const;

    //文字x方向偏移值
    inline void setTextXOffSet(int offset);
    inline int getTextXOffSet() const;

    //文字y方向偏移值
    inline void setTextYOffSet(int offset);
    inline int getTextYOffSet() const;

    //设置按钮的编号
    inline void setButtonID(int iId);
    inline int getButtonID() const;

    // 设置Button的action
    inline void setBtnAction(const QString & strBtnAction);

    //返回Button的Action
    inline QString getBtnAction()  const;

    inline void SetMask(const QString & strPicPath);

    inline Qt::AlignmentFlag getIconAlign() const;

    inline Qt::AlignmentFlag getTextAlign() const;
    bool eventFilter(QObject * pObj, QEvent * pEvent);

    //模拟松开按钮时要做的额外操作
    virtual void DoAfterRelease()
    {
        //
    }

signals:
    void pressed(const QString & strBtnAction); // 按下发出的信号

    void released(const QString & strBtnAction); // 抬起发出的信号

    void clicked(const QString & strBtnAction); // 点击发出的信号

    void longPress(const QString & strBtnAction); // 长按发出的信号

protected:
    void paintEvent(QPaintEvent * pe);

private slots:
    void OnButtonPressed();

    void OnButtonReleased();

    void OnButtonClicked();

protected:
    QString m_strNormalPic; // Button常态时显示的背景图片
    QString m_strEnablePic;  // Button被点击时显示的背景图片
    QString m_strDisalbePic; // Button非激活状态下点击的背景图片
    QString m_strIconPic;    // Button上小图标的图片
    QString m_strMaskPic;   // Button点击时候的遮罩图片
    QString m_strBtnAction;  // Button的Action在Button点击的时候随信号发出,用来标示不同的Btn操作
    Qt::AlignmentFlag m_eIconAlign;  // PushButton上ICON对齐方式
    Qt::AlignmentFlag m_eAlign;  // PushButton上文本对齐方式
    QColor m_clrText;  // PushButton上文本的颜色
    bool m_isTransparent;
    int m_iIconXOffSet; //Icon x偏移值
    int m_iIconYOffSet; //Icon y偏移值
    int m_iTextXOffSet; //文字x偏移值
    int m_iTextYOffSet; //文字y偏移值
    int m_iBtnID; //按钮编号，默认为-1
};

inline void qUIPushButton::set3StatePic(const QString & normal_pic,
                                        const QString & enable_pic,
                                        const QString & disable_pic)
{
    m_strNormalPic = normal_pic;
    m_strEnablePic = enable_pic;
    m_strDisalbePic = disable_pic;
}

inline void qUIPushButton::setTextAlign(/*Qt::AlignmentFlag */int align/* = 0*/)
{
    if (align <= 0)
    {
        return;
    }

    if (align != m_eAlign)
    {
        // 显示转换为可用类型
        m_eAlign = static_cast<Qt::AlignmentFlag>(align);
    }
}

inline void qUIPushButton::setIconPic(const QString & icon_pic)
{
    if (icon_pic != m_strIconPic)
    {
        m_strIconPic = icon_pic;
    }
}

inline QString qUIPushButton::getIconPic() const
{
    return m_strIconPic;
}


//取得三态图标
inline QString qUIPushButton::getNormalPic() const
{
    return m_strNormalPic;
}

inline QString qUIPushButton::getEnablePic() const
{
    return m_strEnablePic;
}

inline QString qUIPushButton::getDisalbePic() const
{
    return m_strDisalbePic;
}

// 设置按钮上的ICON图片位置
inline void qUIPushButton::setIconPicAlign(int nAlign)
{
    if (nAlign <= 0)
    {
        return;
    }

    if (m_eIconAlign != nAlign)
    {
        // 显示转换为可用类型
        m_eIconAlign = static_cast<Qt::AlignmentFlag>(nAlign);
    }
}

//设置偏移量
inline void qUIPushButton::setIconLeftOffSet(int offset)
{
    m_iIconXOffSet = offset;
}

//获取偏移量
inline int qUIPushButton::getIconLeftOffSet() const
{
    return m_iIconXOffSet;
}

//设置ICON Top偏移量
inline void qUIPushButton::setIconTopOffSet(int offset)
{
    m_iIconYOffSet = offset;
}

//获取ICON Top偏移量
inline int qUIPushButton::getIconTopOffSet() const
{
    return m_iIconYOffSet;
}

// 设置Button的action
inline void qUIPushButton::setBtnAction(const QString & strBtnAction)
{
    m_strBtnAction = strBtnAction;
}

//返回Button的Action
inline QString qUIPushButton::getBtnAction() const
{
    return m_strBtnAction;
}

//设置按钮的编号
inline void qUIPushButton::setButtonID(int iIdx)
{
    m_iBtnID = iIdx;
}

inline int qUIPushButton::getButtonID() const
{
    return m_iBtnID;
}

inline void qUIPushButton::setTextXOffSet(int offset)
{
    m_iTextXOffSet = offset;
}

inline int qUIPushButton::getTextXOffSet() const
{
    return m_iTextXOffSet;
}

inline void qUIPushButton::setTextYOffSet(int offset)
{
    m_iTextYOffSet = offset;
}

inline int qUIPushButton::getTextYOffSet() const
{
    return m_iTextYOffSet;
}

inline Qt::AlignmentFlag qUIPushButton::getIconAlign() const
{
    return m_eIconAlign;
}

inline Qt::AlignmentFlag qUIPushButton::getTextAlign() const
{
    return m_eAlign;
}

inline void qUIPushButton::SetMask(const QString & strPicPath)
{
    m_strMaskPic = strPicPath;
}

qUIPushButton * CreateUIPushButton(QRect rect, QString strObjectName, QWidget * parent);

#endif  // QUIPUSHBUTTON_H

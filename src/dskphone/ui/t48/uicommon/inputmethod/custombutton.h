#ifndef CCUSTOMBUTTON_H
#define CCUSTOMBUTTON_H

#include "longpressbutton.h"

enum BUTTON_BG_TYPE
{
    BUTTON_BG_TYPE_ERROR = 0,
    BUTTON_BG_TYPE_LIGHT,
    BUTTON_BG_TYPE_NORMAL,
    BUTTON_BG_TYPE_DEEP,
    BUTTON_BG_TYPE_PRESS,
};

enum BUTTON_ANGLE_TYPE
{
    BUTTON_ANGLE_TYPE_ERROR = 0,
    BUTTON_ANGLE_TYPE_LEFT_TOP,
    BUTTON_ANGLE_TYPE_LEFT_BOTTOM,
    BUTTON_ANGLE_TYPE_RIGHT_TOP,
    BUTTON_ANGLE_TYPE_RIGHT_BOTTOM,
};

class CCustomButton : public CLongPressButton
{
    Q_OBJECT

public:
    CCustomButton(QWidget* v_parent = 0);
    virtual ~CCustomButton(void);

    virtual bool InputButtonMousePress(const QPoint& mousePressPoint);
    virtual bool InputButtonMove(const QPoint& mousePressPoint);
    virtual bool InputButtonMouseRelease(const QPoint& mouseReleasePoint);

    virtual void paintInputButton(QStylePainter& stylePainter);

public:
    //是否需要支持长按
    void SetLongPress(bool bLongPress)
    {
        m_bLongPress = bLongPress;
    }

    //是否在长按生效后，只发送一次按键
    void SetStopWhenEmit(bool bStopWhenEmit)
    {
        m_bStopWhenEmit = bStopWhenEmit;
    }

    //属性：是否是功能键
    void SetFunctionKey(bool bIsFunctionKey)
    {
        m_bIsFunctionKey = bIsFunctionKey;
    }
    bool GetIfFunctionKey()
    {
        return m_bIsFunctionKey;
    }

    void SetEnterKey(bool bIsEnterKey)
    {
        m_bIsEnterKey = bIsEnterKey;
    }
    void SetImeKey(bool bIsImeKey)
    {
        m_bIsImeKey = bIsImeKey;
    }
    void SetShiftKey(bool bIsImeKey)
    {
        m_bIsShiftKey = bIsImeKey;
    }

    bool IsEnterKey()
    {
        return m_bIsEnterKey;
    }
    bool IsImeKey()
    {
        return m_bIsImeKey;
    }
    bool IsShiftKey()
    {
        return m_bIsShiftKey;
    }

    //属性：背景颜色类型
    void SetButtonBGType(BUTTON_BG_TYPE eType)
    {
        m_eBGType = eType;
    }
    BUTTON_BG_TYPE GetButtonBGType()
    {
        return m_eBGType;
    }

    //属性：图标路径
    void SetButtonIconString(const QString &qstrText)
    {
        m_strButtonIcon = qstrText;
    }
    const QString& GetButtonIconString()
    {
        return m_strButtonIcon;
    }

    //获取显示的文字
    const QString& GetNormalText()
    {
        return m_qstrNormalText;
    }

    void SetHintText(const QString& strHintText)
    {
        m_qstrHintText = strHintText;
    }
    const QString& GetHintText()
    {
        return m_qstrHintText;
    }

    void SetFunctionName(const QString& strFunctionName)
    {
        m_qstrFunName = strFunctionName;
    }
    const QString& GetFunctionName()
    {
        return m_qstrFunName;
    }

    //按钮是否有效（如果无效就不需要绘制，也不需要响应，可能用于填充空位）
    bool IfButtonAvailable();

    //根据背景类型和边角类型，获取目标图片路径
    static const QString GetFilletPixByType(BUTTON_BG_TYPE eBGType, BUTTON_ANGLE_TYPE eAngleType);
    //根据背景类型，获取填充颜色
    static QColor GetColorByType(BUTTON_BG_TYPE eBGType);

signals:
    //长按后弹出备选框的信号
    void SignalLongPressToBox(bool bShowOption);

protected:
    virtual void timerEvent(QTimerEvent* pEvent);

protected slots:
    virtual void OnLongPressBtnPressed();

private:
    void paintButtonPress(QStylePainter& stylePainter);

protected:
    //是否需要支持长按
    bool m_bLongPress;
    //长按后是持续发出按键还是连接长按动作
    bool m_bStopWhenEmit;

    //是否功能键
    bool m_bIsFunctionKey;

    //按钮背景类型
    BUTTON_BG_TYPE m_eBGType;

    //按钮中央图标
    QString m_strButtonIcon;

    //该按钮是否是enter功能键，如果是，需要画enter图标
    bool m_bIsEnterKey;

    //该按钮是否是ime切换键，如果是，需要根据实际情况绘制
    bool m_bIsImeKey;

    //该按钮是否是ime切换键，如果是，需要根据实际情况绘制
    bool m_bIsShiftKey;

    //提示用的小文字
    QString m_qstrHintText;

    // function name
    QString m_qstrFunName;
};

#endif // CCUSTOMBUTTON_H

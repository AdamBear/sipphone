#ifndef INPUTBOXBUTTON_H
#define INPUTBOXBUTTON_H

#include "uikernel/buttonwrapper.h"
#include "imagemanager/modimagemanager.h"

typedef QVector<QString>    vecInputBoxString;
typedef QVector<int>            vecInputBoxInt;

class CInputBoxButton : public QWidget
{
    Q_OBJECT

public:
    static CInputBoxButton& GetInstance();

private:
    CInputBoxButton();
    ~CInputBoxButton();

public:
    void ResetInputBox();
    void ShowInputBox();
    void HideInputBox();

    //设置备选框
    //ptButtonLT长按的按钮的左上角位置
    //vChar长按的按钮的备选字符
    void SetInputBox(const QPoint& ptButtonLT, const vecInputBoxString& vChar, int nBoxtType, int nIndex);

    inline int GetInputBoxHighLight()
    {
        return m_nIndex;
    }

    bool ProcesseMouseMoveEvent(QMouseEvent *pMouseEvent);

    void JudgeIndexByMousePosition(const QPoint& pt);

protected:
    virtual void InitInputBox();
#ifndef IF_LYNC
    bool event(QEvent *event);
#endif
    virtual void paintEvent(QPaintEvent* pe);

    //设置备选框高亮背景
    void SetInputBoxHighLight(int nIndex);

    int CalculateTextWidth(const QString& strText);

signals:
    void SignalInputBoxPress();

private:
    QWidget*    m_pWidget;

    //整体位置（包括左右圆角）
    QRect           m_rctBox;
    //移动选中有效位置（比整体位置大一圈）
    QRect           m_rctSelectBox;

    //备选框是否处于显示状态
    bool                m_bIsShow;

    //按钮总个数
    int             m_nCount;
    //按钮总宽度
    int             m_nTotalWidth;
    //当前高亮的按钮序号
    int             m_nIndex;

    //记录当前所有备选文字
    vecInputBoxString       m_vecInputBoxString;
    //记录当前备选文字的宽度
    vecInputBoxInt          m_vecInputBoxWidth;

    //显示文字的字体
    QFont           m_charFont;

    //记录是否要进行全局刷新
    bool                m_bNeedFullScreenUpdate;
    bool                m_bLastFullScreenUpdate;

    int     m_nBoxType;
    int     m_nShowMaxNumRow;  //每行最大显示个数
};
#define g_InputBoxButton CInputBoxButton::GetInstance()

#endif // INPUTBOXBUTTON_H

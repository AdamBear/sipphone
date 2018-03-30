#ifndef DLGINPUT_H
#define DLGINPUT_H

#include "uikernel/scrollpage.h"
#include "inputframe.h"
#include <QResizeEvent>

//用于在显示lineEdit的 CXMLInputFrame控件上添加滚动条
class DlgInput : public QFrame
{
    Q_OBJECT

public:
    DlgInput(QWidget * pParent = 0);
    ~DlgInput();

    //建立input界面
    void InitInputFrame(std::list<InputField *> List_Input_Field);

public:
    CScrollPage * m_pScrollPage;

    //设置input的显示模式
    void SetFrameDisplayMode(INPUT_DISPLAY_MODE itemStyle);

    //为逻辑层提供接口，将所有lineEdit和其序号存入map中返回
    bool  GetFrameLineEditValue(YLHashMap<int, yl::string> & PageData);

    //得到当前焦点所在的序号
    int GetFrameCurrentItem();

    //设置默认焦点
    void SetFocusItem(int iIndex);

    bool eventFilter(QObject * pObj, QEvent * pEvent);

    CXMLInputFrame * GetInputFrame()
    {
        return m_pInputFrame;
    }

    void OnKeyboardShow();
    void OnKeyboardHide();

private:
    //用于显示lineEdit的frame
    CXMLInputFrame * m_pInputFrame;

protected:
    void resizeEvent(QResizeEvent * pEvent);

signals:
    //焦点改变信号
    void FrameFocusChange();
};

#endif

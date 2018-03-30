/*作用：
  根据逻辑层传来的list数据生成所有lineEdit和label
  设置显示模式、安装输入法、设置焦点等
*/
#ifndef INPUT_FRAME_H
#define INPUT_FRAME_H

#include <QLineEdit>
#include <QLabel>
#include <vector>
#include <map>
#include <list>
#include "extended/xmlbrowser/include/logicanduicommon.h"

#define XB_INPUT_FRAME_DEFAULT_FONT_SIZE 18

// 前向声明
class AutoResizeLab;
class CScrollPage;

struct LineEditItem
{
    QLineEdit * pLineEdit;
    INPUT_TYPE eInputType;  // 编辑框的输入类型;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecSoftkey;
#endif
};
class CXMLInputFrame : public QFrame
{
    Q_OBJECT

public:
    //记录当前所有lineEdit的数据类型
    //typedef std::vector<QLineEdit*> ArrayLineEdit;
    typedef std::vector<LineEditItem *> ArrayLineEdit;

    typedef std::vector<QLabel *> ArrayLabel;

    //保存时提供给逻辑层的数据类型
    typedef YLHashMap<int, yl::string>MapLineEdit;

public:
    CXMLInputFrame(QWidget * pParent = 0);
    ~CXMLInputFrame();

    //创建提示label和lineEdit
    void AddItem(std::list<InputField *> List_Input_Field);

    // 设置显示的模式
    void SetDisplayMode(INPUT_DISPLAY_MODE displayMode);

    // 上移item焦点
    void MoveUp();

    // 下移item焦点
    bool MoveDown(bool bIsEnterKey = false);

    //得到当前焦点序号
    int GetItemIndex();

    //得到焦点编辑框的输入类型
    INPUT_TYPE GetFoucsItemInputType();

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam GetFocusSoftkey();
#endif

    //为逻辑层提供接口，返回所有lineEdit组成的map
    bool GetLineEditValue(YLHashMap<int, yl::string> & PageData);

    //事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    //按键响应，需要响应上下键
    void keyPressEvent(QKeyEvent * pKeyEvent);

    //设置默认焦点
    void SetFrameFocusItem(int iIndex);

    //界面显示出来时设置焦点
    virtual void showEvent(QShowEvent * pEvent);

    void AdjustInput(QLineEdit* pEdit, bool bReset = false);
    void OnKeyboardShow();
    void OnKeyboardHide();
    // 是否是最后一个控件
    bool ProcessKeyPress(QKeyEvent* pKeyEvent);

    bool IsLastItem(QLineEdit* pEdit);
private:
    QWidget* m_pParent;
    bool m_bFirstShow;

protected:
    //设置颜色
    void SetTextColor(QWidget * pWidget, QColor color);
    void paintEvent(QPaintEvent * pEevnt);

private:

    // 根据InputField创建LineEdit
    void SetLineEditByInputField(const InputField & objInputField, QLineEdit * pLineEdit);
    // 检查合法性
    bool IsValidInput(INPUT_TYPE eInputType, yl::string strValue);
    // 检查日期的合法性
    BOOL IsDateValid(int iYear, int iMon, int iDate);

    void ClearEdit();

    //用于将所有lineEdit返回给逻辑层
    MapLineEdit m_mapLineEdit;

    //记录默认焦点序号
    int m_nIndex;

signals:
    //焦点改变后发送给scrollPage的信号
    void currentFocusWgtChange(QRect);

    //自定义焦点改变信号
    void FocusWgtChange();

protected:
    static const int    LineEditMaxLength = 128; // Input界面限制LineEdit输入的最大长度

    INPUT_DISPLAY_MODE  m_eDiaplayMode;  // 显示模式

    ArrayLineEdit       m_arrayLineEdit;    //存储所有lineEdit

    ArrayLabel          m_arrayLabel;

    QLineEdit * m_pCurrentFocus; //记录上一次焦点

    QLineEdit * m_pNowFocus; //记录当前焦点
};

#endif //INPUT_FRAME_H

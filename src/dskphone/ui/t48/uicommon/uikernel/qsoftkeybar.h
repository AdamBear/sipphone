/*
 * qSoftKeyBar
 * 话机软件盘
 */

#ifndef QSOFTKEYBAR_H
#define QSOFTKEYBAR_H

#include "quipushbutton.h"
#include <QtGui>
//#include <ETLLib.hpp>

// Softkey一页的最大数量
#define BUTTONS_PER_PAGE            4

#define SOFTKEY_EDK_ACTION      "EDK_"
#define SOFTKEY_EDK_ACTION_STRING_LENGTH 4

// style类型
enum SoftkeyType
{
    STYLE_TYPE_DEFAULT = 0,                     // 默认样式
    STYLE_TYPE_DIVIDE = STYLE_TYPE_DEFAULT,     // 按钮根据背景图分割的类型样式
    STYLE_TYPE_ICONTRAY,                        // 类似iphone图标托盘效果类型样式
    STYLE_TYPE_NORMAL,                          // 普通样式，拥有按钮状态图片
    STYLE_TYPE_MAX
};
// 前向声明
class dataSoftKey;
class qButtonItem;

// softKeyButton的数据类
// 封装了常用的操作和数据
// 和显示类qButtonItem结合使用
class qSoftButton
{
public:
    explicit qSoftButton(const QString & strButtonName = QString(),
                         QObject * receiver = NULL,
                         const char * member = NULL);

    // 设置softkeyButton上的文字
    qSoftButton * SetTitle(const QString & strTitle);

    // 设置Button被点击的时候相应的对象和槽函数
    qSoftButton * ConnectTo(QObject * receiver, const char * member);

    // 保留接口，目前无实际作用
    qSoftButton * EnableButton(bool bEnable);

    // 隐藏或者显示Button
    // bHide 为true则显示 为false则hide
    qSoftButton * ShowButton(bool bShow);

public:
    QString             m_strButtonTitle;   // Btn的Title,显示
    const QObject   *   m_pSignalReceiver;  // 信号接收者的指针
    QString             m_strSingalMember;  // 信号接收函数
    qButtonItem    *    m_pHostButton;      // 存放softKey显示类的指针
    QString             m_strButtonName;    // Btn的名字,Action
    QString             m_strIconPath;      // Btn的图标路径
};

// softkeyBtn的显示类
// 负责显示和相应softKeyBtn的一些操作
// 和数据类qSoftButton结合使用
class qButtonItem : public qUIPushButton
{
    Q_OBJECT

public:
    explicit qButtonItem(QWidget * parent);
    ~qButtonItem();

public:
    // 设置私有数据qSoftButton
    // qButtonItem的逻辑基本由qSoftButton完成
    void setData(qSoftButton * pData);
    QString GetAction() const;

    void SetRealText(const QString & strShowText);
    QString GetRealText() const;

    // 设置软键图标显示类型
    void SetSoftkeyType(SoftkeyType eIconType);

    // 获取软键图标显示类型
    SoftkeyType GetSoftkeySoftkeyType();

    void StartScrollTimer();
    void StopScrollTimer();
    void CheckStartTimer();

    void SetShowText(const QString & text);
    QString GetShowText() const;

public slots:
    void OnClicked();
private slots:
    void UpdateText();

protected:
    void paintEvent(QPaintEvent * pe);

Q_SIGNALS:
    void clicked(qSoftButton *);

private:
    int m_iIconYOffSet;     // Icon y偏移值
    int m_iTextYOffSet;     // 文字y偏移值
    SoftkeyType     m_eSoftkeyType; // 显示类型
    qSoftButton  *  m_pGuestData;
    QString m_strShowText;  // 显示的文字
    QString m_strRealText;  //真实的文字
    QTimer  m_timerScrollText;      //滚动提示定时器
    int m_nBegin;       //取字符串左边开始的位置
    int m_nWidth;           //显示长度
    int m_nDiffLength;      //真实长度和显示长度的差
    int m_nCount;
};

// softKeyBar，负责管理和显示N个softKey
class qSoftKeyBar : public QFrame
{
    Q_OBJECT
public:
    explicit qSoftKeyBar(QWidget * parent = 0);
    ~qSoftKeyBar();

public:
    // 设置父窗口
    void SetParent(QWidget * parent);

    // 设置整个Bar的背景图片
    void SetBackground(QString strBkgnd, QString strDivide);

    // 设置所有Button的背景
    void SetAllBtn3StatePic(const QString & normal_pic,
                            const QString & enable_pic,
                            const QString & disable_pic);
    // 设置单个Button的背景
    void SetBtn3StatePic(int index, const QString & normal_pic,
                         const QString & enable_pic, const QString & disable_pic);
    // 设置Button隐藏
    void SetButtonHide(int iIndex);

    // 设置Button显示
    void SetButtonShow(int iIndex);

    // 设置softkey数据
    qSoftButton * SetButton(int iIndex, qSoftButton * pButton);

    // 设置softkey数据
    // index 表示第几个softkey   lpszName 该softkey的名字
    // lpszTitle 该softkey显示的名字
    // receiver 点击该softkey响应函数的对象指针
    // member 点击该softkey响应函数的函数名 需要用SLOT()宏来进行转化
    qSoftButton * SetButton(int iIndex, LPCSTR lpszName, LPCSTR lpszTitle,
                            QObject * receiver, const char * member);
    qSoftButton * SetButton(int iIndex, LPCSTR lpszName, QString lpszTitle,
                            QObject * receiver, const char * member);

    // 添加一个softKeyBtn
    qSoftButton * AddButton(qSoftButton * pButton);

    // 通过Btn的Index来寻找这个Btn
    qSoftButton * GetButtonByIndex(int iIndex);

    // 通过Btn的名字来寻找这个Btn
    qSoftButton * GetButtonByName(LPCSTR lpszName);

    // 设置所有的Button都抬起
    void setKeyRelease();

    //清空数据
    void Clear();

    // 设置软键图标显示类型
    void SetSoftkeyType(SoftkeyType eIconType);

    // 获取软键图标显示类型
    SoftkeyType GetSoftkeySoftkeyType();

    // 设置图标加载的路径
    void SetIconPath(int iIndex, QString strPath);

public:
    // softKeyBar的事件过滤器
    bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 重新设置按钮的位置、大小和是否显示
    void ResetBtnStatus();

protected:
    void paintEvent(QPaintEvent * pe);
private slots:
    // 点击More按钮响应的槽函数
    void OnButtonMore();

private:
    // 刷新UI界面的Button
    void UpdateButton2UI();

private:
    qSoftButton    *    m_pButtonMore;// more按钮
    dataSoftKey    *    m_pSoftKeyData;// 管理SoftkeyData的类
    qButtonItem    *    m_arrPushButton[BUTTONS_PER_PAGE]; // 显示在界面上Button
    QWidget      *      m_parent;// 父控件指针
    SoftkeyType         m_eSoftkeyType; // 显示类型
    QString             m_strBackgroud;     // 背景图
    QString             m_strDivide;        // 分隔图
};

#endif // QSOFTKEYBAR_H


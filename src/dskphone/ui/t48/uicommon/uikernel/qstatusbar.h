/*
 * qStatisbar
 * 状态栏
 */
#ifndef FRMSTATUSBAR_H
#define FRMSTATUSBAR_H
#include <ETLLib/ETLLib.hpp>
#include <QtGui>
#include <QMap>
#include "quipushbutton.h"

namespace Ui
{
class frmStatusBar;
}

// 前向声明
class qDateTimeLabel;

// STB数据类
class QSTBItem
{
public:
    QSTBItem(int nFlashTime):
        m_nID(-1), m_nFlashTime(nFlashTime) {}
    virtual ~QSTBItem() {}
public:
    int     m_nID;
    QString m_strText;
    QString m_strTips;
    QString m_strIcon;
    QSize   m_size;
    int     m_nFlashTime;           //图标的闪烁时间，-1表示不闪烁
};

// statusBar上的Btn
class STBPushButton : public qUIPushButton, public QSTBItem
{
    Q_OBJECT
public:
    explicit STBPushButton(QWidget * parent = 0,
                           int nFlashTime = -1);          //默认为不闪烁状态
    void InitIconButton();

protected:
    virtual void paintEvent(QPaintEvent * pe);

signals:
    void clicked(qUIPushButton * pBtn);
    void pressed(qUIPushButton * pBtn);
    void released(qUIPushButton * pBtn);
    void longPress(qUIPushButton * pBtn);

private slots:
    void OnSTBPushButtonClicked();
    void OnSTBPushButtonReleased();
    void OnSTBPushButtonPressed();
    void OnSTBPushButtonLongPress();

    void OnTimeOut();           //设置此控件的闪动效果
protected:
    // 用于实现长按功能的计时器
    QTimer * m_timerLongPress;

private:
    QTimer * m_pTimerFlash;     //用于图标闪动的计时器
};

class frmStatusBar : public QFrame
{
    Q_OBJECT

public:
    // 控件对齐方式
    enum STATUS_BAR_ALIGN_TYPE
    {
        ALIGN_TYPE_BEGIN  = -2,
        ALIGN_TYPE_UNKNOW = -1,
        ALIGN_TYPE_LEFT   = 0,
        ALIGN_TYPE_CENTER = 1,
        ALIGN_TYPE_RIGHT  = 2,
        ALIGN_TYPE_END,
    };


public:
    explicit frmStatusBar(QWidget * parent = 0);
    frmStatusBar(STATUS_BAR_ALIGN_TYPE alignType, QWidget * parent = 0);
    ~frmStatusBar();

public:
    /*btnAccount的接口*/
    // 设置AccountButton的显示内容
    void SetAccountCaption(const QString & strCaption);

    /*labTime的接口*/
    // 设置时间显示的格式
    void SetTimeFmt(int iDateFrm, int iTimeFrm);

    // 返回时间显示的格式
    int GetTimeFmt();

    // 返回日期显示的格式
    int GetDateFrm();

    /*btnMenu的接口*/
    // 设置btnMenu的三态图标
    void setMenu3StatePic(const QString & normalPic,
                          const QString & enablePic,
                          const QString & disablePic);

    // 设置Menu的Icon和对齐方式
    void setMenuIcon(const QString & iconPic, int nAlign);

    // 获取MenuBtn的指针
    qUIPushButton * GetBtnMenu();

    /*statusBar的接口*/
    // 设置背景图片
    void SetBackGroundImage(QString strImage);

    // 根据ID获得一个Item的指针
    STBPushButton * GetStatusItemByID(int nID);

    // 设置statusBar的对齐方式
    // 0--left   1--center  2--right
    void SetAlignType(STATUS_BAR_ALIGN_TYPE nType);

    // 将数组中的N个item添加到statusBar上
    bool AddItem(QSTBItem item[], int nSTBItemNr, bool bFix = false);

    // 添加一个item到statusBar上
    bool AddItem(int nID, QString strText, QString strTips,
                 QString strIcon, QSize size, bool bFix = false, int nFlashTime = -1);

    // 删除一个Item
    bool DeleteItem(int nID, bool bFixItem = false);

    // 清空所有的Item
    bool DeleteAllItem();

signals:
    /*btnAccount的信号*/
    // 点击发出该信号
    void clickedAccount(qUIPushButton * pBtn);

    /*btnMenu的信号*/
    // 点击发出该信号
    void clickedMenu(qUIPushButton * pBtn);

    /*STB发出的信号*/
    // 点击时发出该信号
    void clickedItem(qUIPushButton * pBtn);

    // 长按时发出该信号
    void longPressItem(qUIPushButton * pBtn);

protected:
    // 初始化数据
    void  Init();

    // 添加一个STBPushButton按钮到界面上
    bool  AddItem(STBPushButton *& pBtn, bool bFix = false);

    // 根据对齐的方式，修正偏移的距离
    int GetItemOffsetCx(int nItemWidth);

    // 设置固定Item的数量
    void  SetFixItemCount(int nIndex);

    // 将iIndex之后的Item都位移iX
    void  MoveItem(int iIndex, int iX);

    // 通过Item的Size判断是通过插入还是追加的方式添加Item
    // 返回true代表插入的方式 false代表追加的方式
    bool  CheckInsertSTBPushButton(const QSize & size);

    // 计算Item的位置
    // size为插入item的size
    // Insert表示是否插入 true表示插入到FixItem后面 false表示插入到最后面
    QRect GetItemRect(QSize & size, bool bInsert);

    void adjustTBPushButtonSize(STBPushButton * pBtn);

protected slots:
    void OnBtnAccountClicked();
    void OnBtnMenuClicked();
    void OnFrmItemClicked(qUIPushButton * pBtnItem);
    void OnFrmItemPressed(qUIPushButton * pBtnItem);
    void OnFrmItemReleased(qUIPushButton * pBtnItem);
    void OnFrmItemLongPress(qUIPushButton * pBtnItem);

private:
    Ui::frmStatusBar * ui;

protected:
    // statusBar的对齐方式
    // 目前有居左、居右和居中
    STATUS_BAR_ALIGN_TYPE   m_nAlignType;
    QList<STBPushButton *>   m_listBtnItem; // 管理Item的链表
    int                     m_iFixItemCount;
    QLabel         *        m_pLabTip;// 显示Tip的Lab
    qUIPushButton     *     m_pBtnAccount;
    qUIPushButton     *     m_pBtnMenu;
    qDateTimeLabel     *    m_pDateTimeLabel;
};

#endif // FRMSTATUSBAR_H

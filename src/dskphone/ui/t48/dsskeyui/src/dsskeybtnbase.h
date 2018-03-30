#ifndef _DSSKEY_BTN_BASE_H_
#define _DSSKEY_BTN_BASE_H_

#include "uicommon/uikernel/quipushbutton.h"
#include <QTimer>
#include "dsskeyui/include/dsskeyui_common.h"
#include "dsskeyui/include/dsskeybtndisplaydelegate.h"

struct DsskeyDisplayData;
struct DssKeyLogicData;

class QPixmap;

enum BtnStatus
{
    BTN_STASTUS_NORMAL = 0,
    BTN_STASTUS_DOWN,
    BTN_STASTUS_CHECKED = BTN_STASTUS_DOWN,
    BTN_STASTUS_DISABLE,
    BTN_STASTUS_HIGHLIGHT,
    BTN_STASTUS_USING
};

struct TextStatus
{
    yl::string          languageLast;               // 上次使用的语言
    BtnStatus           btnStatusLast;              // 上次按键的状态
    bool                bUCEnableLast;              // 上次是否UC
    bool                m_bNewLineMode;             // 上次的分行模式
    bool                bContentChanged;            // 内容变化

    TextStatus()
    {
        clear();
    }

    void clear()
    {
        languageLast = "";
        btnStatusLast = BTN_STASTUS_NORMAL;
        bUCEnableLast = false;
        bContentChanged = false;
        m_bNewLineMode = false;
    }
};


class qDsskeyButton : public qUIPushButton
{
    Q_OBJECT

public:
    qDsskeyButton(int iIndex = -1, QWidget * parent = 0);

    virtual ~qDsskeyButton();

    // 更新按钮
    void FlashButton(bool bEnable);
    // 获取按钮的索引
    int GetIndex();
    // 获取按钮的类型
    int GetDsskeyType();
    // 获取闪烁类别
    DSS_KEY_FLASH_TYPE GetFlashType();

    bool eventFilter(QObject * pObj, QEvent * pEvent);

// 更新背景图片
    void UpdateGroudPic(const DsskeyDisplayData & sData);

    // 获取显示图标的路径
    QString GetPresenceIconPath();
    // 获取第二行文字
    QString GetSecondText();

    // 分两行显示，计算截断位置
    int GetCutoffPos(const QString & strText, const QFontMetrics & objFontMetrics, int iWidth);

    // 设置按钮按下效果的ICON的图片路径
    inline void setCurrIcon(const QString & icon_pic);

    inline QString getCurrIcon() const;

    // 设置按钮按下效果的ICON的图片路径
    inline void setDownIconPic(const QString & icon_pic);

    inline QString getDownIconPic() const;

    // 鼠标放开事件响应函数
    void mousePressEvent(QMouseEvent * e);
    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent * e);
    // 鼠标放开事件响应函数
    void mouseReleaseEvent(QMouseEvent * e);

    // 设置按键按下标志
    void SetBtnDown();
    // 硬按键单击
    void OnBtnClicked();
    // 设置显示模式
    void SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode);

    //更新显示状态
    virtual void UpdateDisplayStatus(const DsskeyDisplayData & sData);

    // 设置显示代理
    bool SetDisplayDelegate(DsskeyBtnDisplayDelegete * pDelegate);
    // 清除显示代理
    void DebindDisplayDelegate();

#if IF_BUG_30875
    //更新pagebutton显示
    void SetPageDsskeyText(QString strText);
#endif

protected slots:
    void OnBtnLongPress();

Q_SIGNALS:
    void btnClicked(int iDssKeyId);
    void btnPress(int iDssKeyId);
    void btnLongPress(int iDssKeyId);
    // 开始闪烁
    void startFlash(DSS_KEY_FLASH_TYPE eFlashType);
    // 停止闪烁
    void stopFlash(DSS_KEY_FLASH_TYPE eFlashType);

protected:
    void paintEvent(QPaintEvent * pe);
    // 更新显示内容
    bool UpdateContent(const DsskeyDisplayData & sData);
    // 更新闪烁内容
    bool UpdateFlashStatus(const DSS_KEY_FLASH_TYPE & flashType);

private:
    // 初始化数据
    void initData();
    // 设置第二行文字
    void SetSecondText(QString strSec);
    // 绘制图片文本内容
    void DrawTextPic(QPainter & p, QRect rc, int flag, QString strTx);
    // 构造文本图片缓存内容
    void CreateTextPiccache(QPixmap & textpic);
    // 判断使用的图标是否改变,绘制时根据状态获取真正需要显示图标
    bool IsUsedIconCache(QString strDispIcon);
    // 检测文本是否改变，并做相应处理
    bool CheckTextChanged(const bool & bUC, const BtnStatus & status);
    // 检测是否控制灯改变 http://192.168.1.99/Bug.php?BugID=24012
    bool CheckLightChange(DsskeyDisplayData & objDisplayData, DsskeyDisplayData & newDisplayData);
protected:
    bool                m_bRealBtnDown;     // 真正按键按下的标志位
    bool                m_bMousePressed;    //是否有收到按下，如果没有则不处理抬起
    bool                m_bLonePress;       // 长按标志
    QString             m_strCurrIcon;      // 当前要显示的Icon
    QString             m_strFlashIcon1;    // 闪烁显示的Icon1
    QString             m_strFlashIcon2;    // 闪烁显示的Icon2
    QString             m_strDownCurrIcon;    // 当前要显示的按下状态的Icon
    QString             m_strPresenceIcon;  // 显示状态的Icon
    int                 m_eDssKeyType;      // dsskey类型
    int                 m_iIndex;           // 按键索引
    int                 m_iDsskeyID;        // DsskeyID
    DSS_KEY_FLASH_TYPE  m_eFlashType;       // 闪动的类型
    bool                m_bFlash;           // 是否处于闪动状态
    int                 m_nFlashCnt;        // 闪动定时计数
    QTimer              m_longPressTimer;   // 长按定时器
    QString             m_strSecText;       // 第二行文本显示
    DSS_KEY_TEXT_MODE   m_eTextMode;        // 文字显示模式
    DSS_KEY_GROUNDPHOTO_MODE    m_eGroundPhoto;   // 背景显示模式
    QPoint              m_point;                // 鼠标位置
    QPixmap             m_pmLastGroudPic;       // 上次使用的背景图片
    QString             m_strLastGroud;         // 上次使用的背景图片名称
    QPixmap             m_pmLastIcon;           // 上次使用的图标图片
    QString             m_strLastIcon;          // 上次使用的图标名称
    QPixmap             m_pmTextPiccache;       // 文本图片缓存

    TextStatus          m_txStatus;             // 上次的文本状态

    DSSKEY_DISPLAY_MODE m_eDisplayMode;         // 显示模式，是长按键还是短按键

    DsskeyBtnDisplayDelegete  *  m_pDisplayDelegate;
    DsskeyDisplayData   m_dispData;

    QString             m_strLastFristRowText;// 上次使用的Label文字
    QString             m_strCurrGroundPath;// 现在显示的背景图片
    QString             m_strFlashGround;// 刷新的背景图片
    bool                m_bFlashGround; // 闪烁背景图片
    bool                m_bFlushEnable;
};

class qDsskeySettingButton : public qDsskeyButton
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
    QRect m_rcIcon;
    QRect m_rcIndex;
    QString m_strIndex;
#endif
public:
    qDsskeySettingButton(int iIndex = -1, QWidget * parent = 0);

    ~qDsskeySettingButton();

    //更新显示状态
    virtual void UpdateDisplayStatus(const DsskeyDisplayData & sData);

protected:
    void paintEvent(QPaintEvent * pe);
};

inline void qDsskeyButton::setDownIconPic(const QString & icon_pic)
{
    if (icon_pic != m_strDownCurrIcon)
    {
        m_strDownCurrIcon = icon_pic;
    }
}

inline QString qDsskeyButton::getDownIconPic() const
{
    return m_strDownCurrIcon;
}

inline void qDsskeyButton::setCurrIcon(const QString & icon_pic)
{
    if (icon_pic != m_strCurrIcon)
    {
        m_strCurrIcon = icon_pic;
    }
}

inline QString qDsskeyButton::getCurrIcon() const
{
    return m_strCurrIcon;
}

#endif

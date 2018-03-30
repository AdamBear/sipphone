// 普通Setting列表项：显示名加控件"
#ifndef _SETTING_ITEM_BASE_H_
#define _SETTING_ITEM_BASE_H_

#include "uikernel/listitem.h"
#include "uikernel/buttonwrapper.h"
#include <QLabel>
#include <QMouseEvent>
#include <ETLLib/ETL.h>
#include <ylstring.h>
#include "baseui/framelistdefine.h"
#include "settingui/src/settinguifeaturesdefine.h"

class MenuItemData;

#define setting_invalid_flag (int(-1))

enum SETTING_ITEM_TYPE
{
    SETTING_ITEM_NORMAL,
    SETTING_ITEM_MENU,
    SETTING_ITEM_RIDIO,
    SETTING_ITEM_BLUETOOTH,
    SETTING_ITEM_BLUETOOTHDEVICELIST,
    MESSAGE_LIST_ITEM,
    SETTING_ITEM_WIFI,
    WIRELESS_ITEM_SEARCH,
    SETTING_ITEM_USB_AUDIO,
    SETTING_ITEM_USB_PHOTO,
    SETTING_ITEM_VVM,
    SETTING_ITEM_MTSWACDLIST,
    VOICEMAIL_LIST_ITEM,
    SETTING_ITEM_ANIMATION,
    SETTING_ITEM_GENBAND_E911,
};

enum ITEM_STATE
{
    ITEM_STATE_NOMAL,
    ITEM_STATE_PRESS,
    ITEM_STATE_SELECT,
    ITEM_STATE_SELECT_PRESS
};

enum
{
    LIST_ITEM_TYPE_SETTING                          = 0x04000000,
    LIST_ITEM_SUB_TYPE_SETTING_MENU                 = 0x00010000,
    LIST_ITEM_SUB_TYPE_SETTING_RADIO                = 0x00020000,
    LIST_ITEM_SUB_TYPE_SETTING_PHOTO                = 0x00030000,
    LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTH            = 0x00040000,
    LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTHDEVICELIST  = 0x00050000,
    LIST_ITEM_SUB_TYPE_MESSAGE_LIST                 = 0x00070000,
    LIST_ITEM_SUB_TYPE_SETTING_WIFI                 = 0x00060000,
    LIST_ITEM_SUB_TYPE_WIRELESS_SEARCH              = 0x00080000,
    LIST_ITEM_SUB_TYPE_SETTING_MTSWACDLIST          = 0x00090000,
    LIST_ITEM_SUB_TYPE_ANIMATION                    = 0x000a0000,
    LIST_ITEM_SUB_TYPE_GENBAND_E911                 = 0x000b0000,
};

enum
{
    LIST_ITEM_ACTION_MOUSE_PRESS            = LIST_ITEM_ACTION_CUSTOM_BEGIN,        //鼠标按下
    LIST_ITEM_ACTION_MOUSE_RELEASE      = LIST_ITEM_ACTION_CUSTOM_BEGIN + 1,        // 鼠标松开
    NONE_EXTRA                          = LIST_ITEM_ACTION_CUSTOM_BEGIN + 2,        //没有额外icon
    DIRECTORY_EXTRA                     = LIST_ITEM_ACTION_CUSTOM_BEGIN + 3,        //有额外diretory的icon
};


struct QWidgetPair
{
    ~QWidgetPair() {}

    QString first;          //display label text
    QWidget * second;       //second widget
    QString third;          //action
    int iIndex;
    SETTING_ITEM_TYPE eType;
    yl::string strIcon;
    bool bAddIcon;
    bool bAddIndex;
    bool bAddMark;
    int  iExtraButten;//额外按钮的类型
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
    bool bChangeStyleOnPress;
#endif
    bool bChangeStyleOnSelect;
#endif
    bool bWithStatus; //针对Settingmenuitem
    yl::string strAuthorId;
    QVariant appendVariantData;
    int  iFlag;   // 用来代替通过序号来判断标志

    QWidgetPair()
    {
        init();
    }

    QWidgetPair(SETTING_ITEM_TYPE type)
    {
        init();
        eType = type;

        switch (eType)
        {
        case SETTING_ITEM_MENU:
        case SETTING_ITEM_RIDIO:
        case SETTING_ITEM_BLUETOOTH:
        case SETTING_ITEM_BLUETOOTHDEVICELIST:
        case MESSAGE_LIST_ITEM:
        case VOICEMAIL_LIST_ITEM:
        case SETTING_ITEM_ANIMATION:
#if IF_FEATURE_GENBAND_E911
        case SETTING_ITEM_GENBAND_E911:
#endif
            {
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                bChangeStyleOnPress = true;
#endif
                bChangeStyleOnSelect = true;
#endif
                bAddMark = false;
            }
            break;
        default:
            break;
        }
    }

    void init()
    {
        second = NULL;
        third = "";
        eType = SETTING_ITEM_NORMAL;
        bAddIcon = false;
        bAddIndex = true;
        bAddMark = true;
        bWithStatus = false;
        strAuthorId = "";
        iIndex = 0;
        iExtraButten = NONE_EXTRA;


#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
        bChangeStyleOnPress = true;
#endif
        bChangeStyleOnSelect = true;
#endif
        iFlag = setting_invalid_flag;
    }
};
typedef QVector<QWidgetPair> QWPVector;

//CSettingItem
class CSettingItem : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif
public:
    CSettingItem(QWidget * parent, QString strLabel, QWidget * pWgt, QString strAction
                 , int iDataIndex, bool bAddInex = true, bool bAddMark = false, int iExtraButten = NONE_EXTRA);
    CSettingItem(QWidget * parent, QWidgetPair wgtPair, bool bAddInex = true);
    explicit CSettingItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CSettingItem();
    void Init();

    virtual void ClearContent();
    virtual void Relayout();


    QWidget * ContentWgt()
    {
        return m_pWgt;   //获取内容控件
    }
    void SetContentWgt(QWidget * pWgt)
    {
        m_pWgt = pWgt;
        if (NULL != m_pWgt)
        {
            m_pWgt->setParent(this);
            m_pWgt->installEventFilter(this);
        }
    }

    QString GetClickAction()
    {
        return m_strAction;   //获取action
    }
    void SetClickAction(QString strAction)
    {
        m_strAction = strAction;
    }

    void SetLabText(QString strText);//设置标签文字
    void SetWgtRect(QRect);

    void SetConfiged(bool bConfiged);
    bool IsConfiged()
    {
        return m_bConfiged;
    }

    ITEM_STATE GetState();
    virtual void SetItemEnabled(bool bEnabled);// 设置控件enable属性
    virtual bool IsItemEnabled();

#ifdef  QT_TOUCH_SUPPORT
    void SetClickBg(yl::string strClickBg);
#endif

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    void SetChangeStyleOnPress(bool bChange);
#endif
    void SetChangeStyleOnSelect(bool bChange);
#endif

public:
    bool IsEditItem();
    void SetWgetHost(QWidget* pParent);
protected slots:
    void DelayHideKeyboard();

protected:
    void hideEvent(QHideEvent * pEvent);
    void showEvent(QShowEvent * pEvent);
    void paintEvent(QPaintEvent * pEvent);
    virtual void mousePressEvent(QMouseEvent* pEvent);
    virtual void mouseReleaseEvent(QMouseEvent* pEvent);

    void GenerateLabelText();//生成显示标签
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);
    virtual void SetFocus(bool bFocus);
    virtual void OnConfigedChange(bool bConfiged);

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    virtual void ApplyStyleByState(ITEM_STATE eState);
    virtual void ApplyPressStyle();
    virtual void ApplySelectStyle();
    virtual void ApplyNomalStyle();
#endif

protected:
    SETTING_ITEM_TYPE eType;//settingItem的类型如单选框和普通类型
    QString m_strLabel;//标签文本
    QLabel * m_pLabDisplay; //标签控件
    QWidget * m_pWgt; //配置控件
    QString m_strAction;//action
    bool m_bAddIndex; //是否在标签前面添加索引
    bool m_bAddMark;
    int m_bConfiged;
    int m_iExtraButten;
    CButtonWrapper m_btnExtra;
    int m_iBtnExtraLeft;

#ifdef  QT_TOUCH_SUPPORT
    yl::string m_strClickBg;
#endif
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    bool m_bChangeStyleOnPress;
#endif
    bool m_bChangeStyleOnSelect;
#endif
    QWidget* m_pWgetHost;
};

typedef QVector<CSettingItem> SettingItemVec;

//CSettingMenuItem
class CSettingMenuItem : public CSettingItem
{
public:
    CSettingMenuItem(QWidget * parent, MenuItemData * pItemData, bool bAddIcon = false
                     , int iDataIndex = -1, bool bAddInex = true, bool bWithStatus = false);
    CSettingMenuItem(QWidget * pParent, QWidgetPair & pair);
    ~CSettingMenuItem();

    static int GetSettingMenuItemType();

    void AddIcon(LPCSTR strIconPath);
protected:
    void setData(MenuItemData * pItemData, bool bAddIcon = false);

    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

protected:
    // >按钮
    CButtonWrapper m_btnEnter;
    QPixmap m_iconMap;
    bool m_bAddIcon;
    bool m_bWithStatus;//是否显示状态
};

//CSettingRadioItem
class CSettingRadioItem : public CSettingItem
{
public:
    CSettingRadioItem(QWidget * parent, QString strDisplayName, QString strAction, int iIndex = -1,
                      bool bAddIndex = true);
    CSettingRadioItem(QWidget * pParent, QWidgetPair & pair);
    explicit CSettingRadioItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);

    ~CSettingRadioItem() {};

    static int GetSettingRadioItemType();

public:
    virtual void SetFocus(bool bFocus);
    virtual void SetItemEnabled(bool bEnabled);
    virtual bool IsItemEnabled();
    void UpdateIcon();

protected:
    void Init();
    virtual void Relayout();
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void OnConfigedChange(bool bConfiged);

private:
    QLabel * m_pIconLab; // 单选按钮
};

#if IF_FEATURE_METASWITCH_ACD
//CMTSWAcdListItem
class CMTSWAcdListItem : public CSettingMenuItem
{
public:
    CMTSWAcdListItem(QWidget * parent, MenuItemData * pItemData, bool bAddIcon = false
                     , int iDataIndex = -1, bool bAddInex = true, bool bWithStatus = false);
    CMTSWAcdListItem(QWidget * pParent, QWidgetPair & pair);
    ~CMTSWAcdListItem();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();
};
#endif
#if IF_FEATURE_GENBAND_E911
class CGenbandE911ListItem : public CSettingItem
{
public:
    CGenbandE911ListItem(QWidget* pParent, QWidgetPair& pair);
    ~CGenbandE911ListItem();

protected:
    virtual void paintEvent(QPaintEvent* pEvent);
};
#endif
#endif //_SETTING_ITEM_BASE_H_

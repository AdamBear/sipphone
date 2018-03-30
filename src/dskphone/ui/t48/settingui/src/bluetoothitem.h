#ifndef _BLUETOOTH_ITEM_H_
#define _BLUETOOTH_ITEM_H_

#include "settingitembase.h"
#include "uikernel/buttonwrapper.h"
#include <QTimer>
#include "settingui/src/bluetoothuicommon.h"

enum LIST_ITEM_BLUE_BTN
{
    LIST_ITEM_BTN_CLICK     = 3,
};

struct BluetoothAddData
{
    QString strInfo;
    yl::string strMac;
    yl::string strNote;
    bool bIsPairOrNot;        // 是否配对过
    bool bExistNoteOrNot;     // 提示语是否需要一直显示（否则显示几秒后隐藏）
    bool bAutoConnect;        // 是否自动连接
#ifdef SUPPORT_PHONE_DEVICE
    bool bAutoSync;           // auto sync contacts
    bool bMobileDevice;       // other deive（手机、Pad等等）
#endif

    BluetoothAddData() : strInfo(""), strMac(""), strNote(""), bIsPairOrNot(false),
        bExistNoteOrNot(false), bAutoConnect(false)
#ifdef SUPPORT_PHONE_DEVICE
        , bAutoSync(false), bMobileDevice(false)
#endif
    {}
    BluetoothAddData(QString info, yl::string mac, yl::string note, bool isPairOrNot = false,
                     bool existNoteOrNot = false, bool autoConnect = false
#ifdef SUPPORT_PHONE_DEVICE
                             , bool autoSync = false, bool mobileDevice = false
#endif
                    )
        : strInfo(info), strMac(mac), strNote(note), bIsPairOrNot(isPairOrNot),
          bExistNoteOrNot(existNoteOrNot), bAutoConnect(autoConnect)
#ifdef SUPPORT_PHONE_DEVICE
        , bAutoSync(autoSync), bMobileDevice(mobileDevice)
#endif
    {}
    ~BluetoothAddData() {}
};


//CSettingBluetoothDeviceListItem
class CSettingBluetoothDeviceListItem : public CSettingItem
{
    Q_OBJECT

public:
    CSettingBluetoothDeviceListItem(QWidget * parent, int iDataIndex = -1, bool bAddInex = false);
    CSettingBluetoothDeviceListItem(QWidget * pParent, QWidgetPair & pair);
    ~CSettingBluetoothDeviceListItem();

    static int GetSettingBluetoothDeviceListItemType();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

private:
};

//CSettingBluetoothItem
class CSettingBluetoothItem : public CSettingItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcIcon;
    QRect m_rcLblMac;
    QRect m_rcLblDis;
    QRect m_rcLblNote;
#endif
public:
    CSettingBluetoothItem(QWidget * parent, QString strDisplayName, QString strAction,
                          yl::string strMac, yl::string strNote, yl::string strIconPath,
                          QString strInfo, int index = -1, bool bAddInex = false, bool bAddNote = false);
    CSettingBluetoothItem(QWidget * pParent, QWidgetPair & pair, bool bAddIndex = true);
    ~CSettingBluetoothItem();

    static int GetSettingBluetoothItemType();

    // update item data by QWidgetPair
    void SetItemData(QWidgetPair qPair);

    yl::string GetDeviceMac();

#ifdef SUPPORT_PHONE_DEVICE
    bool IsMobileDevice() const;
    bool IsAutoSyncDevice() const;
#endif

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

    // ???????
    void PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                      const char * pszDefaultIcon = NULL);
    void PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon, const QPoint & ptBegin,
                      int nInfoWidth = 0);

    void PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                       const char * pszDefaultIcon = NULL);

    // ?????????????
    void PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                   const char * pszDefaultIcon = NULL);
    // ???
    void PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                   const char * pszDefaultIcon = NULL);
    // ????????????????
    void PaintInfo(QStylePainter & stylePainter, const QPoint & ptText, int nRemainWidth);
    // ??????
    void PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle, const QRect & rcInfo,
                   const QRect & rcNote,
                   int nTitleAlign = Qt::AlignTop, int nInfoAlgin = Qt::AlignBottom, int nNoteAlgin = Qt::AlignLeft);
    // paint detail btn
    void PaintDetailButton(QStylePainter & stylePainter, int nRight);

    void StartTimeClock();
    void StopTimeClock();

private slots:
    void RemoveNote();

private:
    bool m_bAddNote;
    yl::string m_strIconPath;
    QString m_strInfo;
    CButtonWrapper m_btnDetail;
#ifdef BLUETOOTH_USED_NEW_VERSION
    QTimer     *    m_pTimer;
#endif
public:
    BluetoothAddData m_bluetoothData;
};

#endif //_BLUETOOTH_ITEM_H_

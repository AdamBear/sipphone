#ifndef SETTINGWIFIITEM_H
#define SETTINGWIFIITEM_H

#include "settingui/src/settingitembase.h"
#include "wifiui/include/wificommondata.h"

class NoteLabel;

enum LIST_ITEM_WIFI_BTN
{
    LIST_ITEM_WIFI_BTN_CLICK        = 3,
};

class CSettingWifiItem : public CSettingItem
{
    Q_OBJECT
public:
    CSettingWifiItem(QWidget * parent, QString strAction, QString sDisName, int iSecu, int iID,
                     std::string sNote
                     , int iDataIdx = -1, bool bAddIdx = false, bool bAddNote = false);
    CSettingWifiItem(QWidget * parent, QWidgetPair & pair, bool bAddIdx = false);
    ~CSettingWifiItem(void);

    //void SetItemData(QWidgetPair &pair);
    void SetNoteDisplayAndType(const QString & strDisplay, Item_Note_Type eType);
    // set connected or not
    void SetConnected(bool bConnected);

    int GetNetID();
    const QString GetDisName();
    // update item data by QWidgetPair
    void SetItemData(QWidgetPair qPair);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    void PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                      const char * pszDefaultIcon = NULL);
    void PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon, const QPoint & ptBegin,
                      int nInfoWidth = 0);

    void PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                       const char * pszDefaultIcon = NULL);

    //
    void PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                   const char * pszDefaultIcon = NULL);
    void PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                   const char * pszDefaultIcon = NULL);
    //
    void PaintInfo(QStylePainter & stylePainter, const QPoint & ptText, int nRemainWidth);
    void PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle, const QRect & rcInfo,
                   const QRect & rcNote,
                   int nTitleAlign = Qt::AlignTop, int nInfoAlgin = Qt::AlignBottom, int nNoteAlgin = Qt::AlignLeft);
    // paint detail btn
    void PaintDetailButton(QStylePainter & stylePainter, int nRight);

private:
    CButtonWrapper          m_DetailBtn;
    WifiAddData             m_WifiData;
    int                     m_iLeftIcon;
    int                     m_iExistOrNot;
    NoteLabel       *       m_pLblNote;
};

#endif

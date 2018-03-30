#ifndef POPUPBOXITEM_H
#define POPUPBOXITEM_H

#include "uikernel/listitem.h"

enum
{
    LIST_ITEM_TYPE_POPUP_BLUETOOOTH = 0x00007000,
};

enum ItemAction
{
    LIST_ITEM_ACTION_BLUETOOTH_AUTO_CONNECT = 0x0111,
    LIST_ITEM_ACTION_BLUETOOTH_MOBILE_SYNC,
};

class PopupBoxItem : public CListItem
{
    Q_OBJECT

public:
    PopupBoxItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    ~PopupBoxItem();

    void SetLeftDisplay(const QString & strDisplay);
    QString GetLeftDisplay();
    void SetLeftDisplayRect(const QRect rectDisplay);

    void SetWidget(QWidget * pWgt);
    QWidget * GetWidget();
    void SetWidgetLeftStartX(const int iStartX);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

    void PaintInfo(QStylePainter & stylePainter, const QRect & rcDisplay,
                   int nAlign = Qt::AlignVCenter);

private slots:
    void OnCheckBoxChanged(bool);

private:
    int             m_iWidgetStartX;
    QRect           m_rectLeftDisplay;
    QWidget    *    m_pWidget;
    QString         m_strLeftDisplay;
};

#endif

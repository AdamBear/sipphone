#ifndef NETWORK_CONF_MGR_H
#define NETWORK_CONF_MGR_H

#include "uikernel/listitem.h"

enum
{
    LIST_ITEM_TYPE_NW_CONF_MGR = 0x10000000,
};

class qUIPushButton;

class CNetworkMgrItem : public CListItem
{
    Q_OBJECT

public:
    explicit CNetworkMgrItem(QWidget * parent = NULL);
    ~CNetworkMgrItem();

    void SetName(QString strName);

    void SetUserID(QString strID);

    QString GetUserID();

    QString GetUserName();

    void SetAllowRemove(bool bRemove);

    void SetFontSize(int iFontSize);

    void SetFontColor(QColor clr);

    bool IsFontSizeValid(int iFontSize);

signals:
    void RequestRemoveMember(QString strUserID, QString strName);

protected slots:
    void OnMemberRemove();

protected:
    void paintEvent(QPaintEvent * pEvent);

private:
    QString m_strName;
    QString m_strID;
    bool    m_bRemove;              //是否显示remove
    QColor m_fontColor;
    qUIPushButton* m_pBtnRemove;    //remove按钮
};


#endif

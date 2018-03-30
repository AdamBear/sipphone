
#include "uikernel/listitem.h"
#include <ylstring.h>

enum
{
    LIST_ITEM_TYPE_POPUP_MENU           = 0x07000000,
};

class QStylePainter;
class IListItemAction;

class CPopupMenuListItem : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif
public:
    explicit CPopupMenuListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CPopupMenuListItem(void);

    static int GetPopupMenuListItemType();

public:
    void SetMenuIcon(const char * pszIcon, const char * pszSelIcon);
    void SetMenuText(const QString & strText);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintMenu(QStylePainter & stylePainter);

private:
    QRect m_rcIcon;
    QRect m_rcText;

    yl::string m_strIcon;
    yl::string m_strSelIcon;
    QString m_strText;
};
typedef CPopupMenuListItem * CPopupMenuListItemPtr;

#ifndef OPTIONBOX_ITEM_H
#define OPTIONBOX_ITEM_H

#include "uikernel/listitem.h"

enum
{
    LIST_ITEM_TYPE_OPTION_BOX = 0x00000250,
};

class COptionBoxItem : public CListItem
{
public:
    COptionBoxItem(QWidget * parent = NULL, bool bWithFocos = false);
    virtual ~COptionBoxItem();

    void SetText(const QString & strText);

    void SetItemRightMargin(int nRightMargin);

    void SetCurrentSelected(bool bSelected);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

protected:
    QLabel   *  m_pLabelText;

    QRect           m_rectBG;

    bool                m_bWithFocus;
    bool                m_Selected;
};

#endif // OPTIONBOX_ITEM_H

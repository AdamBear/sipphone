
#ifndef __CONTACT_PHOTO_LIST_ITEM_H__
#define __CONTACT_PHOTO_LIST_ITEM_H__

#include <ylstring.h>
#include "uikernel/listitem.h"
#include "uikernel/btnbarwrapper.h"
#include "baseui/titlebutton.h"

enum
{
    LIST_ITEM_TYPE_CONTACT_PHOTO        = 0x08000000,
};

enum
{
    LIST_ITEM_ACTION_CHOOSE_IMAGE_BEGIN         = LIST_ITEM_ACTION_CUSTOM_BEGIN,            // 图片被选中
    LIST_ITEM_ACTION_CHOOSE_IMAGE_END           = LIST_ITEM_ACTION_CUSTOM_BEGIN + 3,        // 图片被选中
    LIST_ITEM_ACTION_SELECT_IMAGE_BEGIN         = LIST_ITEM_ACTION_CUSTOM_BEGIN + 100,      // 图片被按下
    LIST_ITEM_ACTION_SELECT_IMAGE_END           = LIST_ITEM_ACTION_CUSTOM_BEGIN + 103,      // 图片被按下
    LIST_ITEM_ACTION_UNSELECT_IMAGE             = LIST_ITEM_ACTION_CUSTOM_BEGIN + 200,      // 图片取消选中
};

class QString;
class CContactPhotoListItem : public CListItem
{
public:
    explicit CContactPhotoListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CContactPhotoListItem(void);

    enum
    {
        CONTACT_PHOTO_COUNT_PER_ROW             = 4,                // 每行的头像数量
    };

public:
    static int GetPhotoCountPerRow();
    static int GetContactPhotoItemType();

    void ClearPhoto();
    void SetPhoto(int nIndex, const yl::string & strPhotoPath, const QString & strPhotoName);
    void SetSelPhotoIndex(int nIndex);
    int GetSelPhotoIndex() const;

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);

protected:
    CTitleButton * m_arrImage[CONTACT_PHOTO_COUNT_PER_ROW];
    CBtnBarWrapper m_btnBar;

    yl::string m_arrPath[CONTACT_PHOTO_COUNT_PER_ROW];
    QString m_arrName[CONTACT_PHOTO_COUNT_PER_ROW];
    bool m_arrValid[CONTACT_PHOTO_COUNT_PER_ROW];
    int m_nSelIndex;
};
typedef CContactPhotoListItem * CContactPhotoListItemPtr;

#endif

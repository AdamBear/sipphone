#ifndef _USB_PHOTO_ITEM_H_
#define _USB_PHOTO_ITEM_H_

#include "cusbitembase.h"
#include "ylstl/ylstring.h"

#include "usbuicommon.h"

class CUSBPhotoItem : public CUSBItemBase
{
    Q_OBJECT

public:
    CUSBPhotoItem(QWidget* pParent, QWidgetPair& pair);
    ~CUSBPhotoItem();

    static int GetUSBListItemType();

    virtual bool eventFilter(QObject* pObject, QEvent* pEvent)
    {
        return false;
    }

    inline void SetIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }
    inline int GetIndex()
    {
        return m_nIndex;
    }

    inline void SetCanLoad(bool bCanLoad)
    {
        m_bCanLoad = bCanLoad;
    }
    inline bool GetCanLoad()
    {
        return m_bCanLoad;
    }


protected:
    virtual void mousePressEvent(QMouseEvent* pEvent);
    virtual void mouseReleaseEvent(QMouseEvent* pEvent);

protected:
    virtual void paintEvent(QPaintEvent* pEvent);
    virtual void Relayout();

private:
    QLabel      m_picThumbnail;
    int         m_nIndex;

    yl::string  m_strPic;
    QPixmap m_Piximg;
    bool            m_bIsImageLoaded;

    bool            m_bCanLoad;
};

#endif //_USB_PHOTO_ITEM_H_

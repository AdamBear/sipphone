#ifndef __USBPHOTO_SELECTOR_H__
#define __USBPHOTO_SELECTOR_H__

#include <ylstring.h>
#include <vector>
#include "baseui/framelistdelegate.h"

enum USB_PIC_SET_TYPE
{
    USB_PIC_SET_TYPE_WALLPAPER,
    USB_PIC_SET_TYPE_SCRRENSAVER,
    USB_PIC_SET_TYPE_DSSKEYWALLPAPER,
};

typedef std::vector<yl::string> VEC_FAVORITES_ID;

class CUSBPhotoSelector : public CFrameListDelegate
{
public:
    CUSBPhotoSelector();
    virtual ~CUSBPhotoSelector();

public:

public:

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    USB_PIC_SET_TYPE m_eSelIndex;

    //保存的是名称ID
    VEC_FAVORITES_ID m_vecDisplayName;
};

#endif

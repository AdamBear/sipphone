#ifndef __DLG_CUSTOM_PHOTO_H__
#define __DLG_CUSTOM_PHOTO_H__

#include "directorycommon.h"
#include "xwindow/xlistviewadapter.h"
#include "xwindow-kernel/include/xsignal.h"


class CComboBoxListItem;

#ifdef PHOTO_SHOW

#include "dlgdirectorybase.h"

class CDlgCustomPhoto : public CDlgDirectoryBase, public xListViewAdapter
{
public:
    CDlgCustomPhoto();
    virtual ~CDlgCustomPhoto();

public:
    static CBaseDialog* CreateInstance();

    virtual void Init();
    virtual void Uninit();
    virtual void UpdateWnd(bool bUIReturn = false);

    // IListViewAdapter
public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView* pListView);
    // 根据位置获取列表项
    virtual xListViewItem* PrepareItem(xListView* pListView, int nDataIndex, xListViewItem* pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView* pListView, xListViewItem* pItem);

public:
    virtual void loadChildrenElements(xml_node& node);

    virtual bool onKeyPressedEvent(int nKeyCode);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    virtual void RefreshSoftkey();
    virtual void RefreshTitle();
protected:
    void InitPhotoItem();

    void FinishPhotoChoose();
    void OnCurrentIndexChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                              LPVOID pData);

protected:
    xImageView* m_pImgView;
    CComboBoxListItem* m_pPhotoItem;
};
#endif

#endif

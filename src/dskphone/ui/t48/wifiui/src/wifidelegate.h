#ifndef __WIFI_DELEGATE_H
#define __WIFI_DELEGATE_H

#include "settingui/src/subpagelistdelegate.h"
#include "settingui/src/settingitembase.h"
#include "baseui/framelistdelegatebase.h"
#include "wifiui/src/wirelesssearchitem.h"

class WifiDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    WifiDelegate();
    ~WifiDelegate();

    /******************************IListItemAction************************/
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    ///////////////////////////////////////////////////////////////////////
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // 刷新控件状态 : 编辑属性
    virtual void UpdateItemStatus(bool bEanbled);

    //刷新列表，重新加载数据
    virtual void UpdateList();

    void RecordOffSet();
    void ClearOffSet();

#ifdef _T49
    void SetTestMode(bool bTestMode);
    bool IsTestMode()
    {
        return m_bTestMode;
    }

#ifdef QT_SLIP_GESTURE_SUPPORT
    void SetContentHeight(int iContentHeight);

    void RecordOffSet();
    int GetRecordOffSet();
    void SetRecordOffSet(int iOffSet);
    void ClearOffSet();
#endif

#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    //刷新列表，重新加载数据
    virtual void UpdateList();
#endif

    //设置列表基本属性
    virtual void InitSettingListStyle();

#ifdef QT_SLIP_GESTURE_SUPPORT
    /****************************** ISlipAreaDelegate ************************/
    // 初始化滑动相关信息
    virtual void InitSlipInfo(CFrameListBase * pFrameList);

    // 根据位置获取列表项
    virtual CListItem * PrepareItem(CFrameListBase * pFrameList, int nDataIndex);

    // 刷新列表项数据
    virtual bool RefreshItem(CFrameListBase * pFrameList, CListItem * pItem);

    // 滑动区域发生变化
    virtual void OnSlipAreaChanged(CFrameListBase * pFrameList, int nOffset);

    // 不定长列表项场景中，根据偏移量获取偏移量所在Item的位置信息(rcItem)及数据索引(返回值)
    virtual int GetItemInfo(CFrameListBase * pFrameList, int nOffset, QRect & rcItem);
#endif

    /*************************************************************************/
    void SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx, int eItemType = SETTING_ITEM_WIFI);

protected:
    virtual void InitFrameList();
    virtual void UnInitFrameList();
    // 获取真正代理指针
    virtual IFrameListDelegatePtr GetDelegate() const;
#else
public:
    /*************************************************************************/
    void SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx, int eItemType = SETTING_ITEM_WIFI);

    void SetItemData(const QWPVector & vec);
#endif // _T49

private:
signals:
    void ItemDetailAction(CListItem * pItem);

#ifdef _T49
private:
    int                 m_iContentHeight;
    int                 m_iCurOff;
    bool                m_bTestMode;
#else
private:
    QWPVector           m_vecItemData;
    int                 m_iCurDataIndex;
#endif // _T49
};
typedef WifiDelegate * WifiDelegatePtr;

#endif

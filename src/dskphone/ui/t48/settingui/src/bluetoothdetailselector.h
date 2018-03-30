#ifndef __BLUETOOTH_DETAIL_SELECTOR__
#define __BLUETOOTH_DETAIL_SELECTOR__

#include "baseui/framelistdelegate.h"
#include "settingui/src/settingitembase.h"
#include "settingui/src/bluetoothuicommon.h"

class qCheckBox;

class BluetoothDetailSelector : public CFrameListDelegate
{
public:
    BluetoothDetailSelector();
    ~BluetoothDetailSelector();

    // IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    void SetTotalCount(int iTotCnt);
    int GetTotalCount()
    {
        return m_nTotCnt;
    }

    void LoadDataByType(BLUETOOTH_DETAIL_POPUP_DLG_TYPE eType);
    void SetMobileSync(bool bMobileSync);

    void ResetCheckBox();

private:
    int                     m_nSelIndex;
    int                     m_nTotCnt;
    qCheckBox       *       m_pMobileChkBox;
    QVector<QWidgetPair>    m_vecWgtPair;
    BLUETOOTH_DETAIL_POPUP_DLG_TYPE m_eType;
};

#endif

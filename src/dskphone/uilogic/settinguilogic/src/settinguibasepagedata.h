#ifndef __SETTINGUI_BASE_PAGE_DATA_H__
#define __SETTINGUI_BASE_PAGE_DATA_H__

#include "ylstl/yllist.h"
#include "ylstl/ylvector.h"
#include "ylstl/ylstring.h"
#include "settinguisoftkeydata.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguimessagedata.h"

class CSettingUIItemDataBase;
typedef YLVector<CSettingUIItemDataBase *>   VecSetUIItemData;
typedef YLVector<int>                       VEC_SETTING_ITEM_DATA_INDEX;

typedef YLList<CSettingUISoftkeyData *>      ListSetUISoftkeyData;

enum
{
    // 列表项类型(每个继承类使用一个字节来区分)
    SETTING_PAGE_DATA_TYPE_BASE     = 0x01000000,
    SETTING_PAGE_DATA_TYPE_MARK     = 0x00FF0000,
};

class CSettingUIPageDataBase
{
public:
    CSettingUIPageDataBase(int ePageDataType);
    virtual ~CSettingUIPageDataBase();

    virtual void Reset();

public:
    // 设置、获取内存中界面数据
    virtual bool SetPageData(CSettingUIPageDataBase & pageSaveData);
    virtual bool GetPageData(CSettingUIPageDataBase & pageDatas);
    // 从配置表中设置、获取界面数据
    virtual bool LoadPageData(CSettingUIPageDataBase & pageDatas);
    virtual bool SavePageData(const CSettingUIPageDataBase & pageDatas);
    // 添加、删除Item数据到界面中
    virtual bool AddItem(CSettingUIItemDataBase * pItemData);
    virtual bool DelItem(CSettingUIItemDataBase * pItemData, bool bUpdateShowIndex = true);

    void DelFromItem(const yl::string & strItemId);

    // 添加、删除保存后需要发的消息
    bool AddPageMessageData(CSettingUIMessageData * pMsgData);
    bool DelPageMessageData(CSettingUIMessageData * pMsgData);
    // 更新序号映射
    virtual void UpdateShowIndex();
    virtual void UpdateFocusIndex();

public:
    // 根据序号获取Item数据
    CSettingUIItemDataBase * GetItemByDataIndex(int nDataIndex);
    // 根据Item ID获取Item数据
    CSettingUIItemDataBase * GetItemByItemId(const yl::string & strItemId);
    // 根据Item ID获取Item序号
    int GetItemDataIndexByItemId(const yl::string & strItemId);
    // 设置内存中序号为nDataIndex项的数据
    virtual void SetItemDataValueByDataIndex(int nDataIndex, const yl::string & strValue);

public:
    // 获取、设置界面数据类型
    int GetPageDataType();
    void SetPageDataType(int ePageDataType);
    // 设置、获取界面焦点项序号
    int GetFocusIndex();
    void SetFocusIndex(int nFocusIndex);
    virtual void SetFocusIndexByItemID(const yl::string& strItemID);
    //获取、设置界面起始项下标
    int GetPageDataIndex();
    void SetPageDataIndex(int nPageDataIndex);

    // 获取界面显示项个数
    int GetTotalItemNumber();

    // 设置、获取时间到后是否返回到Idle
    bool IsReturnToIdle();
    void SetReturnToIdle(bool bReturnToIdlee);
    // 设置、获取键盘锁锁定后，是否允许操作
    bool IsLock();
    void SetLock(bool bLock);
    // 设置、获取界面数据保存后，提示是否重启话机
    bool IsNetworkChangedReboot();
    void SetNetworkChangedReboot(bool bNetworkChangedReboot);
    // 设置、获取界面是否可编辑
    bool IsEnable();
    void SetEnable(bool bEnable);
    //设置、获取界面保存后是否刷新
    bool IsRefreshPrevousView();
    void SetRefreshPrevousView(bool bRefresh);

    // 设置、获取界面ID
    const yl::string & GetPageID();
    void SetPageID(const yl::string & strPageID);
    //设置、获取界面Authorid
    const yl::string & GetPageAuthorid();
    void SetPageAuthorid(const yl::string & strPageAuthorid);
    // 设置、获取界面Title
    const yl::string & GetPageTitle();
    void SetPageTitle(const yl::string & strPageTitle);

    const yl::string & GetEmptyHint();
    void SetEmptyHint(const yl::string & strHint);

    //设置、获取界面点击响应
    const yl::string & GetAction();
    void SetAction(const yl::string & strAction);

    // 获取界面Softkey显示列表
    CSettingUISoftkeyData * GetListSoftkeyData();
    //设置界面Softkey显示列表
    void SetListSoftkeyData(CSettingUISoftkeyData * pSoftkeyData)
    {
        m_pSoftkeyData = pSoftkeyData;
    }
    // 获取界面保存后发的消息列表
    LIST_SETTING_MSG_DATA GetListPageMessageData();
    LIST_SETTING_MSG_DATA* GetPointListPageMessageData();

    void ClearItemData();
    void ClearSoftkeyData();

    // 设置、获取界面焦点项序号
#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    int GetPageItemCount();
    void SetPageItemCount(int nPageItemCount);
    int GetItemHeight();
    void SetItemHeight(int nItemHeight);
#endif

protected:
    // 获取内存中界面项个数
    int GetTotalItemNumberInMenory();
    // 根据序号获取内存中Item数据
    CSettingUIItemDataBase * GetItemByDataIndexFromMenory(int nDataIndex);

    void ClearMsgData();

protected:
    int m_ePageDataType;                        // 界面数据类型
    int m_nFocusIndex;                          // 焦点项序号
    int m_nPageDataIndex;                       // 界面起始项下标
    bool m_bReturnToIdle;                       // 时间到是否返回到Idle界面
    bool m_bLock;                               // 键盘锁锁住后，是否能够操作
    bool m_bNetworkChangedReboot;               // 配置修改后，是否提示重启
    bool m_bEnable;                             // 界面是否能够编辑
    bool m_bRefreshPrevousView;                 // 界面保存后，是否刷新层界面内容
    VecSetUIItemData m_vecItemData;             // 界面Item列表
    LIST_SETTING_MSG_DATA m_listPageMessageData;// 界面保存后发的消息列表
    yl::string m_strPageID;                     // 界面ID
    yl::string m_strPageAuthorid;               // 界面Authorid
    yl::string m_strTitle;                      // 界面Title
    yl::string m_strEmptyHint;                  // 界面数据为空的提示
    yl::string m_strAction;                     // 界面的点击响应
    yl::string m_strFocusItemID;                // 界面焦点ID
    CSettingUISoftkeyData* m_pSoftkeyData;      // 进入界面时显示的Softkey
    VEC_SETTING_ITEM_DATA_INDEX m_vecShowIndex; // 界面显示Item序号与内存中保存序号映射

#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    int m_nPageItemCount;
    int m_nItemHeight;
#endif
    friend class CSettingUIDelegateBase;
};

typedef  CSettingUIPageDataBase    *    CSettingUIPageDataBasePtr;

#endif


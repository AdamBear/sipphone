#ifndef __SETTING_DELEGATE_H__
#define __SETTING_DELEGATE_H__

#include "directorycommon.h"

#ifdef DIR_SUPPORT_SETTING
#include "browserdelegate.h"


// 编辑状态
typedef struct _ST_DIR_EDIT_STATUS
{
    // 是否全部选中模式
    bool bSelectAll;

    // 非选中模式被选中的联系人ID，选中模式中不被选中的联系人ID
    VEC_CONTACT_ID vecSelectId;

    _ST_DIR_EDIT_STATUS()
        : bSelectAll(false)
    {
        vecSelectId.clear();
    }

    ~_ST_DIR_EDIT_STATUS()
    {
        Reset();
    }

    void Reset()
    {
        bSelectAll = false;
        vecSelectId.clear();
    }
} ST_DIR_EDIT_STATUS;

class CSettingDelegate : public CBrowserDelegate
{
public:
    explicit CSettingDelegate();
    virtual ~CSettingDelegate(void);

    static CSettingDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

#ifdef FL_SUPPORT_TITLE
// IFrameTitleAction
public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);
#endif

public:
    // 初始化数据起始位置
    virtual bool Init(int nType, int nGroupId, int nPageDataIndex = 0);

    // 进入群组
    virtual bool OnChangeGroup(int nGroupId, bool bRefresh = true);

    // 是否选中所有
    bool IsSelectAll() const;
    // 是否有选中
    bool IsHadSelected() const;
    // 获取当前群组联系人数量
    int GetTotalCount() const;

    // 是否选择中联系人
    bool IsSelect(int nId);

    // 设置联系人数量
    void SetTotalCount(int nTotalCount);
    // 重置选中数量
    void ResetSelectCount();
    // 获取选中数量
    int GetSelectCount() const;
    bool GetSelectContactId(VEC_CONTACT_ID & vecId);

protected:
    int GetIndexById(int nId);

private:
    // 总数量
    static int s_nTotalCount;
    // 编辑状态
    static ST_DIR_EDIT_STATUS s_stEditStatus;
};
#endif

#endif


#ifndef __DIRECTORY_MGR_H__
#define __DIRECTORY_MGR_H__

#include <singletonclassdefine.h>
#include "directorylistdelegate.h"
#include "groupdelegate.h"
#include "messagebox/messageboxcallback.h"


class CBaseDialog;
class CFrameList;
class CTreeList;
class CContactPhotoSelector;
class CGroupSelector;
class CNumberSelector;
class CRingSelector;
class CTransferNumSelector;
#if IF_FEATURE_GBBOOK
class CAccountSelector;
#endif


class CDirectoryMgr : public CMsgBoxCallBackAction
{
    DECLARE_SINGLETON_CLASS(CDirectoryMgr)

public:
    // 消息处理
    static LRESULT OnMessage(msgObject & refMessage);

    void Init();
    void UnInit();

    void ClearDirectoryLogicData();

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    CDirectoryListDelegatePtr GetActiveDelegate();
    CDirectoryControlerPtr GetActiveControler();
    CDirUIHelperPtr GetActiveUIHelper();

    // 获取群组数据代理
    CGroupDelegatePtr GetGroupDelegate();

    int GetActiveDirType();

    // 界面刷新
    void OnUIShow(int nUIState, CTreeList * pTreeList, CFrameList * pFrameList);
    // 界面隐藏
    void OnUIHide(CTreeList * pTreeList, CFrameList * pFrameList);

    bool IsGroupShow(int nType, int nGroupId);

    // 响应右侧树形控件群组被选中(如果不被选中，返回false)
    bool OnGroupClick(int nType, int nGroupId);
    // 响应搜索按钮被点击
    void OnSearchClick();
    // 响应新建联系人按钮被点击
    void OnNewContactClick();
    // 响应设置按钮被点击
    void OnSettingClick();
    // 响应删除call log组
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    void OnClearGroupClick();
#endif

    // 进入主界面
    bool EnterDirectory(int nDirType, int nGroupId, CALL_TYPE eCallType = CT_CALLALL);
    // 切换回原来的界面
    bool SwitchLastDirectory(int nDefaultType, int nDefaultGroupId);
    // 进入搜索界面
    void EnterSearch(int nDirType, int nGroupId, int nKeyCode);
    // 进入高级搜索
    void EnterPreciseSearch();
    // 进入高级搜索
    void EnterPreciseSearch(int nDirType, int nGroupId);
    // 进入设置界面
    void EnterSetting();
    // 进入详情界面
    void EnterDetail(int nDirType, int nContactId);
    // 进入新建本地联系人界面
    void EnterAddition(int nDirType, int nGroupId, bool bCopy, bool bIsDsskeyAdd = false);
    // 进入铃声设置界面
    void EnterRingSetting(int nId);
    void OnRingSetting();

    void RetryChgPwdRuest(int nDirType, const yl::string & strPwd);

    bool CheckGroupDelegate(int nDirType);

    void SetCurDirType(int nDirType);
    bool CheckContactDelegate(int nUIState, int nGroupId, bool bChgMod);

    CContactPhotoSelector * GetPhotoSelector();
    CGroupSelector * GetGroupSelector();
    CNumberSelector * GetNumberSelector();
    CRingSelector * GetRingSelector();
    CTransferNumSelector * GetTransferNumSelector();
#if IF_FEATURE_GBBOOK
    CAccountSelector * GetAccountSelector();
#endif
    void ReleaseSelector();

#if DIR_SUPPORT_SELECT_CONTACT
    void SetContactSelector(DIR_SELECT_CONTACT_FUN pFunSelectContact);
    void OnSelectContact(yl::string strContactName, yl::string strContactNumber);
    void CancelSelectDirectory();
    bool GetIsInSelecting();
#endif

    void SetIsEditCallLog(bool bIsEditCallLog);
    bool GetIsEditCallLog();

    int GetModType(int nDirType) const;

protected:
    // Browser/Setting模块
    int AdjustContactListGroupId(int nLastDirType, int nDirType, int nLastGroupId, int nSelGroupId);
    bool CreateContactPage(CBaseDialog *& pDlgPage, CTreeList *& pTreeList, CFrameList *& pFrameList);
    bool CreateCallLogPage(CBaseDialog *& pDlgPage, CTreeList *& pTreeList, CFrameList *& pFrameList);
private:
    // 树形群组列表代理类
    CGroupDelegatePtr m_pGroupDelegate;
    // 当前使用的列表框代理类
    CDirectoryListDelegatePtr m_pActiveDelegate;

    // 模块类型
    int m_nModType;

    // 联系人类型
    int m_nLastType;
    int m_nCurType;

    // 联系人群组ID
    int m_nLastGroupId;
    int m_nCurGroupId;

    // 联系人页面起始序号
    int m_nLastPageDataIndex;
    int m_nCurPageDataIndex;

    CContactPhotoSelector * m_pPhotoSelector;
    CGroupSelector * m_pGroupSelector;
    CNumberSelector * m_pNumSelector;
    CRingSelector * m_pRingSelector;
#if IF_FEATURE_GBBOOK
    CAccountSelector * m_pAccSelector;
#endif
    CTransferNumSelector * m_pTransferNumSelector;

#if DIR_SUPPORT_SELECT_CONTACT
    DIR_SELECT_CONTACT_FUN m_pFunSelectContact;
#endif

    bool        m_bIsEditCallLog;
};

#define g_DirectoryMgr GET_SINGLETON_INSTANCE(CDirectoryMgr)

#endif


#ifndef __DIRECTORY_MGR_H__
#define __DIRECTORY_MGR_H__

#include <singletonclassdefine.h>
#include "directoryadapter.h"
#include <directorydefine.h>

typedef YLList<CDirectoryAdapterPtr> LIST_DIR_ADAPTER;

class CDlgDirectoryBase;

class CDirectoryMgr
{
    DECLARE_SINGLETON_CLASS(CDirectoryMgr)

public:
    // 消息处理
    static LRESULT OnMessage(msgObject & refMessage);

    void Init();
    void UnInit();

    void Reset();

public:
    CDirectoryAdapterPtr GetActiveAdapter();
    CDirectoryControlerPtr GetActiveControler();
    IDirUIHelperPtr GetActiveUIHelper();

    int GetActiveDirType();

    // 进入主界面(从其他模块进入到Directory)
    bool EnterDirectory(int nDirType, int nGroupId);
    bool EnterBrowser(int nDirType, int nGroupId);
    // 进入群组(BroadSoft、Remote联系人)
    bool EnterGroup(int nDirType, int nGroupId);
    // 进入搜索界面
    bool EnterSearch(int nDirType, int nGroupId, int nKeyCode);
    // 进入高级搜索
    void EnterPreciseSearch();
    // 进入详情界面
    void EnterDetail(int nAction, int nDirType, int nGroupId, int nContactId);
    // 进入号码选择界面
    void EnterNumberChoose(int nDirType, int nGroupId);
    // 进入头像选择界面
    void EnterPhotoChoose(int nDirType, int nContactId);
    // 进入Bsft密码框
    void EnterBsftPwd();
#if IF_FEATURE_34162
    // 进入LDAP密码框
    void EnterLDAPPwd();
#endif
    // 进入账号选择界面
    void EnterAccountChoose(int nDirType, int nGroupId, bool bRegisterAccount = true);
    bool LeaveCurrentState(int nRefreshType = DIR_UI_REFRESH_TYPE_ALL);

    void OnReturnIdle(CDlgDirectoryBase * pDlg);

#if DIR_SUPPORT_SELECT_CONTACT
    void SetContactSelector(DIR_SELECT_CONTACT_FUN pFunSelectContact);
    void OnSelectContact(yl::string strContactName, yl::string strContactNumber);
    void CancelSelectDirectory();
    bool IsInSelecting();
#endif

protected:
    bool EnterState(int nState, int nDirType, int nGroupId);

    CDlgDirectoryBase * CreateDirDialog(int nState);
    CDlgDirectoryBase * GetDirDialog(int nState);

private:
    LIST_DIR_ADAPTER m_listAdapter;

    // 当前使用的列表框代理类
    CDirectoryAdapterPtr m_pActiveAdapter;
#if DIR_SUPPORT_SELECT_CONTACT
    DIR_SELECT_CONTACT_FUN m_pFunSelectContact;
#endif
};

#define g_DirectoryMgr GET_SINGLETON_INSTANCE(CDirectoryMgr)

#endif

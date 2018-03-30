#ifndef __SETTINGUI_MANAGER_H__
#define __SETTINGUI_MANAGER_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "settinguicommondata.h"

class CSettingUIDelegateBase;
typedef YLList<CSettingUIDelegateBase*> LIST_SET_UI_DELEGATE_BASE_PTR;

class CSettingUIManager
{
public:
    enum DEL_TYPE
    {
        DEL_TYPE_BASE       = 0x0001,
        DEL_TYPE_FROM_START = DEL_TYPE_BASE + 1,            // 从头开始删
        DEL_TYPE_FROM_TOP   = DEL_TYPE_BASE + 2,            // 从顶开始删
        DEL_TYPE_CUSTOM     = DEL_TYPE_BASE + 3,            // 自定义位置开始删
    };

private:
    CSettingUIManager();
    virtual ~CSettingUIManager();

public:
    static CSettingUIManager * GetInstance();
    static void ReleaseInstance();

public:
    void Init();
    void UnInit();

    void Clear();

    // 询问界面ID为strPageID的Delegate是否已在栈中
    bool IsPageDelegateCreate(const yl::string & strPageID);
    // 根据界面ID从已创建的Delegate栈中获取Delegate
    CSettingUIDelegateBase * GetPageDelegate(const yl::string & strPageID);
    // 把界面Id为strPageID的Delegate提取到栈顶
    bool MoveToTop(const yl::string & strPageID);
    // 获取顶层Delegate
    CSettingUIDelegateBase * GetTopDelegate();
    // 添加Delegate
    bool AddDelegate(CSettingUIDelegateBase * pDelegate);
    // 删除Delegate
    bool DelDelegate(CSettingUIDelegateBase * pDelegate);
    // 从栈顶开始删除Delegate，直到pEndDelegate为止（包括pEndDelegate）
    bool DelDelegateFromTop(CSettingUIDelegateBase * pEndDelegate);
    // 删除指定区间Delegate，确保pEndDelegate、pBeginDelegate都存在
    // bCheck是否检测pBeginDelegate、pEndDelegate都在栈中
    bool DelDelegateRange(CSettingUIDelegateBase * pBeginDelegate,
                          CSettingUIDelegateBase * pEndDelegate, bool bCheck);
    // 从某位置开始删除连续的iNum个Delegate
    bool DelDelegate(int iNum, int eDelType = DEL_TYPE_FROM_START, int iStIndx = 0);

private:
    static CSettingUIManager    *   s_SettingUIManager;

    LIST_SET_UI_DELEGATE_BASE_PTR m_listPageDelegate;
};

#define g_pSettingUIManager (CSettingUIManager::GetInstance())

#endif


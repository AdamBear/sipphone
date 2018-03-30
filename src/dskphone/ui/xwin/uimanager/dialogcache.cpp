#include "uimanager_header.h"
#include "uimanager_inc.h"
#include "dsklog/log.h"

// 预创建的窗口，参数为窗口类名，初始化创建的窗口名都为类名
static LPCSTR s_arrayPrecreateDlg[] =
{
    DLG_CDlgDirectory,
    DLG_CDlgDirSearch,
    DLG_TalkUIFrame,
    DLG_DialingUI,

    NULL // 空指针表示结尾
};

// 第一次创建后缓存的窗口，参数为窗口类名
static LPCSTR s_arrayCacheDlg[] =
{
    NULL // 空指针表示结尾
};

CDialogCache::CDialogCache()
{
    m_MapCacheDlg.clear();
}

CDialogCache::~CDialogCache()
{

}

// 初始化，从配置加载窗口类型及创建窗口
void CDialogCache::Init()
{
    UIMANAGER_INFO("CDialogCache::Init begin");

    // 将需要预创建的窗口加入缓存中
    int i = 0;
    yl::string strTemp = "";
    CBaseDialog * pDlgTemp = NULL;
    while (s_arrayPrecreateDlg[i] != NULL)
    {
        // 遍历需要预创建的窗口数组，将每一项都创建后添加到Map中
        strTemp = s_arrayPrecreateDlg[i];
        pDlgTemp = CreateDlg(strTemp, true);
        if (pDlgTemp != NULL)
        {
            pDlgTemp->DialogHide();
            pDlgTemp->PopWindowObject();
            AddToMap(strTemp, strTemp, pDlgTemp);
        }

        i++;
    }

    UIMANAGER_INFO("CDialogCache::Init end");
}

// 释放，退出时释放缓存窗口
void CDialogCache::Uninit()
{
    MAP_Cache_Dlg::iterator itBegin = m_MapCacheDlg.begin();
    for (; itBegin != m_MapCacheDlg.end(); ++itBegin)
    {
        if (itBegin->second.pDlg != NULL)
        {
            itBegin->second.pDlg->DialogClose();
            itBegin->second.pDlg = NULL;
        }
    }

    m_MapCacheDlg.clear();
}

// 创建小窗口
CBaseDialog * CDialogCache::CreatePrivateDlg(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName /* = ""*/,
        bool bWaitSetActive/* = false*/)
{
    if (strDlgClassName.empty())
    {
        return NULL;
    }

    yl::string strDlgName = strDlgObjectName.empty() ? strDlgClassName : strDlgObjectName;

    // 从缓存链表中获取小窗口指针
    CBaseDialog * pTargetDlg = GetDlgFromCache(strDlgClassName, strDlgName);
    // 如果缓存中未获取到则创建，根据缓存类型判断是否加入缓存map中
    if (pTargetDlg == NULL)
    {
        // 根据窗口类型创建
        pTargetDlg = CreateDlg(strDlgClassName, bWaitSetActive);
        if (pTargetDlg == NULL)
        {
            return NULL;
        }

        // 如果是缓存类型的窗口则将其加入缓存map中
        // 如果缓存窗口列表中已存在相同名字的窗口，则不再加入
        //!!!!!
        if (/*GetCacheType(strDlgClassName) != DCT_NoCache && */!IsDlgCached(strDlgName))
        {
            // 加入缓存中时要标记窗口已被使用
            AddToMap(strDlgClassName, strDlgName, pTargetDlg, true);
        }
    }
    // 取出小窗口时要初始化数据
    pTargetDlg->Init();
    return pTargetDlg;
}

// 获取小窗口，参数为窗口名
CBaseDialog * CDialogCache::GetPrivateDlg(const yl::string & strDlgObjectName)
{
    MAP_Cache_Dlg::iterator itTarget = m_MapCacheDlg.find(strDlgObjectName);
    if (itTarget != m_MapCacheDlg.end()
            && itTarget->second.bUsed)
    {
        // 缓存窗口正在被使用才返回
        return itTarget->second.pDlg;
    }

    UIMANAGER_INFO("[UIManager] Dialog(%s) is not cached or not used!", strDlgObjectName.c_str());
    return NULL;
}

// 用完释放小窗口，参数为窗口指针
void CDialogCache::PutPrivateDlg(CBaseDialog * pDlg)
{
    if (pDlg == NULL)
    {
        UIMANAGER_INFO("CDialogCache::PutPrivateDlg invalid NULL pointor");
        return;
    }

    //!!!!! 逻辑需要优化
    MAP_Cache_Dlg::iterator itBegin = m_MapCacheDlg.begin();
    for (; itBegin != m_MapCacheDlg.end(); ++itBegin)
    {
        if (pDlg == itBegin->second.pDlg)
        {
            break;
        }
    }

    if (itBegin == m_MapCacheDlg.end())
    {
        UIMANAGER_INFO("CDialogCache::PutPrivateDlg no found (name:%s)", pDlg->GetDialogName().c_str());

        // 释放界面数据
        pDlg->Uninit();
        if (!pDlg->DialogClose())
        {
            delete pDlg;
        }
        pDlg = NULL;
        return;
    }

    UIMANAGER_DEBUG("CDialogCache::PutPrivateDlg(name:%s)", pDlg->GetDialogName().c_str());

    // 清空窗口数据
    pDlg->Uninit();

    // 如果窗口需要缓存，则不delete，只清空数据，否则要释放
    if (GetCacheType(itBegin->second.strClassName) == DCT_NoCache)
    {
        m_MapCacheDlg.erase(itBegin);
        if (!pDlg->DialogClose())
        {
            delete pDlg;
        }
        pDlg = NULL;
    }
    else
    {
        // 使用完缓存窗口并还回来时需要将是否使用的标志置为假
        SetDlgUnused(pDlg);
        // 将还回到缓存中的小窗口隐藏
        pDlg->DialogHide();
        // 将还回到缓存中的小窗口销毁
        pDlg->PopWindowObject();
    }
}

// 用完释放小窗口，参数为窗口名
void CDialogCache::PutPrivateDlg(const yl::string & strDlgObjectName)
{
    MAP_Cache_Dlg::iterator itTarget = m_MapCacheDlg.find(strDlgObjectName);
    if (itTarget != m_MapCacheDlg.end())
    {
        //释放窗体
        PutPrivateDlg(itTarget->second.pDlg);
    }
}

// 从map中获取窗口指针，参数为窗口名
CBaseDialog * CDialogCache::GetDlgFromCache(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName)
{
    //这里改成先用窗口名取，若取不到可用的，采用类名，若类名取得到，则将使用该项
    MAP_Cache_Dlg::iterator itTarget = m_MapCacheDlg.find(strDlgObjectName);
    if (itTarget != m_MapCacheDlg.end()
            && !itTarget->second.bUsed)
    {
        // 缓存窗口未被使用才返回，并标记为已使用
        itTarget->second.bUsed = true;
        return itTarget->second.pDlg;
    }

    MAP_Cache_Dlg::iterator itBegin = m_MapCacheDlg.begin();
    for (; itBegin != m_MapCacheDlg.end(); ++itBegin)
    {
        if (!itBegin->second.bUsed
                && strDlgClassName == itBegin->second.strClassName)
        {
            // 缓存窗口未被使用且类名与当前所需使用的对象一致才返回，并标记为已使用
            CBaseDialog * pDialog = itBegin->second.pDlg;
            m_MapCacheDlg.erase(itBegin);
            AddToMap(strDlgClassName, strDlgObjectName, pDialog, true);
            return pDialog;
        }
    }

    UIMANAGER_INFO("[UIManager] Dialog(%s) is not cached or is already used!",
                   strDlgObjectName.c_str());
    return NULL;
}

// 获取窗口缓存类型，参数为窗体类型
Dlg_Cache_Type CDialogCache::GetCacheType(const yl::string & strDlgClassName)
{
    // 到预创建数组中查找是否存在
    int i = 0;
    while (s_arrayPrecreateDlg[i] != NULL)
    {
        if (strDlgClassName == s_arrayPrecreateDlg[i])
        {
            return DCT_PreCreate;
        }

        i++;
    }

    // 到缓存类型数组中查找是否存在
    i = 0;
    while (s_arrayCacheDlg[i] != NULL)
    {
        if (strDlgClassName == s_arrayCacheDlg[i])
        {
            return DCT_Cache;
        }

        i++;
    }

    // 如果不是预创建也不是缓存类型，则返回不缓存
    return DCT_NoCache;
}

// 将窗口加入map中，参数为窗口名
void CDialogCache::AddToMap(const yl::string & strDlgClassName, const yl::string & strDlgObjectName,
                            CBaseDialog * pDlg, bool bUsed/* = false*/)
{
    // 设置ObjectName
    pDlg->SetDialogName(strDlgObjectName);

    SDlgInfo tmpInfo = { pDlg, strDlgClassName, bUsed };
    m_MapCacheDlg.insert(strDlgObjectName, tmpInfo);
}

// 将窗口状态设置为未使用，参数为窗口指针
void CDialogCache::SetDlgUnused(CBaseDialog * pDlg)
{
    if (pDlg == NULL)
    {
        return;
    }

    // 使用完缓存窗口并还回来时需要将是否使用的标志置为假
    MAP_Cache_Dlg::iterator itBegin = m_MapCacheDlg.begin();
    for (; itBegin != m_MapCacheDlg.end(); ++itBegin)
    {
        if (pDlg == itBegin->second.pDlg)
        {
            itBegin->second.bUsed = false;
            break;
        }
    }
}

// 判断窗口是否在缓存中存在，参数为窗口名
bool CDialogCache::IsDlgCached(const yl::string & strDlgObjectName)
{
    MAP_Cache_Dlg::iterator itTarget = m_MapCacheDlg.find(strDlgObjectName);
    if (itTarget != m_MapCacheDlg.end())
    {
        return true;
    }

    return false;
}

// 判断窗口是否在缓存中存在，参数为窗口指针
bool CDialogCache::IsDlgCached(CBaseDialog * pDlg)
{
    MAP_Cache_Dlg::iterator itBegin = m_MapCacheDlg.begin();
    for (; itBegin != m_MapCacheDlg.end(); ++itBegin)
    {
        if (pDlg == itBegin->second.pDlg)
        {
            return true;
        }
    }

    return false;
}

// 根据窗体类型来创建窗口，参数为窗体类型
CBaseDialog * CDialogCache::CreateDlg(const yl::string & strDlgClassName,
                                      bool bWaitSetActive/* = false*/)
{
    // 根据类名创建，类似于Java类反射
    CBaseDialog * pDlg = g_DlgFactory.CreateClassByName(strDlgClassName);
    if (pDlg != NULL)
    {
        return pDlg;
    }

    yl::string strWarning = commonAPI_FormatString("[UIManager] Unknown Dialog Class Name [%s]!",
                            strDlgClassName.c_str());

    chWARNING2(FALSE, strWarning.c_str());
    return NULL;
}

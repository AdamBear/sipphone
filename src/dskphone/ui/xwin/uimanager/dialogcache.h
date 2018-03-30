#ifndef __DIALOG_CACHE__
#define __DIALOG_CACHE__

// 窗口缓存类型
enum Dlg_Cache_Type
{
    DCT_PreCreate, // 上电初始化就创建
    DCT_Cache,     // 第一次创建后缓存
    DCT_NoCache,   // 每一次都重新创建
};

// 缓存窗口信息
struct SDlgInfo
{
    // 需要缓存的窗口指针
    CBaseDialog * pDlg;
    // 窗口类名
    yl::string       strClassName;
    // 该窗口是否已被使用，如果已被使用则不再从缓存中获取
    bool         bUsed;
};

typedef YLHashMap<yl::string, SDlgInfo> MAP_Cache_Dlg;

// 管理小窗口缓存
class CDialogCache
{
    friend class CUIManager;
public:
    CDialogCache();
    ~CDialogCache();

private: // for CUIManager
    // 初始化，从配置加载窗口类型及创建窗口
    void Init();
    // 释放，退出时释放缓存窗口
    void Uninit();
    // 创建小窗口
    CBaseDialog * CreatePrivateDlg(const yl::string & strDlgClassName,
                                   const yl::string & strDlgObjectName = "",
                                   bool bWaitSetActive = false);
    // 获取小窗口，参数为窗口名
    CBaseDialog * GetPrivateDlg(const yl::string & strDlgObjectName);
    // 用完释放小窗口，参数为窗口指针
    void PutPrivateDlg(CBaseDialog * pDlg);
    // 用完释放小窗口，参数为窗口名
    void PutPrivateDlg(const yl::string & strDlgObjectName);

private: // inner
    // 从map中获取窗口指针，参数为窗体类型与窗口名，先用窗口名取，若取不到可用的，采用类名，若类名取得到，则将使用该项
    CBaseDialog * GetDlgFromCache(const yl::string & strDlgClassName,
                                  const yl::string & strDlgObjectName);
    // 获取窗口缓存类型，参数为窗体类型
    Dlg_Cache_Type GetCacheType(const yl::string & strDlgClassName);
    // 将窗口加入map中，参数为窗体类型与窗口名
    void AddToMap(const yl::string & strDlgClassName, const yl::string & strDlgObjectName,
                  CBaseDialog * pDlg, bool bUsed = false);
    // 将窗口状态设置为未使用，参数为窗口指针
    void SetDlgUnused(CBaseDialog * pDlg);
    // 判断窗口是否在缓存中存在，参数为窗口名
    bool IsDlgCached(const yl::string & strDlgClassName);
    // 判断窗口是否在缓存中存在，参数为窗口指针
    bool IsDlgCached(CBaseDialog * pDlg);
    // 根据窗体类型来创建窗口，参数为窗体类型
    CBaseDialog * CreateDlg(const yl::string & strDlgClassName, bool bWaitSetActive = false);

private:
    // 小窗口缓存
    MAP_Cache_Dlg  m_MapCacheDlg;
};

#endif // __DIALOG_CACHE__

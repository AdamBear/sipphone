#ifndef DIRECTORY_DIRECTORYSHORTCUT_H_
#define DIRECTORY_DIRECTORYSHORTCUT_H_

#include <ylstring.h>
#include <yllist.h>

struct DirShortcutListData;
struct DirectoryShortcutData;

// 联系人快捷方式类, 用于配置通话界面的快捷菜单.
class CDirectoryShortcut
{
public:
    // 获取唯一实例
    static CDirectoryShortcut * GetInstance() DECLARE_ATTR;

    // 获取快捷菜单列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetShortcutList(DirShortcutListData * pData);

    // 重新加载配置文件.
    bool ReloadConfigure();

    // 初始化
    bool Init();

private:
    bool LoadConfigure();
    CDirectoryShortcut();
    ~CDirectoryShortcut();

private:

    YLList<DirectoryShortcutData> m_listData;
};


#define g_pDirectoryShortcut (CDirectoryShortcut::GetInstance())

#endif  // DIRECTORY_DIRECTORYSHORTCUT_H_

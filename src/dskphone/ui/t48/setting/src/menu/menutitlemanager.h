#ifndef MENU_MENUTITLEMANAGER_H_
#define MENU_MENUTITLEMANAGER_H_

#include <map>
#include <ylstring.h>

#include "commondefine.h"

namespace NS_MENU
{
// Class CMenuTitleManager, 管理菜单的标题.
class CMenuTitleManager
{
public:
    // 页面标题map.
    // Key: click action, value: 页面标题.
    typedef std::map<yl::string, yl::string> TitleMap;
    typedef TitleMap::iterator TitleMapIter;
    typedef TitleMap::const_iterator TitleMapCIter;

    // 创建单键.
    static CMenuTitleManager * GetInstance();
    // 销毁单键.
    static void ReleaseInstance();

    // 增加页面标题.
    // | strClickAction | 是click action.
    // | strTitle | 是页面标题.
    void SetTitle(const yl::string & strClickAction,
                  const yl::string & strTitle);
    // 删除页面标题.
    // | strClickAction | 是click action.
    void RemoveTitle(const yl::string & strClickAction);

    // 根据click action获取页面标题.
    // | strClickAction | 是click action.
    // 获取成功则返回页面标题, 否则返回空.
    yl::string GetTitleByClickAction(const yl::string & strClickAction) const;

private:
    // 构造函数.
    CMenuTitleManager();
    // 析构函数.
    ~CMenuTitleManager();

private:
    static CMenuTitleManager * m_pInst; // 单键实例.
    TitleMap m_mapTitle;  // 标题map.

    DISALLOW_COPY_AND_ASSIGN(CMenuTitleManager);
};

}  // namespace NS_MENU

#define g_pMenuTitleManager (NS_MENU::CMenuTitleManager::GetInstance())

#endif  // MENU_MENUTITLEMANAGER_H_

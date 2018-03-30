#ifndef SETTING_CONTRAST_H_
#define SETTING_CONTRAST_H_

#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;

namespace NS_SETTING
{
class CContrastController
{
public:
    // 获取单件
    static CContrastController * GetInstance();
    // 释放单件
    static void ReleaseInstance();

    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // 获取当前对比度
    int GetCurrentContrastLevel();
    // 设置对比度
    void SetCurrentContrastLevel(int iLevel);

private:
    CContrastController();
    ~CContrastController();

private:

    static CContrastController * m_pInst; // 单件

    DISALLOW_COPY_AND_ASSIGN(CContrastController);
};

}  // namespace NS_SETTING

#define g_pContrastController (NS_SETTING::CContrastController::GetInstance())

#endif  // SETTING_CONTRAST_H_

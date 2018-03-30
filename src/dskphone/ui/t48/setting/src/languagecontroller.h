#ifndef SETTING_LANGUAGECONTROLLER_H_
#define SETTING_LANGUAGECONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct LanguageItemData;
struct LanguageItemListData;
struct MenuItemListData;
class CLangInfo;

namespace NS_SETTING
{
class CLanguageModel;

// Class CLanguageController
class CLanguageController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CLanguageController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get page menu list by click action.
    // | pData | [out] is the data retrieved.
    // | strClickAction | is the click action, it must begin with OpenPage.
    // Such as "OpenPage("account_list.xml")".
    // Returns true if successfully, otherwise return false.
    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // Get Language Name Data.
    // | pData | is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetLanguageList(LanguageItemListData * pData) const;
    // Get current language name.
    const yl::string GetCurrentLanguage() const;
    // Set current language name.
    // | strLanguage | is the language name to set.
    void SetCurrentLanguage(const yl::string & strLanguage);

    // 校验语言是否合法
    void VeryfyLanguage();

private:
    // Ctor and Dtor.
    CLanguageController();
    ~CLanguageController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

    // 从数组中查找指定语言,返回是否找到
    bool FindLanguage(const YLVector<CLangInfo> & arrayLang, const yl::string strLanguage);

private:
    CLanguageModel * m_pLanguageModel;  // The language model instance.
    static CLanguageController * m_pLanguageController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CLanguageController);
};

}  // namespace NS_SETTING

#define g_pLanguageController (NS_SETTING::CLanguageController::GetInstance())

#endif  // SETTING_LANGUAGECONTROLLER_H_

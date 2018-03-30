#ifndef SETTING_LANGUAGECONTROLLER_H_
#define SETTING_LANGUAGECONTROLLER_H_

#include <ylstring.h>
#include "language/translate/include/translate.h"
#include "commondefine.h"

struct LanguageItemData;
struct LanguageItemListData;
class CLangInfo;

class CLanguageModel;

// Class CLanguageController
class CLanguageController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CLanguageController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get Language Name Data.
    // | pData | is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetLanguageList(VectorLangInfo & objLangArray) const;
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

    // 从数组中查找指定语言,返回是否找到
    bool FindLanguage(const VectorLangInfo & arrayLang, const yl::string & strLanguage);

private:
    static CLanguageController * s_pLanguageController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CLanguageController);
};

#define g_pLanguageController (CLanguageController::GetInstance())

#endif  // SETTING_LANGUAGECONTROLLER_H_

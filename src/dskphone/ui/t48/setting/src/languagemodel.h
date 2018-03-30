#ifndef SETTING_LANGUAGEMODEL_H_
#define SETTING_LANGUAGEMODEL_H_

#include <ylstring.h>

namespace NS_SETTING
{
// Class CLanguageModel
class CLanguageModel
{
public:
    // Get current language name.
    const yl::string GetCurrentLanguage() const;
    // Set current language name.
    // | strLanguage | is the language name to set.
    bool SetCurrentLanguage(const yl::string & strLanguage);

private:
    // Ctor and Dtor.
    CLanguageModel();
    ~CLanguageModel();

private:
    friend class CLanguageController;
};

}   // namespace NS_SETTING

#endif  // SETTING_LANGUAGEMODEL_H_

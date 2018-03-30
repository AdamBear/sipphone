#ifndef __SETTINGUI_LANGUAGE_DELEGATE_H__
#define __SETTINGUI_LANGUAGE_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "language/translate/include/translate.h"

class CSettingUILanguageDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUILanguageDelegate();
    virtual ~CSettingUILanguageDelegate();

    // 获取Input Delegate对像
    static CSettingUIDelegateBase * CreateLanguageDelegate();

    // 加载数据
public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    // 保存数据
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

    virtual bool OnPageAction(int eActionType, int iDataIndex);

    // 判断界面数据是否被修改过
    virtual bool IsPageValueChange(CSettingUIPageDataBase * pPagedata);

    int GetChoseIndex();

private:
    int SelectedLanguageIndex(const VectorLangInfo & vecLang, const yl::string & strSelectedLanguage);

private:
    VectorLangInfo      m_vecLang;                  // 语言列表
    int                 m_iConfigSelectIndex;       // 配置项序号
#ifdef IF_SETTING_WIZARD
    bool                m_bInGuide;
#endif
};


typedef CSettingUILanguageDelegate * CSettingUILanguageDelegatePtr;

#endif // __SETTINGUI_LANGUAGE_DELEGATE_H__

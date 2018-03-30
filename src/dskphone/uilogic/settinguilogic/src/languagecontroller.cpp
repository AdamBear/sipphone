#include "languagecontroller.h"
#include "settinguilogic/include/common.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "settinguilogic/include/messagedefine.h"
#include "interfacedefine.h"
#include "adapterbox/include/modadapterbox.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

//消息回调函数
static LRESULT OnLanguageChangedMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            //配置改变消息
            if (ST_LANG == objMessage.wParam)
            {
                yl::string strLanguage = g_pLanguageController->GetCurrentLanguage();

                // 重新加载翻译语言
                _TransLogic.Reload();

                // 容错，选中语言若被删除则选中默认语言
                g_pLanguageController->VeryfyLanguage();

                AdapterBox_ReloadIME();
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// Class CLanguageController
CLanguageController * CLanguageController::s_pLanguageController = NULL;

CLanguageController * CLanguageController::GetInstance()
{
    if (NULL == s_pLanguageController)
    {
        s_pLanguageController = new CLanguageController();
        if (NULL != s_pLanguageController)
        {
            s_pLanguageController->VeryfyLanguage();
        }
    }

    return s_pLanguageController;
}

void CLanguageController::ReleaseInstance()
{
	if (NULL != s_pLanguageController)
	{
		delete s_pLanguageController;
		s_pLanguageController = NULL;
	}
}

CLanguageController::CLanguageController()
{
    //注册消息
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnLanguageChangedMsg);
}

CLanguageController::~CLanguageController()
{
    // 注销注册的消息
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnLanguageChangedMsg);
}

bool CLanguageController::GetLanguageList(VectorLangInfo & objLangArray) const
{
    return CTransLogic::GetAllLangInfo(objLangArray);
}

const yl::string CLanguageController::GetCurrentLanguage() const
{
	// Read current language.
	return configParser_GetConfigString(kszCurrentLang);
}

void CLanguageController::SetCurrentLanguage(const yl::string & strLanguage)
{
    // Call model's method to set current language.
    if (GetCurrentLanguage() != strLanguage)
    {
        //language change
		if (configParser_SetConfigString(kszCurrentLang, strLanguage.c_str(), CONFIG_LEVEL_PHONE))
        {
            // 重新加载语言
            _TransLogic.Reload();

            AdapterBox_ReloadIME();
        }
    }
}

// 校验语言是否合法
void CLanguageController::VeryfyLanguage()
{
    VectorLangInfo listLang;
    if (CTransLogic::GetAllLangInfo(listLang))
    {
        // 标记是否找到指定语言
        bool bFindLanguage = false;

        yl::string strCurrentLanguage = GetCurrentLanguage();
        if (!strCurrentLanguage.empty())
        {
            bFindLanguage = FindLanguage(listLang, strCurrentLanguage);
        }

        // 如果找到,则说明设置的语言合法
        if (bFindLanguage)
        {
            return;
        }

        // 如果没找到,则使用默认语言英语来查找
        bFindLanguage = FindLanguage(listLang, "English");
        if (bFindLanguage)
        {
            // 如果找到,则将语言修改为英语
            SetCurrentLanguage("English");
        }
        else if (listLang.size() > 0)
        {
            // 如果也不存在英文,则使用第一个语言
            SetCurrentLanguage(listLang[0]);
        }
    }
}

// 从数组中查找指定语言,返回是否找到
bool CLanguageController::FindLanguage(const VectorLangInfo & arrayLang,
                                       const yl::string & strLanguage)
{
    if (strLanguage.empty())
    {
        return false;
    }

    for (int i = 0; i < arrayLang.size(); ++i)
    {
        if (0 == strcmp(arrayLang[i].c_str(), strLanguage.c_str()))
        {
            // 设置的语言存在语言文件
            return true;
        }
    }  // for.

    return false;
}


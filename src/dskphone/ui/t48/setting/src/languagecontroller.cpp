#include "uikernel/languagehelper.h"
#include "languagecontroller.h"

#include "setting/include/common.h"

#include "basemethod.h"
#include "languagemodel.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "setting/include/messagedefine.h"
#include "interfacedefine.h"
#include "language/input/include/modInput.h"
#include "language/translate/include/translate.h"

extern void qReloadIME();

//消息回调函数
static LRESULT OnLanguageChangedMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            //配置改变消息
            if (objMessage.wParam == ST_LANG)
            {
                yl::string strLanguage = g_pLanguageController->GetCurrentLanguage();
                // 发送消息
                etl_NotifyAppEx(false, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                                0, 0, (int)strLanguage.length() + 1, strLanguage.c_str());

                // 容错，选中语言若被删除则选中默认语言
                g_pLanguageController->VeryfyLanguage();

                qReloadIME();

                etl_NotifyApp(true, LANGUAGE_MESSAGE_LANGUAGE_CHANGED, 0, 0);
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CLanguageController
CLanguageController * CLanguageController::m_pLanguageController = NULL;

CLanguageController * CLanguageController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pLanguageController == NULL)
    {
        m_pLanguageController = new CLanguageController();
        if (m_pLanguageController != NULL)
        {
            // Do initialization.
            m_pLanguageController->MyInit();
        }
    }
    return m_pLanguageController;
}

void CLanguageController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pLanguageController != NULL)
    {
        // Do finalization.
        m_pLanguageController->MyFinal();

        delete m_pLanguageController;
        m_pLanguageController = NULL;
    }
}

bool CLanguageController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CLanguageController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CLanguageController::GetLanguageList(LanguageItemListData * pData) const
{
    if (m_pLanguageModel == NULL)
    {
        return false;
    }

    // 从语言文件助手中获取语言文件列表.
    VectorLang listLang;
    LanguageItemData stLID;
    if (CLanguageHelper::GetAllLangInfo(listLang))
    {
        // 拷贝语言列表.
        for (int i = 0; i < listLang.size(); ++i)
        {
            CLangInfo & refLang = listLang[i];
            stLID.Reset();
            stLID.m_strOrigName = refLang.strOrigName;
            stLID.m_strTranslatedName = refLang.strTranName;
            stLID.m_strLangFile = refLang.strLangFile;
            pData->m_listLang.push_back(stLID);
        }  // for.
        return true;
    }
    return false;
}

const yl::string CLanguageController::GetCurrentLanguage() const
{
    // Call model's method to get current language.
    return m_pLanguageModel ? m_pLanguageModel->GetCurrentLanguage() : "";
}

void CLanguageController::SetCurrentLanguage(const yl::string & strLanguage)
{
    // Call model's method to set current language.
    if (GetCurrentLanguage() != strLanguage)
    {
        //language change
        if (m_pLanguageModel && m_pLanguageModel->SetCurrentLanguage(strLanguage))
        {
            //语言翻译注销
            qReloadIME();
            // Post message.
            etl_NotifyAppEx(false, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                            0, 0, (int)strLanguage.length() + 1, strLanguage.c_str());
        }
    }
}

CLanguageController::CLanguageController()
    : m_pLanguageModel(NULL)
{
    //注册消息
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnLanguageChangedMsg);
}

CLanguageController::~CLanguageController()
{
    // 注销注册的消息
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnLanguageChangedMsg);
}

// Initialize.
bool CLanguageController::MyInit()
{
    // Initialize the language model.
    if (m_pLanguageModel == NULL)
    {
        m_pLanguageModel = new CLanguageModel();
    }

    VeryfyLanguage();
    return m_pLanguageModel != NULL;
}

// Finalize.
bool CLanguageController::MyFinal()
{
    // Release the language model.
    if (m_pLanguageModel != NULL)
    {
        delete m_pLanguageModel;
        m_pLanguageModel = NULL;
    }
    return m_pLanguageModel == NULL;
}

// 校验语言是否合法
void CLanguageController::VeryfyLanguage()
{
    VectorLangInfo listLang;
    if (CTransLogic::GetAllLangInfo(listLang))
    {
        yl::string strCurrentLanguage = GetCurrentLanguage();
        if (!strCurrentLanguage.empty())
        {
            for (VectorLangInfo::iterator it = listLang.begin(); it != listLang.end(); ++it)
            {
                if (*it == strCurrentLanguage)
                {
                    return;
                }
            }
        }

        LPCSTR lpDefaultLang = "English";
        for (VectorLangInfo::iterator it = listLang.begin(); it != listLang.end(); ++it)
        {
            if (*it == lpDefaultLang)
            {
                SetCurrentLanguage(lpDefaultLang);
                return;
            }
        }

        if (listLang.size() > 0)
        {
            // 如果也不存在英文,则使用第一个语言
            SetCurrentLanguage(listLang[0]);
        }
    }
}

// 从数组中查找指定语言,返回是否找到
bool CLanguageController::FindLanguage(const VectorLang & arrayLang, const yl::string strLanguage)
{
    if (strLanguage.empty())
    {
        return false;
    }

    for (int i = 0; i < arrayLang.size(); ++i)
    {
        const CLangInfo & refLang = arrayLang[i];
        if (0 == strcmp(refLang.strOrigName.c_str(), strLanguage.c_str()))
        {
            // 设置的语言存在语言文件
            return true;
        }
    }  // for.

    return false;
}

}   // namespace NS_SETTING

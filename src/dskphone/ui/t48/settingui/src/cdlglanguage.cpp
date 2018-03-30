#include "model.h"
#include "cdlglanguage.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/languagecontroller.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "messagebox/modmessagebox.h"
#include "language/translate/include/translate.h"

CDlgLanguage::CDlgLanguage(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

void CDlgLanguage::InitData()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    m_pGeneralFrame->SetDefaultFocus(false);

    QWidgetPair qPair;
    qPair.eType = SETTING_ITEM_RIDIO;

    m_vecOrigName.clear();

    /*
    LanguageItemListData ListData;
    if (!g_pLanguageController->GetLanguageList(&ListData))
    {
        return;
    }

    LanguageItemData stLID;
    YLList<LanguageItemData>::const_iterator pDataIter = ListData.m_listLang.begin();
    for (; pDataIter != ListData.m_listLang.end(); ++pDataIter)
    {
        stLID = *pDataIter;
        if (!stLID.m_strOrigName.empty())
        {
            //实现语言名称翻译
            qPair.first = LANG_TRANSLATE(stLID.m_strOrigName.c_str());
            m_vecWidgetPair.push_back(qPair);

            m_vecOrigName.push_back(stLID.m_strOrigName);
        }
    }*/

    VectorLangInfo vecLangInfo;
    if (!CTransLogic::GetAllLangInfo(vecLangInfo))
    {
        return;
    }

    for (VectorLangInfo::iterator it = vecLangInfo.begin(); it != vecLangInfo.end(); ++it)
    {
        if (it->empty())
        {
            continue;
        }

        qPair.first = LANG_TRANSLATE(it->c_str());
        m_vecWidgetPair.push_back(qPair);
        m_vecOrigName.push_back(*it);
    }

    if (m_vecWidgetPair.size() > 0)
    {
        AddWidget(m_vecWidgetPair, true, false, true, false);
    }

    this->installEventFilter(this);
}

bool CDlgLanguage::IsStatusChanged()
{
    if (NULL == m_pGeneralFrame
            || NULL == m_pListDelegate)
    {
        return false;
    }

    int iSelectIndex = m_pListDelegate->GetCurrentIndex();
    if (iSelectIndex >= 0 && iSelectIndex < m_vecOrigName.size())
    {
        if (m_vecOrigName[iSelectIndex]
                != g_pLanguageController->GetCurrentLanguage())
        {
            return true;
        }
    }
    return false;
}

bool CDlgLanguage::LoadPageData()
{
    this->setFocus();

    if (m_pGeneralFrame != NULL)
    {
        yl::string strCurrentLanguage = g_pLanguageController->GetCurrentLanguage();

        for (int i = 0; i < m_vecOrigName.size(); i++)
        {
            if (strCurrentLanguage == m_vecOrigName[i])
            {
                SetCurrentIndex(i);
                SetConfigedIndex(i);
                return true;
            }
        }
    }
    return false;
}

bool CDlgLanguage::SavePageData()
{
    if (IsStatusChanged())
    {
        QTimer::singleShot(20, this, SLOT(OnSaveLanguageData()));
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_INITIALIZING_LANGUAGE_PLEASE_WAIT),
                                    MESSAGEBOX_NOTE);

        //保存语言文件，同时弹出提示框提示正在初始化语言，保存成功后，销毁弹出框
        return false;
    }
    else
    {
        return true;
    }
}

bool CDlgLanguage::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!isChildOf(pObject, this) && pObject != this)
    {
        return false;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgLanguage::OnSaveLanguageData()
{
    if (m_pListDelegate == NULL || m_pGeneralFrame == NULL)
    {
        return;
    }

    int iSelectIndex = m_pListDelegate->GetCurrentIndex();
    if (iSelectIndex >= 0 && iSelectIndex < m_vecOrigName.size())
    {
        g_pLanguageController->SetCurrentLanguage(m_vecOrigName[iSelectIndex]);
        SetConfigedIndex(iSelectIndex);
    }

    MessageBox_RemoveTopMessageBox(MessageBox_OK);

    emit BtnBackClick();
}

CDlgLanguage::~CDlgLanguage()
{
}

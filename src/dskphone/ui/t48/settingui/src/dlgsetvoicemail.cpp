#include "dlgsetvoicemail.h"
#include "setting/src/voicemailcontroller.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "keymap.h"
#include "uimanager/moduimanager.h"
#include "keyguard/include/modkeyguard.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/qmisc.h"
#include "account/include/modaccount.h"

CDlgSetVoiceMail::CDlgSetVoiceMail(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgSetVoiceMail::~CDlgSetVoiceMail()
{

}

void CDlgSetVoiceMail::InitData()
{
    VoiceMailConfigListData listData;
    //QLabel *pLabel = NULL;
    QLineEdit * pEditSetVoiceMailNumber = NULL;

    if (g_pVoiceMailController->GetConfigList(&listData))
    {
        std::vector<QWidget *> vecpWgt;
        YLList<VoiceMailConfigData>::iterator it = listData.m_listCfg.begin();
        YLList<VoiceMailConfigData>::iterator it_end = listData.m_listCfg.end();
        for (int i = 0; i < listData.m_listCfg.size(); i++)
        {
            QString labelText = toQString(acc_GetAccountShowText(i));

            pEditSetVoiceMailNumber = new QLineEdit(this);
            if (pEditSetVoiceMailNumber != NULL)
            {
                qLineEditUtility::SetLineEditStyle(pEditSetVoiceMailNumber, Qt::black);
                //设置输入法
                qInstallIME(pEditSetVoiceMailNumber, IME_ALL);
                qSetCurrentIME(pEditSetVoiceMailNumber, IME_123);
                //最大输入长度
                qLineEditUtility::SetMaxLength(pEditSetVoiceMailNumber, EDIT_TYPE_NAME);
                pEditSetVoiceMailNumber->installEventFilter(this);

                //设置item账号信箱信息
                if (it != it_end)
                {
                    pEditSetVoiceMailNumber->setText(toQString((*it).m_strMailNumber.c_str()));
                    ++it;
                }

                //pEditSetVoiceMailNumber->setGeometry(0, 0, 258, 34);

                //加入管理链表
                aEditVoiceMailNumber.push_back(pEditSetVoiceMailNumber);

                // 加入焦点管理数组
                vecpWgt.push_back(pEditSetVoiceMailNumber);

                QWidgetPair qPair;
                qPair.first = labelText;
                qPair.second = pEditSetVoiceMailNumber;

                //V81Beta 统一显示名称，统一从acc_GetAccountShowText获取 不显示注册不成功的账号
                if (!acc_IsAccountAvailable(i))
                {
                    pEditSetVoiceMailNumber->hide();
                    continue;
                }

                m_vecWidgetPair.push_back(qPair);
            }
        }
    }
    AddWidget(m_vecWidgetPair);

    if (keyGuard_IsEnable() && keyGuard_IsLock())
    {
        QWidget * pWidget = NULL;
        for (int i = 0; i < m_vecWidgetPair.size(); i++)
        {
            pWidget = m_vecWidgetPair[i].second;
            if (NULL != pWidget)
            {
                pWidget->setEnabled(false);
            }
        }
        pWidget = NULL;
    }
}


bool CDlgSetVoiceMail::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    QWPVector::iterator iter = m_vecWidgetPair.begin();
    for (; iter != m_vecWidgetPair.end(); ++iter)
    {
        if (iter->second != NULL && iter->second->inherits("QLineEdit")
                && iter->second->isEnabled() == true)
        {
            QLineEdit * pEdit = (QLineEdit *)(iter->second);
            if (!pEdit->text().isEmpty()
                    && !qLineEditUtility::IsFormatValid(pEdit->text(), EDIT_TYPE_PHONE_NUMBER_LONG))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_VOICE_NUMBER_IS_INVALID);
                objErrorContent.pLineEdit = pEdit;
                return false;
            }
        }
    }
    return true;
}

bool CDlgSetVoiceMail::LoadPageData()
{
    this->setFocus();
    return true;
}

// 查看页面状态是否改变
bool CDlgSetVoiceMail::IsStatusChanged()
{
    VoiceMailConfigListData listData;
    QLineEdit * pEditSetVoiceMailNumber = NULL;

    if (g_pVoiceMailController->GetConfigList(&listData))
    {
        YLList<VoiceMailConfigData>::iterator it = listData.m_listCfg.begin();
        YLList<VoiceMailConfigData>::iterator it_end = listData.m_listCfg.end();
        for (int i = 0; i < listData.m_listCfg.size(); i++)
        {
            pEditSetVoiceMailNumber = aEditVoiceMailNumber[i];
            if (pEditSetVoiceMailNumber != NULL)
            {
                if (it != it_end)
                {
                    if ((*it).m_strMailNumber != pEditSetVoiceMailNumber->text().toUtf8().data())
                    {
                        return true;
                    }
                    ++it;
                }
            }
        }
    }
    //页面数据没有改变
    return false;
}

// 保存页面数据
bool CDlgSetVoiceMail::SavePageData()
{
    VoiceMailConfigListData listData;
    QLineEdit * pEditSetVoiceMailNumber = NULL;

    for (int i = 0; i < aEditVoiceMailNumber.size(); i++)
    {
        pEditSetVoiceMailNumber = aEditVoiceMailNumber[i];
        if (pEditSetVoiceMailNumber != NULL)
        {
            VoiceMailConfigData configData;
            configData.m_strMailNumber = pEditSetVoiceMailNumber->text().toUtf8().data();
            listData.m_listCfg.push_back(configData);
        }
    }

    g_pVoiceMailController->SetConfigList(listData);
    return true;
}

bool CDlgSetVoiceMail::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }
    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent * pKeyEvt = static_cast<QKeyEvent *>(pEvent);
        if (pKeyEvt->key() == PHONE_KEY_MESSAGE)
        {
            return true;
        }
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

//根据账号id设置编辑框焦点
void CDlgSetVoiceMail::SetVoiceMailNumberFocus(int nAccountId)
{
//  QLineEdit *pEditSetVoiceMailNumber = NULL;
//  pEditSetVoiceMailNumber = aEditVoiceMailNumber[nAccountId];
//  if (pEditSetVoiceMailNumber != NULL)
//  {
//      pEditSetVoiceMailNumber->setFocus();
//  }

    if (m_pGeneralFrame != NULL)
    {
        if (m_pGeneralFrame->GetTotalDataCount() < 1 || !this->isVisible())
        {
            SetCurrentIndex(nAccountId);
        }
        else
        {
            SelectIndex(nAccountId);
        }
    }

}

void CDlgSetVoiceMail::SetDefaultIndex(int nIndex/* = 0*/, bool bEnter/* = false*/)
{
    for (int iAccount = nIndex - 1; iAccount >= 0; iAccount--)
    {
        if (!acc_IsAccountAvailable(iAccount))
        {
            nIndex--;
        }
    }
    CDlgBaseSubPage::SetDefaultIndex(nIndex, bEnter);
}

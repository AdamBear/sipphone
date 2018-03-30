#include "dlgviewvoicemail.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uimanager/moduimanager.h"
#include "setting/src/voicemailcontroller.h"
#include "setting/include/common.h"
#include "account/include/modaccount.h"
#include "baseui/t4xpicpath.h"
#include "settingui/include/modsettingui.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "dlgsetvoicemail.h"
#include "actiondefinecommon.h"
#include "messagebox/modmessagebox.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#endif

///////////////////////////////////////////////////////////////////////////////
CDlgViewVoiceMail::CDlgViewVoiceMail(QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgViewVoiceMail::~CDlgViewVoiceMail()
{

}

//页面数据初始化
void CDlgViewVoiceMail::InitData()
{
    m_vecLabStatus.clear();
    m_vecAccountId.clear();

    int iAcountNum = acc_AccountNum();
    for (int i = 0; i < iAcountNum; i++)
    {
        QLabel * pItemLab = new QLabel(this);
        if (pItemLab != NULL)
        {
            SetWgtFontColor(pItemLab, FONT_TYPE_TEXT);
            pItemLab->raise();
            pItemLab->show();
            m_vecLabStatus.push_back(pItemLab);
        }
    }
    if (m_vecLabStatus.size() != iAcountNum)
    {
        return;
    }
    /////////////////////////////////////////////////////////////////////////
    VoiceMailInfoListData listData;
    if (g_pVoiceMailController->GetInfoList(&listData))
    {
        YLList<VoiceMailInfoData>::iterator it = listData.m_listInfo.begin();
        YLList<VoiceMailInfoData>::iterator it_end = listData.m_listInfo.end();

        QString strAcount = LANG_TRANSLATE(TRID_ACCOUNT);
        VoiceMailInfoData data;

        for (int i = 0; i < iAcountNum && it != it_end; ++i, ++it)
        {
            QString strLab;

            data = *it;
            QString strMessageInfo = "";

            //没注册上的不显示 V81Beta版本需求
            if (data.m_nAccountStaus != LS_REGISTERED)
            {
                continue;
            }

            //判断是否注册成功
            switch (data.m_nAccountStaus)
            {
            case LS_SYSTEM_DEFAULT_ACCOUNT:
            case LS_REGISTERED:
            case LS_DISABLE_REG:
                {
                    strLab = toQString(data.m_strAccountDisplayName.c_str());
                    if (configParser_GetConfigInt(kszHideUnreadVMNumber) == 1)
                    {
                        if (data.m_nUnReadCount > 0)
                        {
                            strMessageInfo = LANG_TRANSLATE(TRID_NEW_UNREAD_VOICE_MAIL);
                        }
                    }
                    else
                    {
                        strMessageInfo = QString::number(data.m_nUnReadCount) + QString(" ") + LANG_TRANSLATE(TRID_NEW)
                                         + QString(" ") + QString::number(data.m_nReadCount) + QString(" ") + LANG_TRANSLATE(TRID_OLD)
                                         + QString(" ") + LANG_TRANSLATE(TRID_MAIL);
                    }

                }
                break;
            case LS_BE_UNREGISTERED:
            case LS_DISABLE:
                {
                    //根据账号组合显示信息
                    strLab = strAcount + QString::number(i + 1);
                    strMessageInfo = LANG_TRANSLATE(TRID_UNREGISTERED);
                }
                break;
            case LS_REGISTER_FAIL:
            case LS_UNREGISTERING:
            case LS_REGISTERING:
            default:
                {
                    strLab = strAcount + QString::number(i + 1);
                    strMessageInfo = LANG_TRANSLATE(TRID_REGISTER_FAIL);
                }
                break;
            }
            m_vecLabStatus[i]->setText(strMessageInfo);

            //V81Beta 统一显示名称，统一从acc_GetAccountShowText获取 不显示注册不成功的账号
            strLab = toQString(acc_GetAccountShowText(i));

            QWidgetPair qPair;
            qPair.first = strLab;
            qPair.eType = SETTING_ITEM_MENU;
#if IF_FEATURE_BROADSOFT_VOICEMAIL
            if (vvMail_IsEnable())
            {
                qPair.third = ACTION_ENTER;
            }
            else
#endif
            {
                qPair.third = ACTION_CONNECT;
            }
            qPair.second = m_vecLabStatus[i];

            m_vecWidgetPair.push_back(qPair);
            m_vecAccountId.push_back(i);
        }

        AddWidget(m_vecWidgetPair, true, false, true, false);
    }

    this->installEventFilter(this);
}

bool CDlgViewVoiceMail::LoadPageData()
{
    this->setFocus();
    return true;
}

bool CDlgViewVoiceMail::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_pGeneralFrame == NULL || objWndData.size() < 4)
    {
        return false;
    }

    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

bool CDlgViewVoiceMail::OnAction(const QString & strAction)
{
    if (strAction == ACTION_CONNECT && m_pGeneralFrame != NULL)
    {
        //连接语音邮箱
        if (m_pGeneralFrame->GetFocusItemDataIndex() < 0
                || m_pGeneralFrame->GetFocusItemDataIndex() >= m_vecAccountId.size())
        {
            return false;
        }
        VoiceMailCallOut(m_vecAccountId.at(m_pGeneralFrame->GetFocusItemDataIndex()));
        return true;
    }
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    else if (strAction == ACTION_ENTER && m_pGeneralFrame != NULL)
    {
        if (m_pGeneralFrame->GetFocusItemDataIndex() < 0
                || m_pGeneralFrame->GetFocusItemDataIndex() >= m_vecAccountId.size())
        {
            return false;
        }
        EnterVisualVoiceMail(m_vecAccountId.at(m_pGeneralFrame->GetFocusItemDataIndex()));
        return true;
    }
#endif

    return false;
}

bool CDlgViewVoiceMail::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!isChildOf(pObject, this) && pObject != this)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_MESSAGE:
                //case PHONE_KEY_OK:
                {
                    //!!!!!
                    if (m_pGeneralFrame != NULL)
                    {
#if IF_FEATURE_BROADSOFT_VOICEMAIL
                        if (configParser_GetConfigInt(kszVoiceMailMessageKeyMode) == 1)
                        {
                            return true;
                        }
#endif
                        return VoiceMailCallOut(m_pGeneralFrame->GetFocusItemDataIndex());
                    }
                }
                return true;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgViewVoiceMail::VoiceMailCallOut(int nAccountId)
{
    VoiceMailInfoListData listData;
    //账号是否可用
    bool bIsUsed = false;
    //语音信箱号码是否设置
    bool bIsSetVoiceMailNumber = false;
    if (g_pVoiceMailController->GetInfoList(&listData))
    {
        YLList<VoiceMailInfoData>::iterator it = listData.m_listInfo.begin();
        YLList<VoiceMailInfoData>::iterator it_end = listData.m_listInfo.end();
        for (int i = 0; it != it_end; i++, ++it)
        {
            if (nAccountId == i)
            {
                VoiceMailInfoData & infoData = *it;
                if (infoData.m_strMailNumber == "")
                {
                    bIsSetVoiceMailNumber = false;
                }
                else
                {
                    bIsSetVoiceMailNumber = true;
                }
                break;
            }
        }
    }

    VoiceMailItemClicked(nAccountId, bIsSetVoiceMailNumber);
    return true;
}

#if IF_FEATURE_BROADSOFT_VOICEMAIL
bool CDlgViewVoiceMail::EnterVisualVoiceMail(int nAccountId)
{
    SubMenuData subMenuData;
    subMenuData.strMenuAction = "OpenSubPage(visual_voice_mail_list)";
    subMenuData.pBeforeSetSubpageData = (void *)&nAccountId;
    emit openpage(subMenuData);
    return true;
}
#endif

//点击item响应插槽函数
void CDlgViewVoiceMail::VoiceMailItemClicked(int nAccountId, bool bIsSetVoiceMailNumber)
{
    if (bIsSetVoiceMailNumber)
    {
        int eAccountStatus = acc_GetState(nAccountId);
        if (eAccountStatus != LS_REGISTERED && eAccountStatus != LS_SYSTEM_DEFAULT_ACCOUNT)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_ACCOUNT_NOT_USABLE), MESSAGEBOX_NOTE,
                                        LONGER_TIME);
            return;
        }
        //调用控制层：呼叫语音信箱号码
        g_pVoiceMailController->ClickInfoByIndex(nAccountId);
    }
    else
    {
        emit openpage(SubMenuData("OpenSubPage(set_voice_mail)", nAccountId));
    }
}

//根据账号id设置编辑框焦点
void CDlgViewVoiceMail::SetVoiceMailNumberFocus(int nAccountId)
{
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


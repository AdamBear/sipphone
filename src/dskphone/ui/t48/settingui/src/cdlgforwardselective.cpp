#include "cdlgforwardselective.h"
#include "uikernel/languagehelper.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "setting/src/menu/menucontroller.h"
#include "messagebox/modmessagebox.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "modsettingui.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "uicommon.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

static LRESULT OnMsgForwardSelective(msgObject & objMessage)
{
    SETTINGUI_INFO("OnMsgForwardSelective [%#x] [%#x]", objMessage.message, objMessage.wParam);

    CDlgForwardSelective * pPage = qobject_cast<CDlgForwardSelective *>(UIManager_GetPrivateDlg(
                                       DLG_CDlgForwardSelective));
    if (NULL == pPage || !pPage->IsTopPage())
    {
        return 0;
    }

    pPage->OnReceiveMsg(objMessage);
    pPage->SetCurrentIndex(0);
    pPage->UpdateWnd();

    return 0;
}

CDlgForwardSelective::CDlgForwardSelective(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent), m_bIsInLoad(false), m_iAccountId(0)
{
}

CDlgForwardSelective::~CDlgForwardSelective()
{
}

void CDlgForwardSelective::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnMsgForwardSelective);
}

void CDlgForwardSelective::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnMsgForwardSelective);
}

void CDlgForwardSelective::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_FORWARD_SELECTIVE_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                OnGetForwardSelectiveData();
            }
            else if (XSI_FORWARD_SELECTIVE_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_FORWARD_SELECTIVE_GET != objMessage.wParam
                    && XSI_FORWARD_SELECTIVE_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            if (m_bIsInLoad)
            {
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
            }
            else
            {
                // 如果所有的账号都下载失败，则退出
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVE_FAILED),
                                            MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
            }
        }
        break;
    default:
        break;
    }
}

bool CDlgForwardSelective::LoadPageData()
{
    m_strTitle = LANG_TRANSLATE(TRID_FORWARD_SELECTIVE);
}

void CDlgForwardSelective::OnGetForwardSelectiveData()
{
    m_bIsInLoad = false;
    ClearAllContent();

    FS_GetForwardSelectiveInfo(m_sFsInfo, m_iAccountId);

    QWidgetPair qPair;

    QComboBox * pCombActive = new QComboBox(this);
    qComboBoxUtility::SetComboBoxStyle(pCombActive, Qt::black);
    pCombActive->addItem(LANG_TRANSLATE(TRID_OFF), false);
    pCombActive->addItem(LANG_TRANSLATE(TRID_ON), true);
    pCombActive->setCurrentIndex(pCombActive->findData(m_sFsInfo.bActive, Qt::UserRole));
    qPair.first = LANG_TRANSLATE(TRID_FORWARD_SELECTIVE_ACTIVE);
    qPair.second = pCombActive;
    m_vecWidgetPair.push_back(qPair);

    QLineEdit * pEditFwdTo = new QLineEdit(this);
    SetLineEditDefaultStyle(pEditFwdTo);
    pEditFwdTo->setText(toQString(m_sFsInfo.strForwardToPhoneNumber));
    qPair.first = LANG_TRANSLATE(TRID_FORWARD_SELECTIVE_FORWARD_TO);
    qPair.second = pEditFwdTo;
    qInstallIME(pEditFwdTo, IME_DIGIT);
    m_vecWidgetPair.push_back(qPair);

    QComboBox * pCombPlayRing = new QComboBox(this);
    qComboBoxUtility::SetComboBoxStyle(pCombPlayRing, Qt::black);
    pCombPlayRing->addItem(LANG_TRANSLATE(TRID_OFF), false);
    pCombPlayRing->addItem(LANG_TRANSLATE(TRID_ON), true);
    pCombPlayRing->setCurrentIndex(pCombPlayRing->findData(m_sFsInfo.bPlayRingReminder, Qt::UserRole));
    qPair.first = LANG_TRANSLATE(TRID_FORWARD_SELECTIVE_PLAYRING);
    qPair.second = pCombPlayRing;
    m_vecWidgetPair.push_back(qPair);

    typedef SForwardSelectiveData::VecCriteriaItem VecItem;
    VecItem & vecItem = m_sFsInfo.vecCriteria;
    int iIndex = 1;
    for (VecItem::iterator it = vecItem.begin(); it != vecItem.end(); ++it)
    {
        const SForwardSelectiveDataCriteriaItem & sItem = *it;

        QComboBox * pCombLabel = new QComboBox(this);
        qComboBoxUtility::SetComboBoxStyle(pCombLabel, Qt::black);
        pCombLabel->addItem(LANG_TRANSLATE(TRID_OFF), false);
        pCombLabel->addItem(LANG_TRANSLATE(TRID_ON), true);
        pCombLabel->setCurrentIndex(pCombLabel->findData(sItem.bActive, Qt::UserRole));

        qPair.first = toQString(it->strName);
        qPair.second = pCombLabel;
        m_vecWidgetPair.push_back(qPair);

        pCombLabel->installEventFilter(this);
    }

    pCombActive->installEventFilter(this);
    pCombPlayRing->installEventFilter(this);
    pEditFwdTo->installEventFilter(this);
    this->installEventFilter(this);

    AddWidget(m_vecWidgetPair);
}

bool CDlgForwardSelective::SavePageData()
{
    m_bIsInLoad = false;

    if (FS_SetForwardSelective(m_sFsInfo, m_iAccountId))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVE_FAILED), MESSAGEBOX_Cancel,
                                    1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }

    return false;
}

// 设置子页面前的操作
void CDlgForwardSelective::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_iAccountId = *(int *)pData;
    }
    else
    {
        m_iAccountId = acc_GetDefaultAccount();
    }
}

void CDlgForwardSelective::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgForwardSelective::OnShow()
{
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return ;
        }

        FS_QueryForwardSelective(m_iAccountId);

        //表示处于加载中
        m_bIsInLoad = true;

        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    }
    else
    {
        m_bIsInLoad = false;
        // 如果所有的账号都下载失败，则退出
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
}

// 处理密码弹窗
bool CDlgForwardSelective::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);

        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }

    return false;
}

//处理XSI密码
void CDlgForwardSelective::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgForwardSelective::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

void CDlgForwardSelective::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();
    int iMsgType = pMessageBox->GetType();

    switch (iMsgID)
    {
    case MSGBOX_ID_POP_XSI_PWD:
        {
            if (iRet == MessageBox_OK)
            {
                yl::string strPwd = pMessageBox->GetExtraData().toString().toUtf8().data();
                OnSubmitXSIPassword(strPwd);
            }
            else if (iRet == MessageBox_Cancel)
            {
                OnCancelInputXSIPassword();
            }
            return;
        }
        break;
    default:
        break;
    }
}

bool CDlgForwardSelective::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    GetPageData();

    if (m_sFsInfo.bActive && m_sFsInfo.strForwardToPhoneNumber.empty())
    {
        int iIndex = 1;
        if (m_vecWidgetPair.size() > iIndex)
        {
            QWidget * pWidget = m_vecWidgetPair[iIndex].second;
            if (pWidget == NULL)
            {
                return false;
            }

            if (pWidget->inherits("QLineEdit"))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_NUMBER_CANNOT_BE_EMPTY);
                objErrorContent.pLineEdit = (QLineEdit *)pWidget;
                return false;
            }
        }
    }

    return true;
}

void CDlgForwardSelective::GetPageData()
{
    ReturnAllEdit();

    FS_GetForwardSelectiveInfo(m_sFsInfo, m_iAccountId);

    for (int iIndex = 0; iIndex < m_vecWidgetPair.size(); ++iIndex)
    {
        QWidget * pWidget = m_vecWidgetPair[iIndex].second;
        if (pWidget == NULL)
        {
            continue;
        }

        if (iIndex == 0 && pWidget->inherits("QComboBox"))
        {
            QComboBox * pComboBox = qobject_cast<QComboBox *>(pWidget);
            m_sFsInfo.bActive = pComboBox->itemData(pComboBox->currentIndex(), Qt::UserRole).value<bool>();
        }
        else if (iIndex == 1 && pWidget->inherits("QLineEdit"))
        {
            QLineEdit * pEdit = qobject_cast<QLineEdit *>(pWidget);
            m_sFsInfo.strForwardToPhoneNumber = pEdit->text().toUtf8().data();
        }
        else if (iIndex == 2 && pWidget->inherits("QComboBox"))
        {
            QComboBox * pComboBox = qobject_cast<QComboBox *>(pWidget);
            m_sFsInfo.bPlayRingReminder = pComboBox->itemData(pComboBox->currentIndex(),
                                          Qt::UserRole).value<bool>();
        }
        else if (iIndex >= 3 && pWidget->inherits("QComboBox"))
        {
            int iRealIndex = iIndex - 3;
            QComboBox * pComboBox = qobject_cast<QComboBox *>(pWidget);
            if (pComboBox != NULL && iRealIndex < m_sFsInfo.vecCriteria.size())
            {
                m_sFsInfo.vecCriteria[iRealIndex].bActive = pComboBox->itemData(pComboBox->currentIndex(),
                        Qt::UserRole).value<bool>();
            }
        }
    }
}
#endif


#include "cdlgexeassislistview.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
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
#include "talk/talklogic/include/modtalklogic.h"
#include "mainwnd/mainwnd.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

CDlgExeAssisListView::CDlgExeAssisListView(QWidget * parent /*= 0*/)
    : CDlgExeAssisUIBase(parent),
      m_pExeDataInfo(0),
      m_pAssDataInfo(0)
{
    m_eUIType = UI_ListView;

    InitData();
}


CDlgExeAssisListView::~CDlgExeAssisListView()
{
    DELETE_OBJ(m_pExeDataInfo);
    DELETE_OBJ(m_pAssDataInfo);
}

ExeAssis_Action CDlgExeAssisListView::GetAction()
{
    if (IsExecutive())
    {
        return EAA_ExeCommon;
    }
    else if (IsAssistant())
    {
        return EAA_AssisCommon;
    }

    return EAA_None;
}

void CDlgExeAssisListView::InitData()
{
    this->installEventFilter(this);
}

bool CDlgExeAssisListView::RefreshData()
{
    if (IsExecutive() && m_pExeDataInfo == NULL)
    {
        m_pExeDataInfo = new CExeCommon;
        m_strTitle = LANG_TRANSLATE(TRID_EXECUTIVE_VIEW_LIST);
    }
    else if (IsAssistant() && m_pAssDataInfo == NULL)
    {
        m_pAssDataInfo = new CAssCommon;
        m_strTitle = LANG_TRANSLATE(TRID_ASSISTANT_VIEW_LIST);
    }

    _MainWND.SetTitle(m_strTitle);

    ClearAllContent();

    if (IsExecutive() && NOT_EMPTY(m_pExeDataInfo))
    {
        if (g_refExeAssisUILogic.GetLogicData(m_pExeDataInfo))
        {
            ExeAssisList & listData = m_pExeDataInfo->listData;
            for (ExeAssisListIt it = listData.begin(); it != listData.end(); ++it)
            {
                QLabel * pLab = new QLabel(this);
                SetWgtFontColor(pLab, FONT_TYPE_TEXT);

                pLab->setText(it->bOptIn ? LANG_TRANSLATE(TRID_OPT_IN) : LANG_TRANSLATE(TRID_OPT_OUT));

                QWidgetPair qPair;
                qPair.first = toQString(it->ItemName.strUserId);
                qPair.second = pLab;
                qPair.third = ACTION_SEND;
                qPair.eType = SETTING_ITEM_MENU;
                m_vecWidgetPair.push_back(qPair);
            }
        }
    }
    else if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        if (g_refExeAssisUILogic.GetLogicData(m_pAssDataInfo))
        {
            ExeAssisList & listData = m_pAssDataInfo->listData;
            for (ExeAssisListIt it = listData.begin(); it != listData.end(); ++it)
            {
                QComboBox * pComboBox = new QComboBox(this);
                qComboBoxUtility::SetComboBoxStyle(pComboBox, Qt::black);

                pComboBox->addItem(LANG_TRANSLATE(TRID_OPT_OUT), false);
                pComboBox->addItem(LANG_TRANSLATE(TRID_OPT_IN), true);

                pComboBox->setCurrentIndex(pComboBox->findData(it->bOptIn, Qt::UserRole));
                pComboBox->installEventFilter(this);

                QWidgetPair qPair;
                qPair.first = toQString(it->ItemName.strUserId);
                qPair.second = pComboBox;
                m_vecWidgetPair.push_back(qPair);
            }
        }
    }

    AddWidget(m_vecWidgetPair);

    return true;
}

CExeAssisInspector * CDlgExeAssisListView::GetPageData()
{
    ReturnAllEdit();

    if (IsExecutive() && NOT_EMPTY(m_pExeDataInfo))
    {
        return m_pExeDataInfo;
    }
    else if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        int iIndex = 0;

        for (QWPVector::iterator it = m_vecWidgetPair.begin(); it != m_vecWidgetPair.end(); ++it, ++iIndex)
        {
            QComboBox * pComboBox = qobject_cast<QComboBox *>(it->second);
            if (pComboBox != NULL && iIndex < m_pAssDataInfo->listData.size())
            {
                m_pAssDataInfo->listData[iIndex].bOptIn =
                    pComboBox->itemData(pComboBox->currentIndex(), Qt::UserRole).value<bool>();
            }
        }

        return m_pAssDataInfo;
    }

    return 0;
}

bool CDlgExeAssisListView::IsStatusChanged()
{
    return IsAssistant();
}

bool CDlgExeAssisListView::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("CDlgExeAssisListView::OnAction [%s]", strAction.toLatin1().data());
    if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        if (strAction == ACTION_INITIATION)
        {
            yl::string strNumber = GetFocusItemName().toUtf8().data();
            if (!strNumber.empty())
            {
                if (BWExeAssis_SetCallInitiationNumber(strNumber, GetLineId()))
                {
                    return talklogic_EnterDialUI("", GetLineId(), true, TCT_CALL_INITIATION);
                }
            }
        }
    }

    if (strAction == ACTION_SEND)
    {
        return CallOut();
    }

    return CDlgExeAssisUIBase::OnAction(strAction);
}

QString CDlgExeAssisListView::GetFocusItemName()
{
    int iIndex = GetCurrentIndex();

    if (iIndex >= 0 && iIndex < m_vecWidgetPair.size())
    {
        return m_vecWidgetPair[iIndex].first;
    }

    return QString();
}

bool CDlgExeAssisListView::OnKeyPress(int nKeyCode, bool bPress)
{
    if (bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_HANDFREE:
        case PHONE_KEY_HANDSET:
        case PHONE_KEY_HANDSET_OFF_HOOK:
            {
                return CallOut();
            }
            break;
        default:
            break;
        }
    }

    return false;
}

bool CDlgExeAssisListView::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_vecWidgetPair.empty() || IsExecutive())
    {
        objWndData[3].m_strSoftkeyAction.clear();
        objWndData[3].m_strSoftkeyTitle.clear();
    }
    else if (IsAssistant())
    {
        objWndData[1].m_strSoftkeyAction = ACTION_INITIATION;
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DSSKEY_CALL_INITIATION);
    }

    return true;
}

bool CDlgExeAssisListView::CallOut()
{
    yl::string strNumber = GetFocusItemName().toUtf8().data();
    if (!strNumber.empty())
    {
        return talklogic_CallOut(strNumber, "", GetLineId());
    }

    return false;
}

#endif

#include "cdlgexeassisfilter.h"
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

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

CDlgExeAssisFilter::CDlgExeAssisFilter(QWidget * parent /*= 0*/)
    : CDlgExeAssisUIBase(parent)
{
    m_eUIType = UI_Filter;

    InitData();
}

ExeAssis_Action CDlgExeAssisFilter::GetAction()
{
    return EAA_Filter;
}

void CDlgExeAssisFilter::InitData()
{
    m_strTitle = LANG_TRANSLATE(TRID_EXECUTIVE_CALL_FILTER);

    m_pCobFilterSwitch = new QComboBox(this);
    m_pCobFilterMode = new QComboBox(this);
    m_pCobFilterType = new QComboBox(this);

    qComboBoxUtility::SetComboBoxStyle(m_pCobFilterSwitch, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobFilterMode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobFilterType, Qt::black);

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_FILTER_SWITCH);
    qPair.second = m_pCobFilterSwitch;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_FILTER_MODE);
    qPair.second = m_pCobFilterMode;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_FILTER_TYPE);
    qPair.second = m_pCobFilterType;
    m_vecWidgetPair.push_back(qPair);

    m_pCobFilterSwitch->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCobFilterSwitch->addItem(LANG_TRANSLATE(TRID_ON), true);

    m_pCobFilterMode->addItem(LANG_TRANSLATE(TRID_FILTERMODE_SIMPLE), EFM_Simple);
    m_pCobFilterMode->addItem(LANG_TRANSLATE(TRID_FILTERMODE_ADVANCE), EFM_Advance);
    m_pCobFilterMode->setCurrentIndex(m_pCobFilterMode->findData(EFM_Simple, Qt::UserRole));

    m_pCobFilterType->addItem(LANG_TRANSLATE(TRID_FILTERTYPE_ALLCALLS), EFT_All_Calls);
    m_pCobFilterType->addItem(LANG_TRANSLATE(TRID_FILTERTYPE_ALLINTERNALCALLS), EFT_All_Internal_Calls);
    m_pCobFilterType->addItem(LANG_TRANSLATE(TRID_FILTERTYPE_ALLEXTERNALCALLS), EFT_All_External_Calls);

    this->installEventFilter(this);
    m_pCobFilterSwitch->installEventFilter(this);
    m_pCobFilterMode->installEventFilter(this);
    m_pCobFilterType->installEventFilter(this);

    connect(m_pCobFilterMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboxValueChanged(int)));

    AddWidget(m_vecWidgetPair);
}

bool CDlgExeAssisFilter::IsStatusChanged()
{
    CExeFilterInfo dataInfo;
    if (g_refExeAssisUILogic.GetLogicData(&dataInfo))
    {
        GetPageData();

        return dataInfo != m_dataInfo;
    }

    return false;
}

bool CDlgExeAssisFilter::RefreshData()
{
    if (g_refExeAssisUILogic.GetLogicData(&m_dataInfo))
    {
        m_pCobFilterSwitch->setCurrentIndex(m_pCobFilterSwitch->findData(m_dataInfo.bSwitchOn,
                                            Qt::UserRole));
        m_pCobFilterMode->setCurrentIndex(m_pCobFilterMode->findData(m_dataInfo.eFilterMode, Qt::UserRole));
        m_pCobFilterType->setCurrentIndex(m_pCobFilterType->findData(m_dataInfo.eFilterType, Qt::UserRole));
    }

    return true;
}

CExeAssisInspector * CDlgExeAssisFilter::GetPageData()
{
    ReturnAllEdit();

    m_dataInfo.bSwitchOn = m_pCobFilterSwitch->itemData(m_pCobFilterSwitch->currentIndex(),
                           Qt::UserRole).value<bool>();
    m_dataInfo.eFilterMode = (EXE_Filter_Mode)m_pCobFilterMode->itemData(
                                 m_pCobFilterMode->currentIndex(), Qt::UserRole).value<int>();
    m_dataInfo.eFilterType = (EXE_Filter_Type)m_pCobFilterType->itemData(
                                 m_pCobFilterType->currentIndex(), Qt::UserRole).value<int>();

    return &m_dataInfo;
}

void CDlgExeAssisFilter::OnComboxValueChanged(int iIndex)
{
    m_pCobFilterType->setEnabled(iIndex == EFM_Simple);
}
#endif

#include "cdlgexeassisalter.h"
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

#define CHECK_RANG(inum, imin, imax) (inum >= imin && inum <= imax)
#define ROLLOVER_ACTION_INDEX           8

static yl::string ToString(int i)
{
    return fmtString("%d", i);
}

static int ToInt(const yl::string & strNum)
{
    return atoi(strNum.c_str());
}

CDlgExeAssisAlter::CDlgExeAssisAlter(QWidget * parent /*= 0*/)
    : CDlgExeAssisUIBase(parent)
    , m_bFirstEnter(true)
{
    m_eUIType = UI_Alter;

    InitData();
}

ExeAssis_Action CDlgExeAssisAlter::GetAction()
{
    return EAA_Alter;
}

void CDlgExeAssisAlter::InitData()
{
    m_strTitle = LANG_TRANSLATE(TRID_EXECUTIVE_CALL_ALTER);

    m_pCobAlterMode = new QComboBox(this);
    m_pCobAdvToNextTimer = new QComboBox(this);
    m_pCobCallIdName = new QComboBox(this);
    m_pEditCallIdName = new QLineEdit(this);
    m_pCobCallIdNumber = new QComboBox(this);
    m_pEditCallIdNumber = new QLineEdit(this);
    m_pCobRecallTimer = new QComboBox(this);
    m_pEditRolloverTimer = new QLineEdit(this);
    m_pCobRolloverAction = new QComboBox(this);
    m_pEditRolloverFwd = new QLineEdit(this);

    qComboBoxUtility::SetComboBoxStyle(m_pCobAlterMode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobCallIdName, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobCallIdNumber, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobRolloverAction, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobAdvToNextTimer, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobRecallTimer, Qt::black);
    SetLineEditDefaultStyle(m_pEditCallIdName);
    SetLineEditDefaultStyle(m_pEditCallIdNumber);
    SetLineEditDefaultStyle(m_pEditRolloverFwd);
    SetLineEditDefaultStyle(m_pEditRolloverTimer);

    qInstallIME(m_pEditCallIdName, IME_Name);
    qInstallIME(m_pEditCallIdNumber, IME_Name);
    qInstallIME(m_pEditRolloverFwd, IME_Name);
    qInstallIME(m_pEditRolloverTimer, IME_DIGIT);

    m_pCobAlterMode->addItem(LANG_TRANSLATE(TRID_ALTER_MODE_SIMULATANEOUS), EAM_Simulataneous);
    m_pCobAlterMode->addItem(LANG_TRANSLATE(TRID_ALTER_MODE_SEQUENTIAL), EAM_Sequential);
    m_pCobAlterMode->setCurrentIndex(m_pCobAlterMode->findData(EAM_Sequential, Qt::UserRole));

    m_pCobCallIdName->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_EXECUTIVE), ECIS_Executive);
    m_pCobCallIdName->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_ORIGINATOR), ECIS_Originator);
    m_pCobCallIdName->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_CUSTOM), ECIS_Custom);
    m_pCobCallIdName->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_EXECUTIVE_ORIGINATOR),
                              ECIS_Executive_Originator);
    m_pCobCallIdName->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_ORIGINATOR_EXECUTIVE),
                              ECIS_Originator_Executive);
    m_pCobCallIdName->setCurrentIndex(m_pCobCallIdName->findData(ECIS_Custom, Qt::UserRole));

    m_pCobCallIdNumber->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_EXECUTIVE), ECIS_Executive);
    m_pCobCallIdNumber->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_ORIGINATOR), ECIS_Originator);
    m_pCobCallIdNumber->addItem(LANG_TRANSLATE(TRID_ALTER_CALLINFO_CUSTOM), ECIS_Custom);
    m_pCobCallIdNumber->setCurrentIndex(m_pCobCallIdNumber->findData(ECIS_Custom, Qt::UserRole));

    m_pCobRolloverAction->addItem(LANG_TRANSLATE(TRID_ALTER_ROLLOVERACTION_VOICEMAIL),
                                  ERA_Voice_Messaging);
    m_pCobRolloverAction->addItem(LANG_TRANSLATE(TRID_ALTER_ROLLOVERACTION_FORWARD), ERA_FWD_SIPURI);
    m_pCobRolloverAction->addItem(LANG_TRANSLATE(TRID_ALTER_ROLLOVERACTION_NONE),
                                  ERA_NoAnswer_Processing);
    m_pCobRolloverAction->setCurrentIndex(m_pCobRolloverAction->findData(ERA_FWD_SIPURI, Qt::UserRole));

    for (int i = 2; i <= 20; ++i)
    {
        QString strValue = LANG_TRANSLATE(toQString(fmtString("%d %s", i, TRID_RINGS)));
        m_pCobAdvToNextTimer->addItem(strValue, i);
        m_pCobRecallTimer->addItem(strValue, i);
    }

    this->installEventFilter(this);
    m_pCobAlterMode->installEventFilter(this);
    m_pCobAdvToNextTimer->installEventFilter(this);
    m_pCobCallIdName->installEventFilter(this);
    m_pEditCallIdName->installEventFilter(this);
    m_pCobCallIdNumber->installEventFilter(this);
    m_pEditCallIdNumber->installEventFilter(this);
    m_pCobRecallTimer->installEventFilter(this);
    m_pEditRolloverTimer->installEventFilter(this);
    m_pCobRolloverAction->installEventFilter(this);
    m_pEditRolloverFwd->installEventFilter(this);

    connect(m_pCobCallIdName, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnComboxCallIdNameValueChanged(int)));
    connect(m_pCobCallIdNumber, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnComboxCallIdNumberValueChanged(int)));
    connect(m_pCobRolloverAction, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnComboxRolloverTypeValueChanged(int)));
    connect(m_pCobAlterMode, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnComboxAlterModeValueChanged(int)));

    if (g_refExeAssisUILogic.GetLogicData(&m_dataInfo))
    {
        AddOptions(ERA_FWD_SIPURI == m_dataInfo.eRolloverAction);
    }
    else
    {
        AddOptions(false);
    }
}

bool CDlgExeAssisAlter::RefreshData()
{
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditRolloverTimer, EDIT_TYPE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditCallIdName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditCallIdNumber, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditRolloverFwd, EDIT_TYPE_NAME);

    if (g_refExeAssisUILogic.GetLogicData(&m_dataInfo))
    {
        m_pCobAlterMode->setCurrentIndex(m_pCobAlterMode->findData(m_dataInfo.eAlterMode, Qt::UserRole));
        m_pCobCallIdName->setCurrentIndex(m_pCobCallIdName->findData(m_dataInfo.eCallInfoNameSource,
                                          Qt::UserRole));
        m_pCobCallIdNumber->setCurrentIndex(m_pCobCallIdNumber->findData(m_dataInfo.eCallInfoNumberSource,
                                            Qt::UserRole));
        m_pCobRolloverAction->setCurrentIndex(m_pCobRolloverAction->findData(m_dataInfo.eRolloverAction,
                                              Qt::UserRole));

        if (m_dataInfo.iAdvanceToNextAssistantAfter < 2 || m_dataInfo.iAdvanceToNextAssistantAfter > 20)
        {
            m_dataInfo.iAdvanceToNextAssistantAfter = 2;
        }
        if (m_dataInfo.iCallPushRecallAfter < 2 || m_dataInfo.iCallPushRecallAfter > 20)
        {
            m_dataInfo.iCallPushRecallAfter = 2;
        }

        m_pCobAdvToNextTimer->setCurrentIndex(m_pCobAdvToNextTimer->findData(
                m_dataInfo.iAdvanceToNextAssistantAfter, Qt::UserRole));
        m_pCobRecallTimer->setCurrentIndex(m_pCobRecallTimer->findData(m_dataInfo.iCallPushRecallAfter,
                                           Qt::UserRole));

        m_pEditCallIdName->setText(toQString(m_dataInfo.strCustomCallName));
        m_pEditCallIdNumber->setText(toQString(m_dataInfo.strCustomCallNumber));
        m_pEditRolloverTimer->setText(toQString(ToString(m_dataInfo.iRolloverAfter)));
        m_pEditRolloverFwd->setText(toQString(m_dataInfo.strRolloverTo));
    }

    return true;
}

bool CDlgExeAssisAlter::IsStatusChanged()
{
    CExeAlterInfo dataInfo;
    if (g_refExeAssisUILogic.GetLogicData(&dataInfo))
    {
        GetPageData();

        return dataInfo != m_dataInfo;
    }

    return false;
}

CExeAssisInspector * CDlgExeAssisAlter::GetPageData()
{
    ReturnAllEdit();

    m_dataInfo.eAlterMode = (EXE_Alter_Mode)m_pCobAlterMode->itemData(m_pCobAlterMode->currentIndex(),
                            Qt::UserRole).value<int>();
    m_dataInfo.eRolloverAction = (EXE_Rollover_Action)m_pCobRolloverAction->itemData(
                                     m_pCobRolloverAction->currentIndex(), Qt::UserRole).value<int>();
    m_dataInfo.eCallInfoNameSource = (EXE_CallInfo_Source)m_pCobCallIdName->itemData(
                                         m_pCobCallIdName->currentIndex(), Qt::UserRole).value<int>();
    m_dataInfo.eCallInfoNumberSource = (EXE_CallInfo_Source)m_pCobCallIdNumber->itemData(
                                           m_pCobCallIdNumber->currentIndex(), Qt::UserRole).value<int>();

    m_dataInfo.iAdvanceToNextAssistantAfter = m_pCobAdvToNextTimer->itemData(
                m_pCobAdvToNextTimer->currentIndex(), Qt::UserRole).value<int>();
    m_dataInfo.iCallPushRecallAfter = m_pCobRecallTimer->itemData(m_pCobRecallTimer->currentIndex(),
                                      Qt::UserRole).value<int>();

    m_dataInfo.iRolloverAfter = ToInt(m_pEditRolloverTimer->text().toUtf8().data());

    m_dataInfo.strCustomCallName = m_pEditCallIdName->text().toUtf8().data();
    m_dataInfo.strCustomCallNumber = m_pEditCallIdNumber->text().toUtf8().data();
    m_dataInfo.strRolloverTo = m_pEditRolloverFwd->text().toUtf8().data();

    return &m_dataInfo;
}

void CDlgExeAssisAlter::OnComboxCallIdNameValueChanged(int iIndex)
{
    m_pEditCallIdName->setEnabled(iIndex == ECIS_Custom);
}

void CDlgExeAssisAlter::OnComboxCallIdNumberValueChanged(int iIndex)
{
    m_pEditCallIdNumber->setEnabled(iIndex == ECIS_Custom);
}

void CDlgExeAssisAlter::OnComboxRolloverTypeValueChanged(int iIndex)
{
    EXE_Rollover_Action eRolloverAction = (EXE_Rollover_Action)m_pCobRolloverAction->itemData(
            m_pCobRolloverAction->currentIndex(), Qt::UserRole).value<int>();
    AddOptions(ERA_FWD_SIPURI == eRolloverAction);

    if (NULL != m_pListDelegate)
    {
        if (!m_bFirstEnter)
        {
            m_pListDelegate->GoNextPage();
            m_pListDelegate->SelectIndex(ROLLOVER_ACTION_INDEX);
        }
        else
        {
            m_bFirstEnter = false;
        }
    }
}

void CDlgExeAssisAlter::OnComboxAlterModeValueChanged(int iIndex)
{
    m_pCobAdvToNextTimer->setEnabled(iIndex == EAM_Sequential);
}

bool CDlgExeAssisAlter::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    GetPageData();

    if (m_dataInfo.eCallInfoNameSource == ECIS_Custom)
    {
        if (m_dataInfo.strCustomCallName.empty())
        {
            objErrorContent.strErrorContent = TRID_TARGET_CANNOT_BE_EMPTY;
            objErrorContent.pLineEdit = m_pEditCallIdName;
            return false;
        }
    }
    if (m_dataInfo.eCallInfoNumberSource == ECIS_Custom)
    {
        if (m_dataInfo.strCustomCallNumber.empty())
        {
            objErrorContent.strErrorContent = TRID_TARGET_CANNOT_BE_EMPTY;
            objErrorContent.pLineEdit = m_pEditCallIdNumber;
            return false;
        }
    }
    if (!CHECK_RANG(m_dataInfo.iRolloverAfter, 0, 9999))
    {
        objErrorContent.strErrorContent = TRID_INVALID_RANGE_VALUE;
        objErrorContent.pLineEdit = m_pEditRolloverTimer;
        return false;
    }
    if (m_dataInfo.eRolloverAction == ERA_FWD_SIPURI)
    {
        if (m_dataInfo.strRolloverTo.empty())
        {
            objErrorContent.strErrorContent = TRID_TARGET_CANNOT_BE_EMPTY;
            objErrorContent.pLineEdit = m_pEditRolloverFwd;
            return false;
        }
    }

    return true;
}

void CDlgExeAssisAlter::AddOptions(bool bAddRolloverFwd)
{
    m_pEditRolloverFwd->hide();
    MakeReadyForRelayout();

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_MODE);
    qPair.second = m_pCobAlterMode;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_TIMER_TO_NEXT);
    qPair.second = m_pCobAdvToNextTimer;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_CALLID_NAME);
    qPair.second = m_pCobCallIdName;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_CALLID_CUSTOM_NAME);
    qPair.second = m_pEditCallIdName;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_CALLID_NUMBER);
    qPair.second = m_pCobCallIdNumber;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER);
    qPair.second = m_pEditCallIdNumber;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_TIMER_RECALL_AFTER);
    qPair.second = m_pCobRecallTimer;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_ROLLOVER_TIMER);
    qPair.second = m_pEditRolloverTimer;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_ROLLOVER_ACTION);
    qPair.second = m_pCobRolloverAction;
    m_vecWidgetPair.push_back(qPair);

    if (bAddRolloverFwd)
    {
        qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALTER_ROLLOVER_FWD_TO);
        qPair.second = m_pEditRolloverFwd;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}

#endif

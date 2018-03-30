#include "dlgsettransparency.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcomboboxutility.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "baseui/transwidgetbgcontroler.h"
#include "service_config.h"
#include "dsklog/log.h"

CDlgSetTransparency::CDlgSetTransparency(QWidget * pParent /* = NULL */)
    : CDlgBaseSubPage(pParent)
{
    InitData();

    InitListItemData();
}

CDlgSetTransparency::~CDlgSetTransparency()
{
}

bool CDlgSetTransparency::LoadPageData()
{
    if (NULL == m_pTransparencyType)
    {
        return false;
    }

    m_pTransparencyType->setCurrentIndex(m_pTransparencyType->findData(m_nTransType));

    return true;
}

bool CDlgSetTransparency::IsStatusChanged()
{
    if (NULL == m_pTransparencyType)
    {
        return false;
    }

    if (m_nTransType != m_pTransparencyType->itemData(m_pTransparencyType->currentIndex()).value<int>())
    {
        return true;
    }

    return false;
}

bool CDlgSetTransparency::SavePageData()
{
    if (IsStatusChanged())
    {
        int nTransType = m_pTransparencyType->itemData(m_pTransparencyType->currentIndex()).value<int>();
        yl::string strValue = g_TransWidgetBgControler.GetTransValueByType(nTransType);

        configParser_SetConfigString(kszTransparencyType, strValue.c_str(), CONFIG_LEVEL_PHONE);
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_TRANSPARENTCY, 0);
    }

    return true;
}

void CDlgSetTransparency::InitData()
{
    m_nTransType = g_TransWidgetBgControler.GetTransparencyType();

    // Init Widget
    m_pTransparencyType = new QComboBox(this);

    if (NULL == m_pTransparencyType)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pTransparencyType, Qt::black);

    FillTransSelectType();

    m_pTransparencyType->installEventFilter(this);
}

void CDlgSetTransparency::InitListItemData()
{
    if (NULL == m_pTransparencyType)
    {
        return;
    }

    MakeReadyForRelayout();

    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(TRID_TRANSPARENCY);
    qPair.second = m_pTransparencyType;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

void CDlgSetTransparency::FillTransSelectType()
{
    if (NULL == m_pTransparencyType)
    {
        return;
    }

    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_0_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_LOW_TRANS)));
    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_20_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_TWENTY_PERCENT)));
    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_40_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_FORTY_PERCENT)));
    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_60_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_SIXTY_PERCENT)));
    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_80_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_EIGHTY_PERCENT)));
    m_pTransparencyType->addItem(LANG_TRANSLATE(TRID_100_PERCEND),
                                 QVariant::fromValue(int(TRANSPARENT_TYPE_HEIGHT_TRANS)));
}

#include "model.h"
#include "cdlgdhcptime.h"

#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "setting/src/dateandtimecontroller.h"
#include "baseui/fontcolor.h"
#include "uikernel/qcheckbox.h"
#include "service_config.h"

CDlgDHCPTime::CDlgDHCPTime(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxDHCPTime(NULL)
{
    InitData();
}

void CDlgDHCPTime::InitData()
{
    //设定滑块大小位置
    m_qCheckBoxDHCPTime = new qCheckBox(this);
    if (m_qCheckBoxDHCPTime == NULL)
    {
        //内存已满立即返回
        return;
    }

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_DHCP_TIME);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxDHCPTime;

    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    m_qCheckBoxDHCPTime->installEventFilter(this);
    this->installEventFilter(this);
}

bool CDlgDHCPTime::GetPageData(DateTimeDHCPData * pData)
{
    if (NULL == pData)
    {
        return false;
    }
    if (m_qCheckBoxDHCPTime != NULL)
    {
        pData->m_isDHCPTimeOn = m_qCheckBoxDHCPTime->GetData();
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgDHCPTime::IsStatusChanged()
{
    DateTimeDHCPData objPageData;
    DateTimeDHCPData objConfigData;

    //获取当前界面已有的配置数据，存入objConfigData
    bool bResult = g_pDateAndTimeController->GetDHCPData(&objConfigData);

    //利用GetPageData获取界面显示的数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);
    if (bResult)
    {
        if (objPageData.m_isDHCPTimeOn != objConfigData.m_isDHCPTimeOn)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CDlgDHCPTime::LoadPageData()
{
    this->setFocus();
    DateTimeDHCPData objData;

    //获取界面已有的配置值，存入objData
    bool bResult = g_pDateAndTimeController->GetDHCPData(&objData);
    if (bResult)
    {
        if (NULL == m_qCheckBoxDHCPTime)
        {
            return false;
        }
        m_qCheckBoxDHCPTime->SetData(objData.m_isDHCPTimeOn);
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgDHCPTime::SavePageData()
{
    if (NULL != m_qCheckBoxDHCPTime)
    {
        DateTimeDHCPData pData;
        GetPageData(&pData);

        /*
        将界面显示值存入pData
        利用Controller将pData值存入当前界面的配置中
        */
        g_pDateAndTimeController->SetDHCPData(pData);

        // 向所有线程广播配置文件更改的消息
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);

        return true;
    }
    else
    {
        return false;
    }
}

CDlgDHCPTime::~CDlgDHCPTime()
{
}

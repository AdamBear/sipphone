#include "dlgstatusrtp.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "ETLLib.hpp"
#include "feature/include/modfeature.h"
#include "talk/vqrtcp/include/modvqrtcp.h"

CDlgStatusRTP::CDlgStatusRTP(QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
{
    //
}

CDlgStatusRTP::~CDlgStatusRTP()
{
    //
}

//加载数据
bool CDlgStatusRTP::LoadPageData()
{
    ClearAllContent();

    //status界面没有行选中效果
    //m_pGeneralFrame->SetItemClickBG("");

    this->setFocus();

    QWidgetPair qPair;

    SVQReportData objData;
    vqrtcp_RTPStatusGetSessionReport(objData);

    const RTPConfigureList * pList = feature_RTPConfigGetEnableList();

    if (NULL != pList)
    {
        RTPConfigureList pRTPlist = *pList;
        SETTINGUI_INFO("pRTPlist.size = %d", pRTPlist.size());
        RTPConfigureList::const_iterator iter = pRTPlist.begin();
        for (; iter != pRTPlist.end(); ++iter)
        {
            RTPConfigureItem item = *iter;
            if (!item.m_isEnable)
            {
                continue;
            }
            yl::string strText = objData.GetAttributeValueById(item.m_strIdName);

            QString strTag;
            if ("CurrentTime" == item.m_strIdName)
            {
                strTag = LANG_TRANSLATE(TRID_STOP_TIME);
            }
            else
            {
                strTag = LANG_TRANSLATE(item.m_strDisplayName.c_str());
            }

            qPair.first = strTag;
            QLabel * pLab = new QLabel(this);
            if (pLab != NULL)
            {
                pLab->setText(toQString(strText.c_str()));
                qPair.second = pLab;
            }

            m_vecWidgetPair.push_back(qPair);
        }

        AddWidget(m_vecWidgetPair);
    }

    return true;
}

//状态显示softkey没有保存项
bool CDlgStatusRTP::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";
    return true;
}


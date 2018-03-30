#include "model.h"
#include "cdlgintercom.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/intercomcontroller.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qcheckbox.h"

CDlgIntercom::CDlgIntercom(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxIntercomAllow(NULL)
    , m_qCheckBoxIntercomBarge(NULL)
    , m_qCheckBoxIntercomMute(NULL)
    , m_qCheckBoxIntercomTone(NULL)
{
    InitData();
}

void CDlgIntercom::InitData()
{
    m_qCheckBoxIntercomTone = new qCheckBox(this);
    m_qCheckBoxIntercomMute = new qCheckBox(this);
    m_qCheckBoxIntercomBarge = new qCheckBox(this);
    m_qCheckBoxIntercomAllow = new qCheckBox(this);

    //指针判空
    if (m_qCheckBoxIntercomAllow == NULL || m_qCheckBoxIntercomBarge == NULL
            || m_qCheckBoxIntercomMute == NULL || m_qCheckBoxIntercomTone == NULL)
    {
        return;
    }

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_INTERCOM_ALLOW);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxIntercomAllow;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_INTERCOM_MUTE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxIntercomMute;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_INTERCOM_TONE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxIntercomTone;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_INTERCOM_BARGE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxIntercomBarge;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //为界面上各输入控件安装事件过滤器
    m_qCheckBoxIntercomTone->installEventFilter(this);
    m_qCheckBoxIntercomMute->installEventFilter(this);
    m_qCheckBoxIntercomBarge->installEventFilter(this);
    m_qCheckBoxIntercomAllow->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

bool CDlgIntercom::GetPageData(IntercomData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    //获取界面滑块对应值，存入pData
    if (m_qCheckBoxIntercomAllow == NULL || m_qCheckBoxIntercomBarge == NULL
            || m_qCheckBoxIntercomMute == NULL || m_qCheckBoxIntercomTone == NULL)
    {
        return false;
    }
    else
    {
        pData->m_isAllowed = m_qCheckBoxIntercomAllow->GetData();
        pData->m_isMuteOn = m_qCheckBoxIntercomMute->GetData();
        pData->m_isToneOn = m_qCheckBoxIntercomTone->GetData();
        pData->m_isBargeOn = m_qCheckBoxIntercomBarge->GetData();

        return true;
    }
}

bool CDlgIntercom::IsStatusChanged()
{
    IntercomData objConfigData;
    IntercomData objPageData;

    //获取Intercom界面已有的配置值，存入objConfigData
    bool bResult = g_pIntercomController->GetIntercomData(&objConfigData);

    //获取Intercom界面当前的显示数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);

    if (bResult)
    {
        //判断配置与界面上的对应项数据是否存在改变，只要有一处改变，就返回true
        if (objConfigData.m_isAllowed != objPageData.m_isAllowed
                || objConfigData.m_isMuteOn != objPageData.m_isMuteOn
                || objConfigData.m_isToneOn != objPageData.m_isToneOn
                || objConfigData.m_isBargeOn != objPageData.m_isBargeOn)
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

bool CDlgIntercom::LoadPageData()
{
    this->setFocus();
    IntercomData objData;

    //获取Intercom界面已有的配置值，存入objData
    bool bResult = g_pIntercomController->GetIntercomData(&objData);
    if (bResult)
    {
        if (m_qCheckBoxIntercomAllow == NULL || m_qCheckBoxIntercomBarge == NULL
                || m_qCheckBoxIntercomMute == NULL || m_qCheckBoxIntercomTone == NULL)
        {
            return false;
        }
        else
        {
            //将objData中的数据在界面的对应区域中显示出来
            m_qCheckBoxIntercomAllow->SetData(objData.m_isAllowed);
            m_qCheckBoxIntercomBarge->SetData(objData.m_isBargeOn);
            m_qCheckBoxIntercomMute->SetData(objData.m_isMuteOn);
            m_qCheckBoxIntercomTone->SetData(objData.m_isToneOn);

            return true;
        }
    }
    else
    {
        return false;
    }
}

bool CDlgIntercom::SavePageData()
{
    //获取界面当前显示数据，存入pData
    IntercomData pageData;
    GetPageData(&pageData);

    //把pData内的显示数据设置为配置数据
    g_pIntercomController->SetIntercomData(pageData);
    return true;

}

CDlgIntercom::~CDlgIntercom()
{

}

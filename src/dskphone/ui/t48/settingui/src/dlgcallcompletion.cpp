#include "model.h"
#include "dlgcallcompletion.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qcheckbox.h"

CDlgCallCompletion::CDlgCallCompletion(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxCallCompletion(NULL)
{
    InitData();
}

CDlgCallCompletion::~CDlgCallCompletion()
{

}


//外部接口：加载数据
bool CDlgCallCompletion::LoadPageData()
{
    this->setFocus();
    AdvancedCallFeatureCallCompletionData pageData;
    bool bIsSuccess =
        g_pAdvancedCallFeatureController->GetCallCompletionData(&pageData);
    if (!bIsSuccess)
    {
        return false;
    }
    else
    {
        //根据已有的配置值，设置Call Waiting对应的滑块位置
        if (NULL == m_qCheckBoxCallCompletion)
        {
            return false;
        }

        m_qCheckBoxCallCompletion->SetData(pageData.m_isOn);
        return true;
    }
}

//获取页面显示数据，存入pData
bool CDlgCallCompletion::GetPageData(AdvancedCallFeatureCallCompletionData * pData)
{
    if (NULL == pData
            || m_qCheckBoxCallCompletion == NULL)
    {
        return false;
    }
    pData->m_isOn = m_qCheckBoxCallCompletion->GetData();
    return true;
}

//外部接口：获取数据状态是否改变
bool CDlgCallCompletion::IsStatusChanged()
{
    AdvancedCallFeatureCallCompletionData objConfigData;
    AdvancedCallFeatureCallCompletionData objPageData;

    //读取页面已有的配置数据，存入objConfigData
    bool bResult = g_pAdvancedCallFeatureController->GetCallCompletionData(&objConfigData);

    //读取界面当前显示的值，存入objPageData
    bResult = bResult && GetPageData(&objPageData);

    if (bResult)
    {
        if (objConfigData.m_isOn != objPageData.m_isOn)
        {
            return true;
        }
        return false;
    }
    return false;
}

//外部接口：保存数据
bool CDlgCallCompletion::SavePageData()
{
    AdvancedCallFeatureCallCompletionData pageData;
    GetPageData(&pageData);

    //把当前界面显示数据设置为配置数据
    g_pAdvancedCallFeatureController->SetCallCompletionData(pageData);
    return true;
}

//外部接口：页面向其数据Mode取数据再次刷新到界面上
void CDlgCallCompletion::Refresh()
{

}

//内部调用：初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgCallCompletion::InitData()
{
    m_qCheckBoxCallCompletion = new qCheckBox(this);
    if (m_qCheckBoxCallCompletion == NULL)
    {
        return;
    }


    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_CALL_COMPLETION);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxCallCompletion;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    m_qCheckBoxCallCompletion->installEventFilter(this);
    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}
//内部调用：用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
void CDlgCallCompletion::ReleaseData()
{

}

//内部调用：用于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
void CDlgCallCompletion::OnViewShow()
{

}

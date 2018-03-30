#include "dlgsoundgeneral.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "setting/src/soundcontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qcheckbox.h"

CDlgSoundGeneral::CDlgSoundGeneral(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxKeySoundOn(NULL)
{
    InitData();
}

CDlgSoundGeneral::~CDlgSoundGeneral()
{
    //释放数据
    ReleaseData();
}


//加载数据
bool CDlgSoundGeneral::LoadPageData()
{
    this->setFocus();

    bool bIsSuccess =
        g_pSoundController->GetGeneralData(&m_SoundGeneralData);
    if (!bIsSuccess)
    {
        return false;
    }
    else
    {
        SetCurrentPageData(m_SoundGeneralData);
        return true;
    }
}

//获取数据状态是否改变
bool CDlgSoundGeneral::IsStatusChanged()
{
    SoundGeneralData SoundGeneralData;
    //取得当前页面数据
    GetCurrentPageData(SoundGeneralData);
    if (ComparePageData(m_SoundGeneralData, SoundGeneralData))
    {
        //前后页面数据相等返回false
        //show_debug("unchanged");
        return false;
    }
    else
    {
        //前后页面数据不相等返回true
        //记录当前页面数据
        //show_debug("changed");
        CopyPageData(m_SoundGeneralData, SoundGeneralData);
        return true;
    }
}

//保存数据
bool CDlgSoundGeneral::SavePageData()
{
    GetCurrentPageData(m_SoundGeneralData);
    g_pSoundController->SetGeneralData(m_SoundGeneralData);
    return true;
}

//页面向其数据Mode取数据再次刷新到界面上
void CDlgSoundGeneral::Refresh()
{
    LoadPageData();
}

//初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgSoundGeneral::InitData()
{
    m_qCheckBoxKeySoundOn = new qCheckBox(this);
    if (m_qCheckBoxKeySoundOn == NULL)
    {
        //内存已满立即返回
        return;
    }

    m_qCheckBoxKeySoundOn->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_PLAY_KEY_TONE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxKeySoundOn;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);
}

//用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
void CDlgSoundGeneral::ReleaseData()
{

}

//于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
void CDlgSoundGeneral::OnViewShow()
{

}

//拷贝页面数据
void CDlgSoundGeneral::CopyPageData(SoundGeneralData & destPageData,
                                    const SoundGeneralData & srcPageData)
{
    //拷贝按键声音开启状态值
    destPageData.m_isKeypressSoundOn = srcPageData.m_isKeypressSoundOn;
}

//比较页面前后数据
bool CDlgSoundGeneral::ComparePageData(const SoundGeneralData & beforePageData,
                                       const SoundGeneralData & afterPageData) const
{
    return (beforePageData.m_isKeypressSoundOn == afterPageData.m_isKeypressSoundOn);
}

//取得当前页面数据
SoundGeneralData & CDlgSoundGeneral::GetCurrentPageData(SoundGeneralData & currentPageData) const
{
    if (m_qCheckBoxKeySoundOn != NULL)
    {
        currentPageData.m_isKeypressSoundOn = m_qCheckBoxKeySoundOn->GetData();
    }

    return currentPageData;
}

//设置当前页面数据
void CDlgSoundGeneral::SetCurrentPageData(SoundGeneralData & currentPageData)
{
    if (m_qCheckBoxKeySoundOn != NULL)
    {
        m_qCheckBoxKeySoundOn->SetData(currentPageData.m_isKeypressSoundOn);
    }
}

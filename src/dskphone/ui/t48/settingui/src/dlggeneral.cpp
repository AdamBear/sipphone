#include "model.h"
#include "dlggeneral.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "setting/include/authorityiddefine.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qcheckbox.h"

//构造函数
CDlgGeneral::CDlgGeneral(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxHistoryRecord(NULL)
    , m_pCmbBoxKeyAsSend(NULL)
{
    InitPageData();
}

CDlgGeneral::~CDlgGeneral()
{

}

//加载数据
bool CDlgGeneral::LoadPageData()
{
    this->setFocus();
    AdvancedCallFeatureGeneralData pageData;
    bool bIsSuccess =
        g_pAdvancedCallFeatureController->GetGeneralData(&pageData);
    if (!bIsSuccess)
    {
        return false;
    }
    else
    {
        SetCurrentPageData(pageData);
        return true;
    }
}

//获取数据状态是否改变
bool CDlgGeneral::IsStatusChanged()
{
    AdvancedCallFeatureGeneralData objPageData;
    AdvancedCallFeatureGeneralData objConfigData;

    //Controller层获取界面已有的配置值，存入objConfigData
    bool bResult = g_pAdvancedCallFeatureController->GetGeneralData(&objConfigData);

    //利用GetPageData获取界面显示的数据，存入objPageData
    GetCurrentPageData(objPageData);

    if (bResult)
    {
        //容错处理，如果下拉框中找不到key as send的字符串，设置此串为Disable
        int iTempIndex = m_pCmbBoxKeyAsSend->findData((int)objConfigData.m_eType);
        if (iTempIndex < 0)
        {
            objConfigData.m_eType = KEY_AS_SEND_DISABLED_SYMBOL;
        }
        //判断对应项数据是否存在改变，只要有一处改变，就返回true
        if (objPageData.m_eType != objConfigData.m_eType
                || objPageData.m_isHistorySaved != objConfigData.m_isHistorySaved)
        {
            return true;
        }
    }

    return false;
}

//保存数据
bool CDlgGeneral::SavePageData()
{
    AdvancedCallFeatureGeneralData pageData;
    GetCurrentPageData(pageData);

    g_pAdvancedCallFeatureController->SetGeneralData(pageData);
    return true;
}

//界面初始化
void CDlgGeneral::InitPageData()
{
    m_pCmbBoxKeyAsSend = new QComboBox(this);
    m_qCheckBoxHistoryRecord = new qCheckBox(this);

    //指针判空
    if (m_qCheckBoxHistoryRecord == NULL || m_pCmbBoxKeyAsSend == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxKeyAsSend, Qt::black);
    //添加disabled项，索引值为0，表示disabeld状态
    m_pCmbBoxKeyAsSend->addItem(LANG_TRANSLATE(TRID_DISABLED),
                                QVariant::fromValue((int)KEY_AS_SEND_DISABLED_SYMBOL));
    //添加key #项，索引值为1，表示“#”拨出有效
    m_pCmbBoxKeyAsSend->addItem("#",
                                QVariant::fromValue((int)KEY_AS_SEND_CROSSHATCH_SYMBOL));
    //添加key *项，索引值为2，表示“*”拨出有效
    m_pCmbBoxKeyAsSend->addItem("*",
                                QVariant::fromValue((int)KEY_AS_SEND_STAR_SYMBOL));

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_KEY_AS_SEND);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxKeyAsSend;
    qPair.strAuthorId = kszFeaturesGeneralKeyAsSend;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_HISTORY_RECORD);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxHistoryRecord;
    qPair.strAuthorId = kszFeaturesGeneralHistoryRecord;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);


    //为界面上各输入控件安装事件过滤器
    m_pCmbBoxKeyAsSend->installEventFilter(this);
    m_qCheckBoxHistoryRecord->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}


//拷贝页面数据
void CDlgGeneral::CopyPageData(AdvancedCallFeatureGeneralData & dest_GeneralPageData,
                               const AdvancedCallFeatureGeneralData & src_GeneralPageData)
{
    //拷贝key as send
    dest_GeneralPageData.m_eType = src_GeneralPageData.m_eType;
    //拷贝history record
    dest_GeneralPageData.m_isHistorySaved = src_GeneralPageData.m_isHistorySaved;
}

//比较页面前后数据
bool CDlgGeneral::ComparePageData(const AdvancedCallFeatureGeneralData & before_GeneralPageData,
                                  const AdvancedCallFeatureGeneralData & after_GeneralPageData) const
{
    if (before_GeneralPageData.m_isHistorySaved != after_GeneralPageData.m_isHistorySaved
            || before_GeneralPageData.m_eType != after_GeneralPageData.m_eType)
    {
        //不相等
        return false;
    }
    else
    {
        //相等
        return true;
    }
}

//取得当前页面数据
AdvancedCallFeatureGeneralData & CDlgGeneral::GetCurrentPageData(
    AdvancedCallFeatureGeneralData & current_GeneralPageData) const
{
    //指针判空
    if (m_qCheckBoxHistoryRecord != NULL && m_pCmbBoxKeyAsSend != NULL)
    {
        //取得key as send状态值
        current_GeneralPageData.m_eType =
            static_cast<KeyAsSendType>(m_pCmbBoxKeyAsSend
                                       ->itemData(m_pCmbBoxKeyAsSend->currentIndex(), Qt::UserRole).value<int>());

        //取得history record 状态值
        current_GeneralPageData.m_isHistorySaved = m_qCheckBoxHistoryRecord->GetData();
    }

    return current_GeneralPageData;
}

//设置当前页面数据
void CDlgGeneral::SetCurrentPageData(AdvancedCallFeatureGeneralData & current_GeneralPageData)
{
    //指针判空
    if (m_qCheckBoxHistoryRecord == NULL || m_pCmbBoxKeyAsSend == NULL)
    {
        return;
    }

    //设置key as send状态值
    int nTempIndex = m_pCmbBoxKeyAsSend->findData((int)current_GeneralPageData.m_eType);
    if (nTempIndex < 0)
    {
        nTempIndex = 0;
    }
    m_pCmbBoxKeyAsSend->setCurrentIndex(nTempIndex);

    m_qCheckBoxHistoryRecord->SetData(current_GeneralPageData.m_isHistorySaved);
}


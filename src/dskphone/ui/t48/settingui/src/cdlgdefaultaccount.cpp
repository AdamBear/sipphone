#include "cdlgdefaultaccount.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "uikernel/qcomboboxutility.h"

struct AccountData;
struct AccountItemListData;

CDlgDefaultAccount::CDlgDefaultAccount(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxAccount(NULL)
    , m_bNoAccount(false)
{
    InitData();
}

void CDlgDefaultAccount::InitData()
{
    m_pCmbBoxAccount = new QComboBox(this);
    if (m_pCmbBoxAccount == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxAccount, Qt::black);
    //利用迭代器将List存放的数据的m_strDisplayName项加入到下拉框中
    AccountItemListData ListData;
    if (! g_pAdvancedCallFeatureController->GetValidAccountList(&ListData))
    {
        m_bNoAccount = true;
        m_pCmbBoxAccount->addItem(LANG_TRANSLATE(TRID_NO_ACCOUNT), -1);
    }
    else
    {
        AccountData data;
        std::list<AccountData>::const_iterator pDataIter = ListData.m_listAccount.begin();
        for (/* */; pDataIter != ListData.m_listAccount.end(); ++pDataIter)
        {
            data = *pDataIter;
            if (!data.m_strName.empty())
            {
                m_pCmbBoxAccount->addItem(toQString(data.m_strName.c_str()), data.m_nID);
            }
        }
    }

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_DEFAULT_ACCOUNT);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxAccount;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //!!!!! T46安装了过滤器需要测试一下
    /*
        m_pCmbBoxAccount->installEventFilter(this);
        //为界面自己安装事件过滤器
        this->installEventFilter(this);
    */
}

bool CDlgDefaultAccount::IsStatusChanged()
{
    //指针判空
    if (m_bNoAccount || m_pCmbBoxAccount == NULL || m_pCmbBoxAccount->count() == 0)
    {
        return false;
    }

    int nAccountId = m_pCmbBoxAccount->
                     itemData(m_pCmbBoxAccount->currentIndex(), Qt::UserRole).value<int>();

    if (nAccountId == -1)
    {
        return false;
    }

    int iDefaultAccountId = g_pAdvancedCallFeatureController->GetDefaultAccountID();
    //
    if (nAccountId != iDefaultAccountId)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgDefaultAccount::LoadPageData()
{
    //指针判空
    if (m_pCmbBoxAccount == NULL
            || m_bNoAccount)
    {
        return false;
    }

    this->setFocus();
    int iDefaultAccountId = g_pAdvancedCallFeatureController->GetDefaultAccountID();

    //当前下拉框选定的就是当前配置中的语言
    m_pCmbBoxAccount->setCurrentIndex(m_pCmbBoxAccount
                                      ->findData(iDefaultAccountId, Qt::UserRole));

    return true;
}

bool CDlgDefaultAccount::SavePageData()
{
    //指针判空
    if (m_pCmbBoxAccount == NULL)
    {
        return false;
    }

    int nDefaultAccountID = m_pCmbBoxAccount->
                            itemData(m_pCmbBoxAccount->currentIndex(), Qt::UserRole).value<int>();

    g_pAdvancedCallFeatureController->SetDefaultAccount(nDefaultAccountID);

    return true;
}

CDlgDefaultAccount::~CDlgDefaultAccount()
{

}

#include "cdlgsecurityclassification.h"
#include "account/include/modaccount.h"
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uicommon/uikernel/languagehelper.h"


CDlgSecurityClassification::CDlgSecurityClassification(QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
{
    InitPageData();
}

CDlgSecurityClassification::~CDlgSecurityClassification()
{

}

//加载数据
bool CDlgSecurityClassification::LoadPageData()
{
    ClearAllContent();

    SetWindowTitle(LANG_TRANSLATE(TRID_SECURITY_CLASSIFICATION));

    int iAcountNum = acc_AccountNum();
    QString strAccountNumber;
    QWidgetPair qPair;
    qPair.bAddIndex = true;
    qPair.eType = SETTING_ITEM_NORMAL;
    qPair.third = "";

    for (int i = 0; i < iAcountNum; ++i)
    {
        if (!acc_IsAccountAvailable(i)
                || configParser_GetCfgItemIntValue(kszAccountEnableSecurityClassification, i) != 1)
        {
            continue;
        }

        QLabel * pLab = new QLabel(this);
        if (pLab == NULL)
        {
            return false;
        }

        pLab->setText("");
        SetWgtFontColor(pLab, FONT_TYPE_TEXT);

        strAccountNumber = toQString(acc_GetAccountShowText(i));
        yl::string strValue = talklogic_GetAccountSecurityLevel(i);
        strValue = strValue.empty() ? TRID_UNCLASSIFIED : strValue;
        pLab->setText(LANG_TRANSLATE(strValue.c_str()));
        qPair.first = strAccountNumber;
        qPair.second = pLab;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair, true, false);
    return true;
}

//获取数据状态是否改变
bool CDlgSecurityClassification::IsStatusChanged()
{
    return false;
}

//保存数据
bool CDlgSecurityClassification::SavePageData()
{
    return true;
}

//界面初始化
void CDlgSecurityClassification::InitPageData()
{

}

void CDlgSecurityClassification::OnReShow()
{
    LoadPageData();
    UpdateWnd();
}

//状态显示softkey没有保存项
bool CDlgSecurityClassification::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}
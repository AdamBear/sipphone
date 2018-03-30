#include "cdlgautoanswer.h"
#include "account/include/modaccount.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "keymap.h"
#include "baseui/fontcolor.h"
#include "setting/src/autoanswercontroller.h"
#include "uikernel/qcheckbox.h"

CDlgAutoAnswer::CDlgAutoAnswer(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgAutoAnswer::~CDlgAutoAnswer()
{
}

void CDlgAutoAnswer::InitData()
{
    m_vecAccountId.clear();

    QWidgetPair qPair;
    qPair.bAddIndex = true;
    QString strAccountDisplayName = "";
    //获取可用账号数据
    int nIndex = 0;
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        qCheckBox * pCheckBox = new qCheckBox(this);
        if (NULL != pCheckBox)
        {
            //V81Beta 统一显示名称，统一从acc_GetAccountShowText获取 不显示注册不成功的账号
            strAccountDisplayName = toQString(acc_GetAccountShowText(i));
            pCheckBox->SetData(false);

            //设置Activation标签
            qPair.first = strAccountDisplayName;
            qPair.second = pCheckBox;
            // 加入映射中

            //V81Beta 统一显示名称，统一从acc_GetAccountShowText获取 不显示注册不成功的账号
            if (!acc_IsAccountAvailable(i))
            {
                pCheckBox->hide();
                continue;
            }

            m_vecWidgetPair.push_back(qPair);
            m_vecAccountId.push_back(i);
        }
    }
    //设置为None的提示必须在AddWidget之后
    AddWidget(m_vecWidgetPair);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

// 对输入框的数据进行验证
bool CDlgAutoAnswer::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

//从V层读取当前界面的配置，通过C层保存
bool CDlgAutoAnswer::SavePageData()
{
    for (int iLineId = 0; iLineId  < m_vecWidgetPair.size(); iLineId ++)
    {
        if (NULL != m_vecWidgetPair[iLineId].second
                && m_vecWidgetPair[iLineId].second->inherits("qCheckBox"))
        {
            qCheckBox * pCbox = (qCheckBox *)(m_vecWidgetPair[iLineId].second);

            //获取当前界面显示数据，设置为配置数据
            g_pAutoAnswerController->SetAutoAnswerStatusByLineId(m_vecAccountId.at(iLineId), pCbox->GetData());
        }
    }

    return true;
}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgAutoAnswer::LoadPageData()
{
    if (m_vecWidgetPair.size() < 1)
    {
        return false;
    }

    for (int iLineId = 0; iLineId  < m_vecWidgetPair.size(); iLineId++)
    {
        // 从配置中读值
        bool bStatus = g_pAutoAnswerController->GetAutoAnswerStatusByLineId(m_vecAccountId.at(iLineId));

        if (NULL != m_vecWidgetPair[iLineId].second
                && m_vecWidgetPair[iLineId].second->inherits("qCheckBox"))
        {
            ((qCheckBox *)(m_vecWidgetPair[iLineId].second))->SetData(bStatus ? true : false);
        }
    }

    return true;
}

//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgAutoAnswer::IsStatusChanged()
{
    if (m_vecWidgetPair.size() < 1)
    {
        return false;
    }

    for (int iLineID = 0; iLineID  < m_vecWidgetPair.size(); iLineID ++)
    {
        // 从配置中读值
        bool bStatus = g_pAutoAnswerController->GetAutoAnswerStatusByLineId(m_vecAccountId.at(iLineID));

        if (NULL != m_vecWidgetPair[iLineID].second
                && m_vecWidgetPair[iLineID].second->inherits("qCheckBox"))
        {
            if (((qCheckBox *)(m_vecWidgetPair[iLineID].second))->GetData() != bStatus)
            {
                return true;
            }
        }
    }
    return false;
}


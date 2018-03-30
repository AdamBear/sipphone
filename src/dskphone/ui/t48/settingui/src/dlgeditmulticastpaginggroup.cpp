#include "dlgeditmulticastpaginggroup.h"

#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "setting/src/multicastpaginglistcontroller.h"
#include "uikernel/qlineeditutility.h"
#include "uicommon/qtcommon/qmisc.h"
#include "commonunits/modcommonunits.h"

CDlgMulticastPagingGroupEdit::CDlgMulticastPagingGroupEdit(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabelOrder(NULL)
    , m_pEditGroupAddres(NULL)
    , m_pEditLabel(NULL)
    , m_pEditChannel(NULL)
    , m_pGroupInfo(NULL)
{
    InitData();
}

void CDlgMulticastPagingGroupEdit::InitData()
{
    m_pLabelOrder = new QLabel(this);
    m_pEditGroupAddres = new QLineEdit(this);
    m_pEditLabel = new QLineEdit(this);
    m_pEditChannel = new QLineEdit(this);
    if (NULL == m_pLabelOrder
            || NULL == m_pEditGroupAddres
            || NULL == m_pEditLabel
            || NULL == m_pEditChannel)
    {
        return ;
    }


    // 安装输入法
    qInstallIME(m_pEditGroupAddres, IME_DIGIT);
    qInstallIME(m_pEditLabel, IME_Name);
    qInstallIME(m_pEditChannel, IME_Number);

    qLineEditUtility::SetLineEditStyle(m_pEditGroupAddres, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditLabel, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditChannel, Qt::black);

    m_pEditChannel->setMaxLength(2);

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_NUMBER);
    qPair.second = m_pLabelOrder;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_ADDRESS);
    qPair.second = m_pEditGroupAddres;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_LABEL);
    qPair.second = m_pEditLabel;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_CHANNEL);
    qPair.second = m_pEditChannel;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
    m_pEditGroupAddres->installEventFilter(this);
    m_pEditLabel->installEventFilter(this);
    m_pEditChannel->installEventFilter(this);

    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_EIDT_PAGING_GROUP).toUtf8().data();
}

bool CDlgMulticastPagingGroupEdit::LoadPageData()
{
    if (NULL == m_pGroupInfo
            || NULL == m_pLabelOrder
            || NULL == m_pEditGroupAddres
            || NULL == m_pEditLabel
            || NULL == m_pEditChannel)
    {
        return false;
    }

    char szTemp[256] = {0};
    sprintf(szTemp, "%d", m_pGroupInfo->iIndex);
    yl::string strTemp(szTemp);
    m_pLabelOrder->setText(toQString(strTemp));
    m_pEditLabel->setText(toQString(m_pGroupInfo->strLabel));

    sprintf(szTemp, "%d", m_pGroupInfo->iChannel);
    strTemp = szTemp;
    m_pEditChannel->setText(toQString(strTemp));

    m_pEditGroupAddres->setText(toQString(m_pGroupInfo->strAddress));
    SetCurrentIndex(1);//选中第二行

    return true;
}

bool CDlgMulticastPagingGroupEdit::IsStatusChanged()
{
    return m_pEditGroupAddres->text() != QString(m_pGroupInfo->strAddress.c_str())
           || m_pEditLabel->text() != QString(m_pGroupInfo->strLabel.c_str()
                   || m_pEditChannel->text() != QString(m_pGroupInfo->iChannel));
}

bool CDlgMulticastPagingGroupEdit::SavePageData()
{
    if (NULL == m_pGroupInfo)
    {
        return false;
    }

    PagingGroupInfo info;
    info.iIndex = m_pGroupInfo->iIndex;
    info.strAddress = m_pEditGroupAddres->text().toUtf8().data();
    info.strLabel = m_pEditLabel->text().toUtf8().data();
    info.iChannel = m_pEditChannel->text().toInt();
    g_pPagingListController->SaveGroupInfo(info);
    return true;
}

// 设置子页面前的操作
void CDlgMulticastPagingGroupEdit::BeforeSetSubPage(void * pData)
{
    PagingGroupInfo * pInfo = static_cast<PagingGroupInfo *>(pData);
    if (NULL != pInfo)
    {
        m_pGroupInfo = pInfo;
    }
}

bool CDlgMulticastPagingGroupEdit::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pEditGroupAddres->text().isEmpty() && m_pEditLabel->text().isEmpty()
            && m_pEditChannel->text().isEmpty())
    {
        return true;
    }

    if (m_pEditGroupAddres->text().isEmpty())
    {
        objErrorContent.pLineEdit = m_pEditGroupAddres;
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INFORMATION_NOT_COMPLETE);
        return false;
    }

    if (m_pEditLabel->text().isEmpty())
    {
        objErrorContent.pLineEdit = m_pEditLabel;
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INFORMATION_NOT_COMPLETE);
        return false;
    }

    yl::string strChannel = m_pEditChannel->text().toUtf8().data();
    if (!commonUnits_IsValidPagingChannel(strChannel))
    {
        objErrorContent.pLineEdit = m_pEditChannel;
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_SAVE_CHANNEL_ERROR);
        return false;
    }

    return true;
}

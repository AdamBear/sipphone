#include "dlgviewmessage.h"
#include "xwindow/xWindow.h"
#include "settinguiwizardtitle.h"
#include "pageiddefine.h"
#include "baseui/include/baseuicommon.h"
#include "basesettinguidelegate.h"
#include "settinguiwizardmanager.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "textmessageviewdelegate.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"

namespace
{
#define  VIEW_MESSAGE_XML           "setting/dlgmessageview.xml"
#define  VIEW_MESSAGE_TITLE         "BoxGuideTitle"
#define  VIEW_MESSAGE_SCROLL        "scrollCtrl"
#define SCROLL_PAGE_COUNT 1
}

CBaseDialog * CDlgViewMessage::CreateInstance()
{
    CDlgViewMessage * pDlg = new CDlgViewMessage;

    if (NULL != pDlg)
    {
        pDlg->loadContent(VIEW_MESSAGE_XML);
    }

    return pDlg;
}

CDlgViewMessage::CDlgViewMessage()
    : m_pScroll(NULL)
{
}

CDlgViewMessage::~CDlgViewMessage()
{
}

void CDlgViewMessage::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);

    m_GuideTitle.SetTitleView(getViewById(VIEW_MESSAGE_TITLE));
    m_pScroll = static_cast<xScrollBar *>(getViewById(VIEW_MESSAGE_SCROLL));
}

void CDlgViewMessage::RefreshTitle(const yl::string & strTitle)
{
    yl::string strRealTitle = strTitle;
    if (Edk_IsEnable() && EDK_IsTipModeEnable())
    {
        strRealTitle = EdkUI_GetMenuIDByPageData(kszPageIDTextMsgView, "", -1);
        m_GuideTitle.SetTitle(strRealTitle);
    }
    else if (!strRealTitle.empty())
    {
        m_GuideTitle.SetTitle(strRealTitle);
    }

    m_GuideTitle.SetGuideInfo("");

    if (NULL == g_pTextMessageController)
    {
        return;
    }

    yl::string strPageInfo = "";

    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    int nIndex = g_pTextMessageController->GetTempDataIndex();
    TextMsgSimpListData txtMsgList;
    if (-1 == nIndex || !g_pTextMessageController->GetMessageList(&txtMsgList, stDetailData.m_eGType))
    {
        return;
    }
    int nTotalNum = txtMsgList.m_listMessage.size();
    ++nIndex;
    strPageInfo = commonAPI_FormatString("%d/%d", nIndex, nTotalNum);
    m_GuideTitle.SetPageInfo(strPageInfo);
}

void CDlgViewMessage::Init()
{
    RefreshTitle();
}

void CDlgViewMessage::RefreshUI(bool bReLoadData /*= false*/)
{
    CDlgSettingUI::RefreshUI(bReLoadData);

    CTextMessageViewDelegate * pDelegate = static_cast<CTextMessageViewDelegate *>
                                           (m_pSettingUIDelegate);
    if (NULL == pDelegate || NULL == m_pScroll)
    {
        return ;
    }

    int nTotal = pDelegate->GetTotalPage();
    if (nTotal <= 1)
    {
        m_pScroll->hide();
        return;
    }
    else
    {
        m_pScroll->show();
    }

    m_pScroll->SetScrollInfo(SCROLL_PAGE_COUNT, pDelegate->GetTotalPage(), pDelegate->getCurrentPage());
}

void CDlgViewMessage::InitExtraData(void * pData)
{
    if (NULL != pData)
    {
        int nGType = *(int *)pData;
        TextMsgItemDetailData stInboxData;
        if (TEXT_MSG_GROUP_IN == nGType
                && g_pTextMessageController->GetMessageDetail(&stInboxData, TEXT_MSG_GROUP_IN, 0))
        {
            g_pTextMessageController->SetTempTextMsgDetailData(stInboxData);
        }
        RefreshTitle();
    }

    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->InitExtraData(pData);
    }
}

void CDlgViewMessage::RefreshTitle()
{
    if (NULL == g_pTextMessageController)
    {
        return;
    }
    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();

    yl::string strTitle;
    if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
    {
#ifndef IF_LCD_SIZE_132_64
        if (28 == devicelib_GetPhoneType())
        {
            strTitle = stDetailData.m_strFromName + "<" + stDetailData.m_strFromName + "@" +
                       stDetailData.m_strFromServer + ">  ";
        }
        else
#endif  // IF_LCD_SIZE_132_64
        {
            strTitle = _LANG(TRID_FROM);
            strTitle += ":";
            strTitle += stDetailData.m_strFromName;
        }
    }
    else if (TEXT_MSG_GROUP_SENDING == stDetailData.m_eGType
             || TEXT_MSG_GROUP_SENT == stDetailData.m_eGType)
    {
#ifndef IF_LCD_SIZE_132_64
        if (28 == devicelib_GetPhoneType())
        {
            // sent outbox view 页面的特殊的title形式
#ifndef _WIN32
#warning acc_GetAccountIDByName
#endif
            int iAccountID;// = acc_GetAccountIDByName(stDetailData.m_strToName.c_str());
            yl::string strAccountPort = configParser_GetCfgItemStringValue(kszAccountSIPServerPort, iAccountID);
            strTitle = stDetailData.m_strToName + "@" + stDetailData.m_strToServer;

            // 如果没获取到PORT，就没有冒号“：”
            if (!strAccountPort.empty())
            {
                strTitle += (":" + strAccountPort);
            }
        }
        else
#endif  // IF_LCD_SIZE_132_64
        {
            strTitle = _LANG(TRID_TO);
            strTitle += ":";
            strTitle += stDetailData.m_strToName;
        }
    }

    RefreshTitle(strTitle);
}

#include "bsftcallloguihelper.h"
#include "bsftcalllogcontroler.h"
#include "bsftcontactcontroler.h"
#include "directorylistaction.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

#if IF_FEATURE_MUTI_CALLLOG
#include "callloggroupdelegate.h"
#include "account/include/modaccount.h"
#endif


// 从broadsoft时间中提取出月日和时间
namespace
{
bool ParseBSFTCalllogTime(yl::string & strTime, const char * pszTime)
{
    if (NULL == pszTime)
    {
        return false;
    }

    char strYear[20] = {0}, strMonth[20] = {0}, strDay[20] = {0}, strHour[20] = {0}, strMinite[20] = {0},
                                           strSecond[20] = {0}, strMilliSecond[20] = {0}, strTimeZene[20] = {0};

    sscanf(pszTime, "%[^-]-%[^-]-%[^T]T%[^:]:%[^:]:%[^.].%[^-+]%s",
           strYear, strMonth, strDay, strHour, strMinite, strSecond, strMilliSecond, strTimeZene);

    strTime.append(strMonth);
    strTime.append(" ");
    strTime.append(strDay);
    strTime.append(" ");
    strTime.append(strHour);
    strTime.append(":");
    strTime.append(strMinite);

    return true;
}

time_t TransformTime2Time_t(LPCSTR strCallLog, BOOL bHaveSecond)
{
    //2010-09-01T04:18:24.491-04:00
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;
    int iZoneHour = 0;
    int iZoneMinute = 0;
    sscanf((char *)strCallLog, "%d-%d-%dT%d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
    yl::string strLogTime = (yl::string)(strCallLog);
    yl::string strTimeZone = "";
    int nLogTimeLen = strLogTime.length();

    if (nLogTimeLen >= 6)
    {
        strTimeZone = strLogTime.substr(nLogTimeLen - 6, 6);
    }

    sscanf(strTimeZone.c_str(), "%d:%d", &iZoneHour, &iZoneMinute);

    yl::string  szRetVal = "";

    //将时间转化成time_t
    tm tmWhen;
    tmWhen.tm_year = iYear - 1900;
    if (iMonth < 0)
    {
        iMonth = 0;
    }
    tmWhen.tm_mon = iMonth - 1;
    tmWhen.tm_mday = iDay;
    tmWhen.tm_hour = iHour ;
    tmWhen.tm_min = iMinute;
    if (bHaveSecond)
    {
        tmWhen.tm_sec = iSecond;
    }

    time_t tTime   =   mktime(&tmWhen);
    return tTime;
}
}

CBSFTCallLogUIHelper::CBSFTCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_BROADSOFT)
{
}

CBSFTCallLogUIHelper::~CBSFTCallLogUIHelper()
{
}

CBSFTCallLogUIHelper * CBSFTCallLogUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || CALL_LOG_TYPE_BROADSOFT != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBSFTCallLogUIHelper *)pUIHelper.Get();
}

void CBSFTCallLogUIHelper::InitSettingTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactListTitlePtr pContactListTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactListTitle::IsContactListTitle(pTitle))
    {
        bAdd = false;
        pContactListTitle = (CContactListTitlePtr)pTitle;
    }
    else
    {
        pContactListTitle = new CContactListTitle;
        if (NULL == pContactListTitle)
        {
            return;
        }
    }

    pContactListTitle->SetAction(pDelegate.Get());

    pContactListTitle->SetBtnCount(1);
    pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CBSFTCallLogUIHelper::InitDetailTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CBSFTCallLogControler * pBSFTCallLog = CBSFTCallLogControler::GetControler(pControler);

    if (NULL == pBSFTCallLog)
    {
        return;
    }

    bool bAdd = true;
    CContactDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactDetailTitle::IsContactDetailTitle(pTitle))
    {
        bAdd = false;
        pDetailTitle = (CContactDetailTitlePtr)pTitle;
    }
    else
    {
        pDetailTitle = new CContactDetailTitle;
        if (NULL == pDetailTitle)
        {
            return;
        }
    }

    int nButtonCount = 2;
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
    {
        nButtonCount++;
    }
#endif

    if (!CBSFTContactControler::IsExitPersonal() || pBSFTCallLog->IsPersonalCalllog())
    {
        pDetailTitle->SetBtnCount(nButtonCount);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
#if DIR_SUPPORT_NET_CALLLOG_DELETE
        if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
        }
#endif
    }
    else
    {
        nButtonCount++;

        pDetailTitle->SetBtnCount(nButtonCount);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
#if DIR_SUPPORT_NET_CALLLOG_DELETE
        if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
        }
#endif
    }

    pDetailTitle->SetAction(pDelegate.Get());

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pDetailTitle);
    }
    else
    {
        pDetailTitle->RefreshTitle();
    }
}

int CBSFTCallLogUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_EDIT_CALLLOG:
        {
            return DIR_ACTION_BSFTLOG_EDIT;
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL;
            }
        }
        break;
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
            {
                return DIR_ACTION_BSFT_DELETE_CALLLOG;
            }
        }
        break;
#endif
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CBSFTCallLogUIHelper::OnAttachFrameList()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_SETTING == nState)
    {
        InitSettingTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_DETAIL_ITEM_COUNT);
        return;
    }

    CCommonUIHelper::OnAttachFrameList();
}

bool CBSFTCallLogUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    BSFTCallLogRecord * pCallLog = CBSFTCallLogControler::GetBSFTCallLog(pDetail);

    DIRUI_INFO("CBSFTCallLogUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pCallLog, nDataIndex, nDataCount, m_pFrameList);

    if (NULL == m_pFrameList || NULL == pCallLog)
    {
        return false;
    }

    //BSFT callog 时间转换
    yl::string strBornTick = Timet2DetailString(TransformTime2Time_t(pCallLog->m_strBornTick.c_str(),
                             true));
    QString arrTitle[3] = {LANG_TRANSLATE(TRID_NAME), LANG_TRANSLATE(TRID_NUMBER),
                           LANG_TRANSLATE(TRID_TIME)
                          };
    QString arrValue[3] = {toQString(pCallLog->GetDisplayName(), true), toQString(pCallLog->GetNumber(), true),
                           toQString(strBornTick)
                          };

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nItemIndex = 0;

    for (; nItemIndex < nMaxPageTotalCount && nItemIndex < 3; ++nItemIndex)
    {
        RefreshDetailItem(nItemIndex, nItemIndex, 3, arrTitle[nItemIndex], arrValue[nItemIndex]);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
    return true;
}

bool CBSFTCallLogUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        SmartPtr & pDetail)
{
    DIRUI_INFO("CBSFTCallLogUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d)", nId, nItemIndex,
               nDataIndex);

#if IF_FEATURE_MUTI_CALLLOG
    BSFTLogGroupRecord * pGroup = CBSFTCallLogControler::GetBSFTCallLogGroup(pDetail);

    if (NULL != pGroup)
    {
        yl::string strTemp;
        if (BsftLog_IsMutilLogEnable())
        {
            strTemp = acc_GetAccountShowText(BsftLog_GetAccountIDByGroupID(nId), configParser_GetConfigInt(kszNetworkCalllogCallerNameType));
        }
        else
        {
            strTemp = pGroup->GetDisplayName();
        }

        QString strGroupName = toQString(strTemp, true);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strGroupName);
    }
#endif

    BSFTCallLogRecord * pCallLog = CBSFTCallLogControler::GetBSFTCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

    yl::string strHead = "";
    yl::string strBornTick = Timet2String(TransformTime2Time_t(pCallLog->m_strBornTick.c_str(), true),
                                          "", true, false, false, false, true);

    QString strName = toQString(pCallLog->GetDisplayName(), true);
    bool bHaveName = !strName.isEmpty();

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    const yl::string & strCalllogName = pCallLog->GetDisplayName();
    yl::string strCalllogNumber = pCallLog->GetNumber();
    yl::string strValue;
    GetTextOfRecordToShow(strValue, strCalllogName, strCalllogNumber, "");
    strName = toQString(strValue, true);
    bHaveName = true;
#endif

    return RefreshCallLogItem(nId, nItemIndex, nDataIndex, nDataCount, pCallLog->GetLogType(),
                              bHaveName, strName, pCallLog->GetNumber(), strBornTick, strHead);
}

#if IF_FEATURE_MUTI_CALLLOG
void CBSFTCallLogUIHelper::RefreshCallLogGroup(int nGroupID, bool bRefresh /*= false*/)
{
    DIRUI_INFO("CBSFTCallLogUIHelper::RefreshCallLogGroup [%d] [%d]", nGroupID, bRefresh);
    CGroupDelegatePtr pDelegate = g_DirectoryMgr.GetGroupDelegate();
    CCallLogGroupDelegate * pGroupDelegate = CCallLogGroupDelegate::GetGroupDelegate(pDelegate);
    if (pGroupDelegate != NULL)
    {
        if (bRefresh)
        {
            int nAccGroupID = kszRootId;
            if (nGroupID > 0)
            {
                nAccGroupID = Dir_GetParentIdById(nGroupID, BSFT_LOG);
            }

            pGroupDelegate->RefreshBroadSoftGroup(nAccGroupID);

        }

        pGroupDelegate->SetDefaultGroup(CALL_LOG_TYPE_BROADSOFT, nGroupID);
    }
}
#endif


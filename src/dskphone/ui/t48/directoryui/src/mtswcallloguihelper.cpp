#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcallloguihelper.h"
#include "mtswcalllogcontroler.h"
//#include "mtswcontactcontroler.h"
#include "directorylistaction.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "service_system.h"


// 从broadsoft时间中提取出月日和时间
namespace
{
time_t TransformTime2Time_t(LPCSTR strCallLog, BOOL bHaveSecond)
{
    //2014-11-18 21:12:38
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;

    sscanf((char *)strCallLog, "%d-%d-%d %d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);

    //将时间转化成time_t
    tm tmWhen;
    tmWhen.tm_year = iYear - 1900;
    if (iMonth < 0)
    {
        iMonth = 0;
    }
    tmWhen.tm_mon = iMonth - 1;
    tmWhen.tm_mday = iDay;
    tmWhen.tm_hour = iHour;
    tmWhen.tm_min = iMinute;
    if (bHaveSecond)
    {
        tmWhen.tm_sec = iSecond;
    }

    time_t tTime = mktime(&tmWhen);

    //DIR_INFO("-=-=-=-=  TransformTime2Time_t: %d-%02d-%02d %02d:%02d:%02d  =-=-=-=-",iYear, iMonth, iDay, iHour, iMinute, iSecond);
    return tTime;
}
}

CMTSWCallLogUIHelper::CMTSWCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_MTSW)
{
}

CMTSWCallLogUIHelper::~CMTSWCallLogUIHelper()
{
}

CMTSWCallLogUIHelper * CMTSWCallLogUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || CALL_LOG_TYPE_MTSW != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CMTSWCallLogUIHelper *)pUIHelper.Get();
}

void CMTSWCallLogUIHelper::InitSettingTitle()
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
#if IF_FEATURE_GBBOOK
    pContactListTitle->SetCheckable(true);
#endif

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

void CMTSWCallLogUIHelper::InitDetailTitle()
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
    CMTSWCallLogControler * pMTSWCallLog = CMTSWCallLogControler::GetControler(pControler);

    if (NULL == pMTSWCallLog)
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

    if (MtswDir_IsEnable())
    {
        pDetailTitle->SetBtnCount(4);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO_PHONEBOOK));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_MTSW_CONTACT, LANG_TRANSLATE(TRID_ADD_MTSW_CONTACT));
    }
    else
    {
        pDetailTitle->SetBtnCount(3);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO_PHONEBOOK));
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

int CMTSWCallLogUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_EDIT_CALLLOG:
        {
            return DIR_ACTION_LCLOG_EDIT;
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
            }
        }
        break;
    case LIST_TITLE_ACTION_MOVE_MTSW_CONTACT:
        {
            if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_MTSW_ADD_CONTACTLIST;
            }
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CMTSWCallLogUIHelper::OnAttachFrameList()
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

bool CMTSWCallLogUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    MTSWCallLogRecord * pCallLog = CMTSWCallLogControler::GetMTSWCallLog(pDetail);

    DIRUI_INFO("CMTSWCallLogUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pCallLog, nDataIndex, nDataCount, m_pFrameList);

    if (NULL == m_pFrameList || NULL == pCallLog)
    {
        return false;
    }

    yl::string strDisplayName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();

    if (strDisplayName.empty() && strNumber.empty())
    {
        strDisplayName = TRID_ANONYMOUS;
        strNumber = TRID_ANONYMOUS;
    }

    //meta callog 时间转换
    yl::string strBornTick = Timet2DetailString(TransformTime2Time_t(pCallLog->m_strBornTick.c_str(),
                             TRUE));
    QString arrTitle[4] = {LANG_TRANSLATE(TRID_NAME), LANG_TRANSLATE(TRID_NUMBER),
                           LANG_TRANSLATE(TRID_TIME), LANG_TRANSLATE(TRID_DURATION)
                          };
    QString arrValue[4] = {toQString(strDisplayName, true), toQString(strNumber, true),
                           toQString(strBornTick), toQString(pCallLog->m_strDuration.c_str())
                          };

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nItemIndex = 0;

    for (; nItemIndex < nMaxPageTotalCount && nItemIndex < 4; ++nItemIndex)
    {
        RefreshDetailItem(nItemIndex, nItemIndex, 4, arrTitle[nItemIndex], arrValue[nItemIndex]);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
    return true;
}

bool CMTSWCallLogUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        SmartPtr & pDetail)
{
    MTSWCallLogRecord * pCallLog = CMTSWCallLogControler::GetMTSWCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

    yl::string strBornTick = Timet2String(TransformTime2Time_t(pCallLog->m_strBornTick.c_str(), TRUE),
                                          "", true, false, false, false, true);
    yl::string strNameTmp = pCallLog->GetDisplayName();
    bool bHaveName = !strNameTmp.empty();

    if (strNameTmp.empty())
    {
        strNameTmp = pCallLog->GetNumber();
        if (strNameTmp.empty())
        {
            strNameTmp = TRID_ANONYMOUS;
        }
    }

    QString strName = toQString(strNameTmp, true);

    yl::string strHead = "";

    return RefreshCallLogItem(nId, nItemIndex, nDataIndex, nDataCount, pCallLog->GetLogType(),
                              bHaveName, strName, pCallLog->GetNumber(), strBornTick, strHead);
}
#endif //IF_FEATURE_METASWITCH_CALLLOG

#include "localcallloguihelper.h"
#include "directorymgr.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "localcalllogcontroler.h"
#include "localcontactcontroler.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "localcalllogcontroler.h"
#include "account/include/modaccount.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "commonunits/commonunits_def.h"
#include "qtcommon/qmisc.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "uikernel/languagehelper.h"
#include <resource/modresource.h>


namespace
{
void GetCallLogDuration(int nDuration, yl::string & strDuration)
{
    char szBuffer[128];

    int nHour = nDuration / 3600;
    int nMinSec = nDuration % 3600;
    int nMin = nMinSec / 60;
    int nSec = nMinSec % 60;

    snprintf(szBuffer, sizeof(szBuffer), "%02d:%02d:%02d", nHour, nMin, nSec);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    strDuration = szBuffer;
}
}

CLocalCallLogUIHelper::CLocalCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_LOCAL)
{
}

CLocalCallLogUIHelper::~CLocalCallLogUIHelper()
{
}

CLocalCallLogUIHelper * CLocalCallLogUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || CALL_LOG_TYPE_LOCAL != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CLocalCallLogUIHelper *)pUIHelper.Get();
}

void CLocalCallLogUIHelper::InitSettingTitle()
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
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        pContactListTitle->SetBtnCount(2);
        pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
    }
    else
    {
        pContactListTitle->SetBtnCount(1);
    }

    pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CLocalCallLogUIHelper::InitDetailTitle()
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
    CLocalCallLogControler * pLocalCallLog = CLocalCallLogControler::GetControler(pControler);

    if (NULL == pLocalCallLog)
    {
        return;
    }

    yl::string strName;

    if (!pLocalCallLog->CCommonDirCtrl::GetContactName(strName))
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

#if 1//zzl
    int iBtnCount = 3;
    bool bShowAddContact = false;
    bool bShowAddBlackList = false;

    if ((strName.empty()
            || !CLocalContactControler::IsExitContact(strName))
            && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
    {
        bShowAddContact = true;
        iBtnCount++;
    }
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        bShowAddBlackList = true;
        iBtnCount++;
    }

    pDetailTitle->SetBtnCount(iBtnCount);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    if (bShowAddContact)
    {
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));
    }
    pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
    if (bShowAddBlackList)
    {
        pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
    }
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
#else
    if (!strName.empty() && CLocalContactControler::IsExitContact(strName))
    {
        pDetailTitle->SetBtnCount(4);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else
    {
        pDetailTitle->SetBtnCount(5);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_EDIT_CALLLOG, LANG_TRANSLATE(TRID_EDIT));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
#endif
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

int CLocalCallLogUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
        }
        break;
    case LIST_TITLE_ACTION_EDIT_CALLLOG:
        {
            return DIR_ACTION_LCLOG_EDIT;
        }
        break;
    case LIST_TITLE_ACTION_ADD_BLACKLIST:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST;
            }
        }
        break;
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LCLOG_DEL_CALL_LOG;
            }
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CLocalCallLogUIHelper::OnAttachFrameList()
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

bool CLocalCallLogUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    LocalLogItemRecord * pCallLog = CLocalCallLogControler::GetLocalCallLog(pDetail);

    DIRUI_INFO("CLocalCallLogUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pCallLog, nDataIndex, nDataCount, m_pFrameList);

    if (NULL == m_pFrameList || NULL == pCallLog)
    {
        return false;
    }

    if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();

#if IF_FEATURE_HIDE_CALLLOG_DETAIL_LINE
    int nDetailInfoCount = 4;
#else
    int nDetailInfoCount = 6;
#endif
    m_pFrameList->SetTotalDataCount(nDetailInfoCount);

    yl::string strTime = Timet2DetailString(pCallLog->GetAdjustTime());
    yl::string strName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();
    yl::string strDuration;

    if (strName.empty())
    {
        strName = strNumber;
    }

    QString strTransName;

    TransBsftFACText(strName, strTransName);

    GetCallLogDuration(atoi(pCallLog->m_strDuration.c_str()), strDuration);

#if IF_FEATURE_HIDE_CALLLOG_DETAIL_LINE
    QString arrTitle[4] = {LANG_TRANSLATE(TRID_NAME), LANG_TRANSLATE(TRID_NUMBER),
                           LANG_TRANSLATE(TRID_TIME), LANG_TRANSLATE(TRID_DURATION)
                          };

    QString arrValue[4] = {strTransName, toQString(strNumber, true), toQString(strTime, true),
                           toQString(strDuration)
                          };
#else
    QString strLine;

    if (pCallLog->m_strLocalSipName.empty())
    {
        if (Bluetooth_GetBtAccountID() == pCallLog->m_nLine)
        {
            strLine = LANG_TRANSLATE(TRID_BLUETOOTH_MOBILE_LINE);
        }
        else
        {
            strLine = LANG_TRANSLATE(TRID_UNKNOWN_LINE);
        }
    }
    else
    {
        if (acc_GetRegisterName(acc_GetDirectIPAccountID()) == pCallLog->m_strLocalSipName)
        {
            strLine = LANG_TRANSLATE(TRID_DIRECT_IP_LINE);
        }
        else
        {
            int nAccountId = acc_GetAccountIDByName(pCallLog->m_strLocalSipName.c_str(),
                                                    pCallLog->m_strLocalSipServer.c_str());

            // http://10.2.1.199/Bug.php?BugID=38583
            if (-1 == nAccountId)
            {
                strLine = LANG_TRANSLATE(TRID_UNKNOWN);
            }
            else
            {
                strLine = LANG_TRANSLATE(TRID_LINE) + QString(" %1").arg(nAccountId + 1);
            }
        }
    }

    QString strRelation = "";
    bool bResult = false;
    if (LocalLog_DetailedEnable())
    {
        bResult = GetRelationDescribe(pCallLog->m_eRelation, pCallLog->m_strRelationNum, strRelation);
    }
    if (!bResult)
    {
        nDetailInfoCount--;
    }

    QString arrTitle[6] = {LANG_TRANSLATE(TRID_NAME), LANG_TRANSLATE(TRID_NUMBER),
                           LANG_TRANSLATE(TRID_TIME), LANG_TRANSLATE(TRID_LINE),
                           LANG_TRANSLATE(TRID_DURATION), LANG_TRANSLATE(TRID_RELATION)
                          };

    QString arrValue[6] = {strTransName, toQString(strNumber, true), toQString(strTime, true),
                           strLine, toQString(strDuration), strRelation
                          };
#endif
    int nItemIndex = 0;

    for (; nItemIndex < nMaxPageTotalCount && nItemIndex < nDetailInfoCount; ++nItemIndex)
    {
        RefreshDetailItem(nItemIndex, nItemIndex, nDetailInfoCount, arrTitle[nItemIndex],
                          arrValue[nItemIndex]);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
    return true;
}

bool CLocalCallLogUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        SmartPtr & pDetail)
{
    LocalLogItemRecord * pCallLog = CLocalCallLogControler::GetLocalCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

    yl::string strBornTick = Timet2String(pCallLog->GetAdjustTime());
    const yl::string & strName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();
    yl::string strValue;
    bool bHaveName = !strName.empty();

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    GetTextOfRecordToShow(strValue, strName, strNumber, pCallLog->m_strRemoteSipServer);
#else
    if (strName.empty())
    {
        strValue = strNumber;
    }
    else
    {
        strValue = strName;
    }
#endif

    QString strTransName;

    TransBsftFACText(strValue, strTransName);

    yl::string strHead = pCallLog->GetPhoto();

    if (!strHead.empty())
    {
        yl::string strPath;
        yl::string strFile;

        res_toRealPath(strHead, PHFILE_TYPE_CONTACT_IMAGE);
        ParseFilePath(strHead, strPath, strFile);

        DIRUI_INFO("Path:%s File:%s", strPath.c_str(), strFile.c_str());

        strHead = strPath + CONTACT_PHOTO_ICON_PATH + strFile;
    }

    CALL_TYPE eCallType = pCallLog->GetLogType();

    return RefreshCallLogItem(nId, nItemIndex, nDataIndex, nDataCount, eCallType,
                              bHaveName, strTransName, strNumber, strBornTick, strHead);
}

bool CLocalCallLogUIHelper::IsSureDelete(int nId)
{
    CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_THE_RECORD,
                                DIR_ACTION_LCLOG_DEL_CALL_LOG, nId);

    return false;
}

bool CLocalCallLogUIHelper::GetRelationDescribe(CALL_RELATION eRelation,
        yl::string & strRelationNum, QString & strOutput)
{
    bool bResult = false;

    switch (eRelation)
    {
    case CR_BUSY:
        strOutput = LANG_TRANSLATE(TRID_BUSY);
        bResult = true;
        break;
    case CR_REJECTED:
        strOutput = LANG_TRANSLATE(TRID_REJECTED);
        bResult = true;
        break;
    case CR_TRANS_TO:
        strOutput = LANG_TRANSLATE(TRID_TRANSFER_TO);
        bResult = true;
        break;
    case CR_FWD_TO:
        strOutput = LANG_TRANSLATE(TRID_FORWARD_SELECTIVE_FORWARD_TO);
        bResult = true;
        break;
    default:
        break;
    }

    if (!strRelationNum.empty() && !strOutput.isEmpty())
    {
        strOutput += " ";
        strOutput += toQString(strRelationNum);
    }
    else if (!strRelationNum.empty())
    {
        strOutput = toQString(strRelationNum);
        bResult = true;
    }

    return bResult;
}


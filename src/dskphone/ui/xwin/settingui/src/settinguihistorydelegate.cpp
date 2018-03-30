#include "settinguihistorydelegate.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "contacts/directory/include/moddirinterface.h"
#include "directoryui/include/moddirectoryui.h"
#include "setuidatadefine.h"


namespace
{
const char * kszHistoryLocal = "HistoryLocal";
const char * kszHistoryBsft = "HistoryBsft";
#if IF_FEATURE_METASWITCH_CALLLOG
const char * kszHistoryMTSW = "HistoryMTSW";
#endif
}

CSettingUIHistoryDelegate::CSettingUIHistoryDelegate()
{

}

CSettingUIHistoryDelegate::~CSettingUIHistoryDelegate()
{

}

CSettingUIDelegateBase * CSettingUIHistoryDelegate::CreateHistoryDelegate()
{
    CSettingUIHistoryDelegate * pHistoryDelegate = new CSettingUIHistoryDelegate;

    return pHistoryDelegate;
}

bool CSettingUIHistoryDelegate::EnterHistoryDirectly()
{
    if (BsftLog_IsEnable())
    {
        return false;
    }
#if IF_FEATURE_METASWITCH_CALLLOG
    return false;
#endif

    DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, knInvalidRId);

    return true;
}

CSettingUIPageDataBase * CSettingUIHistoryDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase * pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData || !bReLoad)
    {
        return pPageData;
    }

    yl::string strEmpty = "";
    AddMenuItem(pPageData, kszHistoryLocal, _LANG(TRID_LOCAL_HISTORY), strEmpty);

    if (BsftLog_IsEnable())
    {
        AddMenuItem(pPageData, kszHistoryBsft, _LANG(TRID_NETWORK_CALLLOG), strEmpty);
    }

#if IF_FEATURE_METASWITCH_CALLLOG
    AddMenuItem(pPageData, kszHistoryMTSW, _LANG(TRID_MTSW_CALLLOG), strEmpty);
#endif
    return pPageData;
}

bool CSettingUIHistoryDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        const yl::string & strID = GetItemIDByDataIndex(iDataIndex);

        if (kszHistoryLocal == strID)
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, knInvalidRId);

            return true;
        }
        else if (kszHistoryBsft == strID)
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, knInvalidRId);

            return true;
        }
#if IF_FEATURE_METASWITCH_CALLLOG
        else if (kszHistoryMTSW == strID)
        {
            return DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, kszAllContactId);
        }
#endif
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

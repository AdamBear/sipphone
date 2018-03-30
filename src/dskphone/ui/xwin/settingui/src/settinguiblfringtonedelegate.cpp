#include "settinguiblfringtonedelegate.h"

#if IF_SUPPORT_BLF_RING
#include "setuidatadefine.h"
#include "settinguiradioitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/soundcontroller.h"
#include "account/include/modaccount.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "idlescreen/include/modidlescreen.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define MSGBOX_ID_SET_FAIL -1
#define PLAY_RING_DELAY_TIME 500
}

CSettingUIBLFRingToneDelegate::CSettingUIBLFRingToneDelegate()
{

}

CSettingUIBLFRingToneDelegate::~CSettingUIBLFRingToneDelegate()
{
    ResetSatus();
}

CSettingUIDelegateBase* CSettingUIBLFRingToneDelegate::CreateBLFRingToneDelegate()
{
    CSettingUIBLFRingToneDelegate* pDelegate = new CSettingUIBLFRingToneDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUIBLFRingToneDelegate::LoadPageData(bool bReLoad /* = false*/)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        AddBlfListRing(m_pPageData);
    }

    return m_pPageData;
}

bool CSettingUIBLFRingToneDelegate::SavePageData(CSettingUIPageDataBase* pPageData,
        bool bBackToPrevious /* = true*/, bool bPopHint /* = true*/)
{
    if (NULL == pPageData || NULL == g_pSoundController || !pPageData->IsEnable())
    {
        return false;
    }

    ResetSatus();

    int nCurremtIndex = GetFocusIndex();
    if (nCurremtIndex >= m_vecRingPath.size() || nCurremtIndex < 0)
    {
        return false;
    }

    yl::string strFilePath = m_vecRingPath[nCurremtIndex];

    if (strFilePath != g_pSoundController->GetCurrentBlfRingPath())
    {
        if (!g_pSoundController->SetCureentBlfRingPath(strFilePath))
        {
            ShowNoteMessgaeBox(_LANG(TRID_SET_FAILED), MSGBOX_ID_SET_FAIL);
            return false;
        }
    }

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

bool CSettingUIBLFRingToneDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_FOCUS == eActionType)
    {
        OnItemFocusChange(iDataIndex);
        return true;
    }

    return CSettingUIAccountDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettingUIBLFRingToneDelegate::AddBlfListRing(CSettingUIPageDataBase* pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    m_vecRingPath.clear();

    //加载List内容
    RingFileListData stRingData;
    if (NULL == g_pSoundController || !g_pSoundController->GetRingFileList(&stRingData))
    {
        return;
    }

    bool bSetChecked = false;
    int nFocusIndex = 0;
    const yl::string strCurrentRing = g_pSoundController->GetCurrentBlfRingPath();

    LIST_RING_FILE_DATA::ListIterator iter = stRingData.m_listRing.begin();
    for (int i = 0; iter != stRingData.m_listRing.end(); ++iter, ++i)
    {
        RingFileData stData = *iter;

        if (!stData.m_strRingDisplayName.empty())
        {
            //非账号铃声，第一项为common
            CSettingUIRadioItemDataPtr pItemData = new CSettingUIRadioItemData;
            if (NULL != pItemData)
            {
                pItemData->SetShowIndex(true);
                pItemData->SetDisplayName(stData.m_strRingDisplayName);

                if (!pPageData->AddItem(pItemData))
                {
                    delete pItemData;
                    continue;
                }
                //如果该item值与当前使用铃声路径相等
                if (stData.m_strRingPath == strCurrentRing)
                {
                    nFocusIndex = i;
                    pItemData->SetCheck(true);
                    bSetChecked = true;
                }
                m_vecRingPath.push_back(stData.m_strRingPath);
            }
        }
    }

    //设置焦点及选中项
    SetFocusIndex(nFocusIndex);

    if (!bSetChecked)
    {
        CSettingUIRadioItemDataPtr pCheckItem = CSettingUIRadioItemData::GetRadioListDataItem(
                pPageData->GetItemByDataIndex(0));
        if (NULL != pCheckItem)
        {
            pCheckItem->SetCheck(true);
        }
    }
}
#endif

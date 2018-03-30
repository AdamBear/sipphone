#include "settinguiringtonesettingdelegate.h"
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
#include "settingui/include/settinguidefine.h"

namespace
{
const yl::string strCommon = "common";
#define MSGBOX_ID_SET_FAIL -1
#define PLAY_RING_DELAY_TIME 500
#define RING_MIN_VALUE      0
#define RING_MAX_VALUE      15
}

CSettingUIRingToneSettingDelegate::CSettingUIRingToneSettingDelegate()
    : m_bKeyPressSound(false)
{

}

CSettingUIRingToneSettingDelegate::~CSettingUIRingToneSettingDelegate()
{
    ResetSatus();
}

CSettingUIDelegateBase * CSettingUIRingToneSettingDelegate::CreateRingToneSettingDelegate()
{
    CSettingUIRingToneSettingDelegate * pDelegate = new CSettingUIRingToneSettingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUIRingToneSettingDelegate::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (bPress && (PHONE_KEY_VOLUME_DECREASE == nKeyCode || PHONE_KEY_VOLUME_INCREASE == nKeyCode)
            && voice_IsVolumeCanChange())
    {
        int nVolume = voice_GetVolume();
        nVolume = (PHONE_KEY_VOLUME_INCREASE == nKeyCode) ? nVolume + 1 : nVolume - 1;
        voice_SetVolume(nVolume, VT_AUTO, TRUE);

        if (0 == voice_GetVolume())
        {
            // 音量为0，就显示静音状态
            idleScreen_SetStatus(PS_STATE_RING_MUTE);
        }
        else
        {
            // 音量改变，就撤销静音状态
            idleScreen_DropStatus(PS_STATE_RING_MUTE);
        }

        return true;
    }

    return CSettingUIAccountDelegateBase::ProcessKeyEvent(nKeyCode, bPress);
}

CSettingUIPageDataBase * CSettingUIRingToneSettingDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        AddListRing(m_pPageData);
#if IF_SUPPORT_RING_TONE_TITLE
        yl::string strTitle = IsAccount() ? acc_GetAccountShowText(m_iLineID - 1) : _LANG(
                                  TRID_COMMON_RINGTONE);
        m_pPageData->SetPageTitle(strTitle);
#endif
    }

    return m_pPageData;
}

bool CSettingUIRingToneSettingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
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

    if (strFilePath != g_pSoundController->GetCurrentRingPath(m_iLineID - 1))
    {
        // 当设置为common,应把账号下的铃声设置为空
        if (strFilePath == strCommon)
        {
            strFilePath = "";
        }

        if (!g_pSoundController->SetCurrentRingPath(strFilePath, m_iLineID - 1))
        {
            ShowNoteMessgaeBox(_LANG(TRID_SET_FAILED), MSGBOX_ID_SET_FAIL);
            return false;
        }
    }

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

bool CSettingUIRingToneSettingDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_FOCUS == eActionType)
    {
        OnItemFocusChange(iDataIndex);
        return true;
    }

    return CSettingUIAccountDelegateBase::OnAction(eActionType, iDataIndex);
}

bool CSettingUIRingToneSettingDelegate::IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix)
{
    iVolumeType = VT_RING;
    iMin = RING_MIN_VALUE;
    iMax = RING_MAX_VALUE;
    return true;
}

void CSettingUIRingToneSettingDelegate::AddListRing(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    m_vecRingPath.clear();

    if (IsAccount())
    {
        //非账号铃声，第一项为common
        CSettingUIRadioItemDataPtr pItemData = new CSettingUIRadioItemData;
        if (NULL != pItemData)
        {
            pItemData->SetShowIndex(true);
            pItemData->SetDisplayName(_LANG(TRID_COMMON_RINGTONE));
            if (!pPageData->AddItem(pItemData))
            {
                delete pItemData;
            }

            m_vecRingPath.push_back(strCommon);
        }
    }

    //加载List内容
    RingFileListData stRingData;
    if (NULL == g_pSoundController || !g_pSoundController->GetRingFileList(&stRingData))
    {
        return;
    }

    bool bSetChecked = false;
    int nFocusIndex = 0;
    const yl::string strCurrentRing = g_pSoundController->GetCurrentRingPath(m_iLineID - 1);

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
                    nFocusIndex = IsAccount() ? i + 1 : i;
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

bool CSettingUIRingToneSettingDelegate::IsAccount()
{
    return m_iLineID > 0;
}

void CSettingUIRingToneSettingDelegate::OnItemFocusChange(int nIndex)
{
    ResetSatus();
    m_tmPalyRing.SingleShot(PLAY_RING_DELAY_TIME, TIMER_METHOD(this,
                            CSettingUIRingToneSettingDelegate::OnStartPalyRing));
}

void CSettingUIRingToneSettingDelegate::OnStartPalyRing(xThreadTimer* timer, LPVOID pExtraData)
{
    ResetSatus();

    if (NULL == g_pSoundController)
    {
        return;
    }

    int nIndex = GetFocusIndex();
    if (nIndex < m_vecRingPath.size() && nIndex >= 0)
    {
        yl::string strPath = m_vecRingPath[nIndex];
        if (strCommon == strPath)
        {
            strPath = g_pSoundController->GetCommonRingPath();
        }

        g_pSoundController->PlayRing(strPath);
    }
}

void CSettingUIRingToneSettingDelegate::ResetSatus()
{
    m_tmPalyRing.KillTimer();
    if (NULL != g_pSoundController)
    {
        g_pSoundController->StopRing();
    }
}

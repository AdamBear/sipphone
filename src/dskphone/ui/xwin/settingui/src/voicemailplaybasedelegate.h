#ifndef _VOICEMAIL_PLAY_BASE_DELEGATE_H_
#define _VOICEMAIL_PLAY_BASE_DELEGATE_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL

#include "baseui/include/baseuicommon.h"
#include "settinguilogic/include/common.h"
#include <settingui/include/settinguidefine.h>

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguidirectoryassociatebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CVoiceMailTimerItemData;

#if IF_SUPPORT_SELECT_CONTACT_INFO
class CVoiceMailPlayBaseDelegate : public CDirectoryAssociateBaseDelegate
#else
class CVoiceMailPlayBaseDelegate : public CSettinguiDataDelegateBase
#endif
{
public:
    explicit CVoiceMailPlayBaseDelegate();
    virtual ~CVoiceMailPlayBaseDelegate();
    static CSettingUIDelegateBase* CreateVoiceMailPlayBaseDelegate();

public:
    virtual void OnTimeOut();

protected:
    void LoadVoiceMailPlayBoxItem(CSettingUIPageDataBase* pPageData, const yl::string& strTime,
                                  int nMaxTicks, bool bShowIcon, bool bShowStatus, int nPlayStatus,
                                  const yl::string& strID);
    void SetVoiceMailPlayByItemID(const yl::string& strItemID, int nPlayStatus);
    void SetVoiceMailPlay(CVoiceMailTimerItemData* pVMItemData, int nPlayStatus);
};
#endif

#endif //_VOICEMAIL_PLAY_BASE_DELEGATE_H_

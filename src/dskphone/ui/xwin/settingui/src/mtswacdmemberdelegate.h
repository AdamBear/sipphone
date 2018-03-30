#ifndef _MTSW_ACD_MEMBER_DELEGATE_H_
#define _MTSW_ACD_MEMBER_DELEGATE_H_

#if IF_FEATURE_METASWITCH_ACD
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"
#include "broadsoft/callcenter/include/cccommon.h"

typedef YLVector<SMLHGData>         VEC_SMLGG_DATA;
typedef YLVector<SMLHGMemberData>   VEC_MEMBER_DATA;

class CMtswAcdMemberDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswAcdMemberDelegate();
    virtual ~CMtswAcdMemberDelegate();
    static CSettingUIDelegateBase* CreatMtswAcdMemberDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);

private:
    void OnActionSend();
    //获取MLHG列表
    void GetMLHGMembers();
};
#endif

#endif //_MTSW_ACD_MEMBER_DELEGATE_H_

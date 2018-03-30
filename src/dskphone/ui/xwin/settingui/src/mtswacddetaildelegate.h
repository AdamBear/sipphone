#ifndef _MTSW_ACD_DETAIL_DELEGATE_H_
#define _MTSW_ACD_DETAIL_DELEGATE_H_

#if IF_FEATURE_METASWITCH_ACD
#include "settinguilogic/include/common.h"
#include "mtswacdmemberdelegate.h"

class CMtswAcdDetailDelegate : public CMtswAcdMemberDelegate
{
public:
    CMtswAcdDetailDelegate();
    virtual ~CMtswAcdDetailDelegate();
    static CSettingUIDelegateBase* CreatMtswAcdDetailDelegate();
    static LRESULT OnMTSWAcdDetailMsg(msgObject& msg);

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    virtual bool SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);

public:
    //获取MLHG列表
    void GetMLHGDetail();

protected:
    void OnActionMembers();

};
#endif

#endif //_MTSW_ACD_DETAIL_DELEGATE_H_

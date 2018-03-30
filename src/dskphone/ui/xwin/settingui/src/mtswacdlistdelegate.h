#ifndef _MTSW_ACDLIST_DELEGATE_H_
#define _MTSW_ACDLIST_DELEGATE_H_

#if IF_FEATURE_METASWITCH_ACD
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"
#include "broadsoft/callcenter/include/cccommon.h"

typedef YLVector<SMLHGData>  VEC_SMLGG_DATA;

class CMtswAcdListDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswAcdListDelegate();
    virtual ~CMtswAcdListDelegate();
    static CSettingUIDelegateBase* CreatMtswAcdListDelegate();
    static LRESULT OnMTSWAcdlistMsg(msgObject& msg);

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

public:
    //获取MLHG列表
    void GetMLHGList();

protected:
    yl::string GetAcdListTitle();
    bool SetCurrentMLHG();
    bool IsAllLogout();
};
#endif

#endif //_MTSW_ACDLIST_DELEGATE_H_

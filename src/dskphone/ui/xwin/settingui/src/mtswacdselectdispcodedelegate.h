#ifndef _MTSW_ACD_SELECT_DISPCODE_DELEGATE_H_
#define _MTSW_ACD_SELECT_DISPCODE_DELEGATE_H_

#if IF_FEATURE_METASWITCH_ACD
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"
#include "broadsoft/callcenter/include/cccommon.h"

typedef YLVector<SMLHGData>  VEC_SMLGG_DATA;
typedef YLVector<yl::string>  VEC_DISPCODE;
class CSettingUIComboBoxItemData;

class CMtswAcdSelectDispcodeDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswAcdSelectDispcodeDelegate();
    virtual ~CMtswAcdSelectDispcodeDelegate();
    static CSettingUIDelegateBase* CreatMtswAcdSelectDispcodeDelegate();
    static LRESULT OnMTSWAcdDetailMsg(msgObject& msg);

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    virtual bool SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);

public:
    //获取MLHG列表
    void GetSelectDispcode();

protected:
    void OnOKAction();
    CSettingUIComboBoxItemData* LoadComboboxItem(const yl::string& strItemID,
            const yl::string& strDisplayName, bool bReadOnly);
    void AddComboboxoption(CSettingUIComboBoxItemData* pItemData,
                           const yl::string& strDisplayName, const yl::string& strValue);
};
#endif

#endif //_MTSW_ACD_SELECT_DISPCODE_DELEGATE_H_

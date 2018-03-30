#ifndef _ACD_DISPOSITION_CODE_DELEGATE_H_
#define _ACD_DISPOSITION_CODE_DELEGATE_H_

#include "basesettinguidelegate.h"
#include "ylvector.h"
#include "settinguilogic/include/common.h"

class CACDDispositionCodeDelegate : public CSettingUIDelegateBase
{
public:
    CACDDispositionCodeDelegate();
    ~CACDDispositionCodeDelegate();

    static CSettingUIDelegateBase * CreateACDDispositionCodeDelegate();

public:
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);

    void SetCallID(int iCallID);

private:
    void PushbackItem(const yl::string & strCode, const yl::string & strName);

private:
    VEC_STRING_DATA m_codeVec;
    int m_nCallId;
};

#endif //_ACD_DISPOSITION_CODE_DELEGATE_H_

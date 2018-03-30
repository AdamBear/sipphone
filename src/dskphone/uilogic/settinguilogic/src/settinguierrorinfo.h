#ifndef __SETTINGUI_ERROR_INFO_H__
#define __SETTINGUI_ERROR_INFO_H__

#include "ylstl/ylstring.h"


class CSettingUIErrorInfo
{
public:
    CSettingUIErrorInfo();
    virtual ~CSettingUIErrorInfo();

public:
    void Reset();

    CSettingUIErrorInfo & operator = (const CSettingUIErrorInfo & objSoftkeyItemData);

    bool GetSoftkeyData(CSettingUIErrorInfo & objSoftkeyItemData);
    bool SetSoftkeyData(const CSettingUIErrorInfo & objSoftkeyItemData);

public:
    const yl::string & GetEmptyError();
    void SetEmptyError(const yl::string & strEmptyError);
    const yl::string & GetFormateError();
    void SetFormateError(const yl::string & strFormateError);
    const yl::string & GetRangeError();
    void SetRangeError(const yl::string & strRangeError);

private:
    yl::string              m_strEmptyError;
    yl::string              m_strFormateError;
    yl::string              m_strRangeError;
};

typedef     CSettingUIErrorInfo    *    CSettingUIErrorInfoPtr;

#endif


#ifndef __SETTINGUI_DATA_VALID_INFO_H__
#define __SETTINGUI_DATA_VALID_INFO_H__

#include "ylstl/ylstring.h"


class CSettingUIDataValidInfo
{
public:
    CSettingUIDataValidInfo();
    virtual ~CSettingUIDataValidInfo();

public:
    void Reset();

public:
    int GetValidIndex();
    void SetValidIndex(int iValidIndex);

    const yl::string & GetValidNote();
    void SetValidNote(const yl::string & strValidNote);

    int GetMsgBoxType();
    void SetMsgBoxType(int nMsgBoxType);

    int GetShowtime();
    void SetShowtime(int nShowTime);

private:
    int                 m_nValidIndex;
    yl::string          m_strValidNote;
    int                 m_nMsgBoxType;
    int                 m_nTime;
};

typedef     CSettingUIDataValidInfo    *    CSettingUIDataValidInfoPtr;

#endif // __SETTINGUI_DATA_VALID_INFO_H__


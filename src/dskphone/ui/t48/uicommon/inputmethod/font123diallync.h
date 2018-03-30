#pragma once
#include "fontpreeditbase.h"
#include "datastruct.h"


class CFont_123_Dail_Lync : public CFontPreeditBase
{
public:
    CFont_123_Dail_Lync(void);
    ~CFont_123_Dail_Lync(void);
public:
    //加载文件数据
    virtual void LoadIMEFile();
    void InitData();
    QString DealKeyPlus();

    bool DealKeyRelease(QObject* pObject, int iKeyCode);

    //根据输入Keycode获取输出
    virtual QString GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus);
};

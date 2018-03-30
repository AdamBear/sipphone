#pragma once

#include "fontbase.h"
#include "datastruct.h"

class CFontCustom : public CFontBase
{

public:
    CFontCustom(void);
    virtual ~CFontCustom(void);

public:
    virtual QString GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus);

    //处理外部传进来的更改输入库
    virtual QString GetFirstChar(int iKeyCode);

    virtual void InitData();

private:
    QString GetFontFromIme(int iKeyCode);

};


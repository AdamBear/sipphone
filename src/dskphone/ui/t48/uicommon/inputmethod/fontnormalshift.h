#pragma once
#include "fontbase.h"
#include "datastruct.h"

class CFontNormal_Shift : public CFontBase
{
public:
    CFontNormal_Shift(void);
    ~CFontNormal_Shift(void);
public:
    //根据输入Keycode获取输出
    virtual QString GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus);

public:
    //处理外部传进来的更改输入库
    QString GetFirstChar(int iKeyCode);

    void InitData();
private:
    QString GetFontABC(int iKeyCode);
};

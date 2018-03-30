#pragma once
#include "fontpreeditbase.h"

class CFontSymbol : public CFontPreeditBase
{
public:
    CFontSymbol(void);
    ~CFontSymbol(void);
public:
    //加载文件数据
    virtual void LoadIMEFile();

    void InitData();
};

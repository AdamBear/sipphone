#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"

class CFont2ab_ : public CFontPreeditBase
{
public:
    CFont2ab_(void);
    ~CFont2ab_(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();
    void InitData();
};




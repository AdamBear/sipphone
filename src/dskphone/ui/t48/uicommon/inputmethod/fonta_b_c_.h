#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"


class CFonta_b_c_ : public CFontPreeditBase
{
public:
    CFonta_b_c_(void);
    virtual ~CFonta_b_c_(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();

    void InitData();
};



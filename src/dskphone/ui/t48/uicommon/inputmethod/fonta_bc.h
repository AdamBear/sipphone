#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"

class CFonta_bc : public CFontPreeditBase
{
public:
    CFonta_bc(void);
    virtual ~CFonta_bc(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();

    void InitData();

};





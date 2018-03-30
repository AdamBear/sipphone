#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"


class CFontabc: public CFontPreeditBase
{
public:
    CFontabc(void);
    virtual ~CFontabc(void);


public:
    //加载文件数据
    virtual void LoadIMEFile();

    void InitData();

};



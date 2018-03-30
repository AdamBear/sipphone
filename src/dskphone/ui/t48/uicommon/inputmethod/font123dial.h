#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"


class CFont_123_Dial: public CFontPreeditBase
{
public:
    CFont_123_Dial(void);
    virtual ~CFont_123_Dial(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();
    void InitData();

};


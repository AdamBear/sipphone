#pragma once

#include "fontpreeditbase.h"
#include "datastruct.h"

class CFont_123_IP: public CFontPreeditBase
{
public:
    CFont_123_IP(void);
    virtual ~CFont_123_IP(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();
    void InitData();
};

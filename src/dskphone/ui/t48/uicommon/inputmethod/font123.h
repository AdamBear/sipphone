#ifndef FONT_123_H
#define FONT_123_H

#include "fontpreeditbase.h"
#include "datastruct.h"


class CFont123: public CFontPreeditBase
{
public:
    CFont123(void);
    virtual ~CFont123(void);

public:
    //加载文件数据
    virtual void LoadIMEFile();
    void InitData();

};


#endif

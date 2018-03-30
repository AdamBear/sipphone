#ifndef _H_COLORTEXT_DELEGATE_T9
#define _H_COLORTEXT_DELEGATE_T9

#include "colortextdelegatebase.h"

class CColorTextDelegateT9 : public CColorTextDelegateBase
{
public:
    CColorTextDelegateT9();
    ~CColorTextDelegateT9();

private:
    //计算出搜索字符串的位置
    virtual int SearchIndex(const QString & strTotalText) const;
};

#endif




#ifndef __NORMAL_TITLE_H__
#define __NORMAL_TITLE_H__

#include "baseui/include/titlebase.h"


class CNormalTitle : public CTitleBase
{
public:
    CNormalTitle();
    virtual ~CNormalTitle();

public:
    virtual void SetTitleView(xView * pTitle);

    bool SetPageInfo(const yl::string & strPageInfo);

protected:
    xTextView * m_pPageInfo;
};

#endif

#ifndef __TITLE_BASE_H__
#define __TITLE_BASE_H__

#include <ETLLib/ETLLib.hpp>
#include <ylstl/ylstring.h>


class xView;
class xTextView;

class CTitleBase
{
public:
    CTitleBase();
    virtual ~CTitleBase();

public:
    xView * GetTitleView() const;
    virtual void SetTitleView(xView * pTitle);

    bool SetTitle(const yl::string & strTitle);

    bool Show();
    bool Hide();
    bool IsVisible() const;

protected:
    xView * m_pTitle;
    xTextView * m_pText;
};

#endif

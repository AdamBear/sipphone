#ifndef __T2X_WIDGET_H__
#define __T2X_WIDGET_H__

#include "xwindow/xSoftkeyBar.h"


class CIconHotKey : public xSlotHotKey
{
public:
    CIconHotKey();
    virtual ~CIconHotKey();

public:
    virtual void UpdateHotKeyBtn(xButton * pBtn);

    void SetIcon(const yl::string & strIcon)
    {
        m_strIcon = strIcon;
    }
    const yl::string & GetIcon() const
    {
        return m_strIcon;
    }

protected:
    yl::string m_strIcon;
};

#endif

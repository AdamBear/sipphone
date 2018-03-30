#include "xWindowManager.h"

class xKeyHook
{
public:
    xKeyHook(int iProvide = INT_MIN)
        : m_iProvide(iProvide)
    {
        getCurrentStation().AddKeyHook(this);
    };
    virtual ~xKeyHook()
    {
        getCurrentStation().DelKeyHook(this);
    };

    virtual bool processKeyHookEvent(xView* pView, int key, bool bPressed, bool bLongPress) = 0;

    bool operator < (const xKeyHook& rhs) const
    {
        return m_iProvide < rhs.m_iProvide;
    };

private:
    int m_iProvide;
};

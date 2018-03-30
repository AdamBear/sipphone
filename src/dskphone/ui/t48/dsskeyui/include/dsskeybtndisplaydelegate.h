#ifndef _DSSKEYBTN_DISPLAY_DELEGATE_H_
#define _DSSKEYBTN_DISPLAY_DELEGATE_H_

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include <QWidget>


class QStylePainter;

////////////////Dsskey按键自定义显示代理的基类/////////////////////////
class DsskeyBtnDisplayDelegete
{
public:
    explicit DsskeyBtnDisplayDelegete();
    virtual ~ DsskeyBtnDisplayDelegete();

    virtual void PaintDsskeyBtn(QStylePainter & stylePainter, const DsskeyDisplayData & sData,
                                const QWidget * pWidget);
protected:

private:

};
///////////////////////////////////////////////////////////////////

#endif //_DSSKEYBTN_DISPLAY_DELEGATE_H_

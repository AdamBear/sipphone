#ifndef __SCROLL_CTRL_H__
#define __SCROLL_CTRL_H__

#include "framelistdefine.h"

#ifdef FL_SUPPORT_SCROLL_PAGE
#include "pagectrl.h"
typedef CPageCtrl CScrollCtrl;
#else
#ifdef FL_SUPPORT_SCROLL_SLIDE
#include "listslider.h"
typedef CListSlider CScrollCtrl;
#endif
#endif

#endif

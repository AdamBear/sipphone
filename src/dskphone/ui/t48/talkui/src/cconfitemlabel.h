#ifndef _CONF_ITEM_LABEL_H_
#define _CONF_ITEM_LABEL_H_


#include <QtGui>

#include <ylstring.h>
#include "imagemanager/modimagemanager.h"

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#else
#include "T3XStyleUI/T3XPicPath.h"
#endif

const int CONFITEM_HD_ICON_WIDTH = 30;
const int CONFITEM_HELD_ICON_WIDTH = 30;
const int CONFITEM_HOLD_ICON_WIDTH = 30;

class CConfItemLabel : public QLabel
{
    Q_OBJECT

public:
    CConfItemLabel(QWidget * parent /* = 0 */);

    ~CConfItemLabel();

public:
    //设置内容
    void SetContent(const yl::string & strHDIcon, const yl::string & strHoldIcon = "",
                    const yl::string & strHeldIcon = "");

    //设置HD图标
    void SetHDIcon(const yl::string & strHDIcon = "");

    //设置Hold图标
    void SetHoldIcon(const yl::string & strHoldIcon = "");

    //设置Far Mute图标
    void SetFarMuteIcon(const yl::string & strMuteIcon = "");

    void ClearContent();

    bool IsShowHDIcon() const;

    bool IsShowHoldIcon() const;

    bool IsShowFarMuteIcon() const;

private:
    QLabel     *    m_pHoldLabel;     //Hold图标

    QLabel     *    m_pHeldLabel;     // Held图标

    QLabel     *    m_pHDLabel;       // 使用G.722的通话线路，显示HD文字
};
#endif   //_CONF_ITEM_LABEL_H_

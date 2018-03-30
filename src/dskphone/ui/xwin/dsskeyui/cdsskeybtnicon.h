#ifndef __DSSKEYBTN_ICON_H__
#define __DSSKEYBTN_ICON_H__
#include "cdsskeybutton.h"

class CDsskeyBtnIcon : public CDsskeyButton
{
public:
    CDsskeyBtnIcon();
    ~CDsskeyBtnIcon();
public:
public:
    virtual void loadChildrenElements(xml_node & node);
    //更新显示状态
    virtual void UpdateDisplayStatus(const DsskeyDisplayData& sData);

    void SetLinePixmap(const yl::string & strLinePix);
    void SetIcon(const yl::string & strIcon);
    void SetStatusIcon(const yl::string & strStatusIcon);

    // 设置DsskeyIcon和文字为空 并且隐藏
    void SetIconAndTextEmptyHide();
    // 设置DsskeyIcon和文字为空 但是显示
    void SetIconAndTextEmptyShow();

    void ShowHideIcon();

    virtual void ChangeDisplay();
protected:
    xImageView * m_pIcon;
    xImageView * m_pLine;
    xImageView * m_pIconStatus;
    xFrameLayout * m_pFrmIcon;

    yl::string m_strCurIcon;
    yl::string m_strDownCurIcon;
    yl::string m_strPresenceIcon;
};

#endif

#ifndef _DLG_VIEW_MESSAGE_H_
#define _DLG_VIEW_MESSAGE_H_

#include "dlgsettingui.h"
#include "settinguiwizardtitle.h"

class xScrollBar;
class CDlgViewMessage : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgViewMessage();
    virtual ~CDlgViewMessage();

public:
    virtual void Init();
    virtual void loadChildrenElements(xml_node & node);
    //刷新Title
    virtual void RefreshTitle();
    virtual void RefreshTitle(const yl::string & strTitle);
    virtual void RefreshUI(bool bReLoadData = false);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);

protected:
    CSettingUIWizardlTitle      m_GuideTitle;
    xScrollBar         *        m_pScroll;
};

#endif //_DLG_VIEW_MESSAGE_H_

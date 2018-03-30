#ifndef __INPUT_BAR_H__
#define __INPUT_BAR_H__

#include "xwindow/xWindowManager.h"
#include "uimanager/basedialog.h"
#include "inputbarcommon.h"

class CInputBar : public xLinearLayout
{

public:
    CInputBar();
    ~CInputBar();

public:
    void Init();
    virtual void loadAttributes(xml_node& node);
    virtual void loadChildrenElements(xml_node& node);
    void SetBarType(INPUTBAR_TYPE eType);

    INPUTBAR_TYPE GetBarType()
    {
        return m_eBarType;
    }
    void SetBarInfo(int nIndex, int nCount, yl::wstring & strSequence);
    void SetInputInfo(int nIndex, const VEC_STRING& vecAllIme);
    int GetSoftbarHeight()
    {
        return m_nSoftbarHeight;
    }
protected:
    typedef YLVector<xTextView*> TextItemVector;
    bool m_bShowFrame;
    INPUTBAR_TYPE m_eBarType;

    TextItemVector m_vecTextItem;
    int m_nCharItemWidth;
    int m_nImeItemWidth;
    int m_nSoftbarHeight;
    xColor m_colorFocusBG;
    xColor m_colorNormalBG;
    xColor m_colorFocusText;
    xColor m_colorNormalText;
    xColor m_colorFrame;
};

#endif

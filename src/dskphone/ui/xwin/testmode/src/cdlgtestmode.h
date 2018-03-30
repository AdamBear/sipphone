#ifndef _TEST_DIALOG_H
#define _TEST_DIALOG_H

#include "uimanager/basedialog.h"
#include "xwindow/xWindow.h"
#include "ylvector.h"
#include "test_include.h"

class CAutoVolumeBar;
class CTestPhone;

struct stAreaColor
{
    chRect      rcArea;
    xColor      color;
};

typedef YLVector<stAreaColor> VEC_AREA_COLOR;

class CDlgTestMode : public CBaseDialog, public xFrameLayout
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgTestMode();
    ~CDlgTestMode();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void loadChildrenElements(xml_node & node);

    void InitTestPhone(int iTestType);

    virtual bool IsShowSoftkey() {
        return false;
    }

    virtual bool IsShowVolumeBar() {
        return false;
    }

    virtual bool IsRejectReturnToIdle() {
        return true;
    }

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    void SetDisplayText(const yl::string & strContent, int nTextType, int nFontSize);

    void SetTextColor(int nTextType, const xColor & color);

    void FillFullByPicture(const yl::string& strPic);

    void FillRectByColor(int nAreaType, const xColor & color);

#ifdef _CP920
    void ShowErrorKeys(bool bShow);
#endif

    void SetTextFontSize(xTextView * pTextView, int nFontSize);

    virtual bool onPaintBackground(xPainter & p);

    CTestPhone * GetTestPhone() {
        return m_pTestPhone;
    }

protected:
    chRect GetRectByAreaType(int nAreaType);

    xTextView* GetTextView(YLVector<xTextView *>& vecNonActive, int nTextType);

#ifdef _CP920
    bool GetBtnIdByKey(int nRawKey, yl::string & strId);
#endif

private:
    xTextView   *   m_pTextTitle;
    xTextView   *   m_pTextNormal;
    xTextView   *   m_pTextCenter;
    xTextView   *   m_pTextSecondHalf;
    xTextView   *   m_pTextFullScreen;
#ifdef _CP920
    xLinearLayout * m_pBoxKeyResult;
#endif
    CTestPhone   *  m_pTestPhone;
    VEC_AREA_COLOR  m_vecAreaColor;

    xPixmap pixmap;
};

#endif // _TEST_DIALOG_H

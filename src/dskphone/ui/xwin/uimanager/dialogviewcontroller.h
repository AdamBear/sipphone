#ifndef __XVIEW_CONTROLLER__
#define __XVIEW_CONTROLLER__

#include "xwindow/xWindow.h"
#include "uimanager_header.h"
#include "uimanager_common.h"
#include "dsskeyui/dsskeyuicommondefine.h"
#include "baseui/include/baseuidefine.h"
////////////////////////////softkeyBar数据类
class CSoftKeyBarData
{
public:
    CSoftKeyBarData()
    {
        InitData();
    }

    ~CSoftKeyBarData() {}

    // 初始化Softkey数据
    void InitData();

    // 重载赋值操作符
    CSoftKeyBarData & operator=(const CSoftKeyBarData & other);

    CSoftKeyBarData(const CSoftKeyBarData & other);

public:
    int         m_iType;
    yl::string  m_strSoftkeyTitle;
    yl::string  m_strBtnPic;
};

// 重定义类型
typedef YLVector<CSoftKeyBarData> CArraySoftKeyBarData;
///////////////////////////////////////////

// 所有小窗口的基类，所有模块的小窗口必须从该类继承
// 以实现统一的Init Uninit接口
class CDialogViewController : public xSlotHandler
{

public:
    explicit CDialogViewController(xView * pDialogView);
    virtual ~CDialogViewController();

    static void SetDefaultShowSoftkey(bool bShowSoftkey);

    ////////////////////xView///////////////////////
    virtual void CreateWindowObject(bool bWaitSetActive = false);

    virtual void DialogShow(int nShowCmd = xView::SHOW_CMD_DEFAULT);
    virtual void DialogRaise();
    virtual void DialogUpdate();
    virtual void DialogSetActive();
    virtual void DialogHide();
    virtual bool DialogClose();

    void BringWindowObjectTop();
    void PopWindowObject();

    /////////////////////////////////////////////////
    void SetProcessed(bool bProcessed)
    {
        m_bProcessed = bProcessed;
    }
    bool GetProcessed()
    {
        return m_bProcessed;
    }

    ////////////////////音量条///////////////////////
    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix);
    ///////////////////////////////////////////////////

    ////////////////背景///////////////////////////
    //设置整个大背景的图片（不是界面自身的背景）
    virtual xBrush GetBackGroundBrush();
    // 是否重叠属性，默认为true;
    virtual bool GetOverlap()
    {
        return true;
    }
    ///////////////////////////////////////////////

    /////////////////////Dsskey////////////////////
    //
    virtual bool IsShowDsskey(bool & bExpend)
    {
        return m_bDefaultShowDsskey;
    }

    void SetDefaultShowDsskey(bool bShowDsskey);
    void UpdateDsskey(bool bShow);
    virtual DSSKEY_DISPLAY_MODE GetDsskeyDisplayMode()
    {
        return DDM_HIDE;
    }

    /////////////////////Softkey/////////////////////
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);
#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
    virtual yl::string GetSoftkeyBG();
#endif

    //设置Softkey数据
    virtual void SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData);

    //设置Softkey数据到Softkeybar上
    virtual void SetupSoftKey(CArraySoftKeyBarData & objSoftkeyData);

    // 是否显示Softkey，默认为不显示
    virtual bool IsShowSoftkey()
    {
        return s_bDefaultShowSoftKey;
    }

    void UpdateSoftkey(bool bShow);

    xView * GetDialogView();
    ///////////////////////////////////////////////////
    virtual bool OnEdkSoftkeyAction(int iType)
    {
        return OnSoftkeyAction(iType, false);
    }

protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress)
    {
        return false;
    }

    virtual bool OnDsskeyAction(int iType, bool bLongPress)
    {
        return false;
    }

private:
    void slotOnSoftkeypress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                            LPVOID pData);
    void slotOnSoftKeyLonngPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                 LPVOID pData);
    //////////////////////////////////////////////////

protected:
    xView         *         m_pDialogView;
    dataWindowObject    *   m_pDialogWindowObject;

    // softkey
    CArraySoftKeyBarData    m_SoftkeyData;

    static bool             s_bDefaultShowSoftKey;

    // dsskey
    bool m_bDefaultShowDsskey;
    DSSKEY_DISPLAY_MODE m_eDsskeyDisplayMode;         // 显示模式，是长按键还是短按键
private:
    bool                        m_bProcessed;
};

#endif // __XVIEW_CONTROLLER__

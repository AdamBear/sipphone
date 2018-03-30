#ifndef __ELEMENT_HAVE_UI_BASE_H__
#define __ELEMENT_HAVE_UI_BASE_H__

#ifdef IF_FEATURE_XMLBROWSER

// 有界面的Element基类
class CElementHaveUIBase : public CElementBase
{
    friend class CXMLBrowserManager;
public:
    CElementHaveUIBase();
    virtual ~CElementHaveUIBase() {}

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy() = 0;
    // 是否播放提示音
    virtual bool IsNeedPlayBeep() = 0;

    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);
    // 根据各Elment的情况更新URI
    virtual void UpdateURI(yl::string & strURI, int iFocusedItem) {}
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate) = 0;
#if IF_BUG_32431
    virtual bool UpdateData(XMLParseData * pParseData, bool & bUpdate)
    {
        return false;
    }
#endif

protected:
    // URI后面添加selection
    void AddSelectionToURI(yl::string & strURI, const yl::string & strSelection);
    // 判断并处理合法的URL
    void ProcessLegalURL(yl::string & strURL, bool bDestroyOnExit/* = false*/);
    // 处理URL的跳转
    bool GotoURL(yl::string & strURI, bool bDestroyOnExit /*= false*/);

private:
    // Softkey的处理步骤1
    bool BeginProcessSoftkey(XB_ACTION_TYPE eAction, yl::string strURI,
                             int iFocusedItem, bool & bExitWnd, bool & bUpdate);
    // Softkey的处理步骤2
    bool Step2ProcessSoftkey(yl::string strURI, int iFocusedItem, bool & bExitWnd,  bool & bUpdate);

protected:
    // 当前焦点Item
    int                  m_iCurrentFocus;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __ELEMENT_HAVE_UI_BASE_H__

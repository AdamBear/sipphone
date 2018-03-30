#ifndef __ELEMENT_TEXT_SCREEN_H__
#define __ELEMENT_TEXT_SCREEN_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element TextScreen类
class CElementTextScreen : public CElementHaveUIBase
{
public:
    CElementTextScreen();
    virtual ~CElementTextScreen() {}

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);

private:
    IMPLEMENT_GETXDATA(XBTextScreenData);

private:
    // 保存属性
    TextScreenAttr    m_dataAttr;
    // text的内容
    yl::string            m_strText;
    // 传给UI的数据
    XBTextScreenData  m_dataToUI;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_TEXT_SCREEN_H__


#ifndef __ELEMENT_FORMATTED_TEXT_SCREEN_H__
#define __ELEMENT_FORMATTED_TEXT_SCREEN_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element FormattedTextScreen类
class CElementFormattedText : public CElementHaveUIBase
{
public:
    CElementFormattedText();
    virtual ~CElementFormattedText();

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
    IMPLEMENT_GETXDATA(XBFormattedTextData);

};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_FORMATTED_TEXT_SCREEN_H__

#ifndef __ELEMENT_INPUT_SCREEN_H__
#define __ELEMENT_INPUT_SCREEN_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element InputScreen类
class CElementInputScreen : public CElementHaveUIBase
{
public:
    typedef YLHashMap<int, yl::string> SubmitDataMap;

    CElementInputScreen();
    virtual ~CElementInputScreen();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // 根据各Elment的情况更新URI
    virtual void UpdateURI(yl::string & strURI, int iFocusedItem);
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);

private:
    // 获得当前焦点所在输入框的类型
    INPUT_TYPE GetCurrentType();
    // 该功能在UI那边处理
    // Submit的处理
    bool Submit(SubmitDataMap * pData);
    // 根据index获得参数名Param
    yl::string GetParamByIndex(int iIndex);
    // 根据当前焦点获得selection
    yl::string GetSelectionByFocus(int iIndex);

private:
    IMPLEMENT_GETXDATA(XBInputData);

private:
    // 保存属性
    InputAttr         m_dataAttr;
    // 保存ItemList
    List_Input_Field  m_ListInput;
    // 用户输入完成后，将输入内容发送到该URL
    yl::string            m_strURL;
    // 传给UI的数据
    XBInputData       m_dataToUI;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_INPUT_SCREEN_H__


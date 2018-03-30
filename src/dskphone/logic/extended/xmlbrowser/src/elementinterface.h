#ifdef IF_FEATURE_GENBAND

#ifdef IF_FEATURE_XMLBROWSER

class CElementInterface :   public CElementBase
{
public:
    CElementInterface(void);
    ~CElementInterface(void);

    // 创建之后的处理
    virtual bool AfterCreate();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // 销毁之前的处理
    virtual void BeforeDestroy();

    //发送组装完的请求消息
    void PostInterfaceMessage();

    //提示处理
    void ResultTip();

private:
    int m_iErrorCode;
    IMPLEMENT_GETXDATA(InterfaceData)
};


#endif  // IF_FEATURE_XMLBROWSER

#endif // #ifdef IF_FEATURE_GENBAND



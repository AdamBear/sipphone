#ifndef __ELEMENT_EXECUTE_H__
#define __ELEMENT_EXECUTE_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element Execute类
class CElementExecute : public CElementBase
{
public:
    CElementExecute();
    virtual ~CElementExecute();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();

private:
    IMPLEMENT_GETXDATA(XBExecuteData);

private:
    // 保存属性
    //ExecuteAttr    m_dataAttr;
    // 保存ItemList
    //List_Execute_Item  m_ListItem;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_EXECUTE_H__

#ifndef __ELEMENT_DIRECTORY_H__
#define __ELEMENT_DIRECTORY_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element Directory类
class CElementDirectory : public CElementHaveUIBase
{
public:
    CElementDirectory();
    virtual ~CElementDirectory();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);
    // 获取指定控件的号码,无号码则返回空
    virtual yl::string GetItemNumberByIndex(UINT uIndex);
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);

#if IF_FEATURE_XMLDIRECTORY
    bool Submit(char * pData);

    virtual bool UpdateData(XMLParseData * pParseData, bool & bUpdate);
#endif

private:
    // 获取焦点Item
    DirMenuItem * GetItem(int iFocusedItem);

    IMPLEMENT_GETXDATA(XBDirectoryData)

private:
    // 保存属性
    // DirectoryAttr   m_dataAttr;
    // 保存ItemList
    // List_Dir_Menu_Item  m_ListItem;
    // 传给UI的数据
    XBDirectoryData  m_dataToUI;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_DIRECTORY_H__

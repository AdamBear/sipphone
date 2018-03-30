#ifndef MSGCOMMONDELEGATE_H
#define MSGCOMMONDELEGATE_H

#include "uikernel/iframelistdelegate.h"
#include "uikernel/ilistitemaction.h"
#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include "datadelegate.h"
#include <QObject>
#include <QLineEdit>

typedef std::vector<CSettingItem *> SettingItemList;

namespace
{
//#define LOAD_ALL_DATA_FROM_LOGIC
}

class MsgCommonDelegate : public QObject, public IFrameListDelegate, public IListItemAction
//CFrameListDelegate
{
    Q_OBJECT

public:
    MsgCommonDelegate();
    MsgCommonDelegate(CFrameList * pList);
    ~MsgCommonDelegate();

    /***************************一般接口*****************************/
    //设置列表数据
    void SetContent(QWPVector * pVecWP);

    // 刷新控件的编辑属性
    void UpdateItemStatus(bool bEanbled);

    //刷新列表，重新加载数据
    void UpdateList();

    //绑定一个列表
    void BindFramelist(CFrameList * pList);
    void UnBindFramelist();

    void SetItemParent(QWidget * parent);

    //保存列表状态
    void SaveListStatus();
    //重载列表状态
    void RestoreListStatus();
    //保存列表状态并解绑
    void SaveListStatusAndUnBindFramelist();

    //设置列表基本属性
    void InitSettingListStyle();

    //设置焦点到指定的输入框，以及附加操作（选中行）
    void SetErrorFocus(QLineEdit * pLineEdit);

    void SelectIndex(int nIndex = 0, bool bEnter = false);//马上选中指定行

    //显示出来的时候的操作
    void RefreshFocus();

    //清空列表，bDeleteContenWgt：是否同时删除列表item上面的控件
    void ClearContent(bool bDeleteContenWgt, bool bDelWgtOnlg = false);
    void DelWgtOnlg();

    void DelItemByDataIdx(int iDataIdx, bool bUpdate = false);

    //预设选中行，不会立即生效，列表刷新时将会选中该行
    void SetCurrentIndex(int iIndex);

    //处理按键事件
    bool ProcessKeyPress(int keyCode);

    QString GetFocusItemAction();

    int GetFocusItemDataIndex();

    int GetCurrentRow();//选中了当前页的第几行

    int GetCurrentIndex();

    void SelectItemByContentWgt(QWidget * pWidget);

    // 获取存取Item vec
    //SettingItemList& GetSetItemList() { return m_vecItem; }

    void SetConfigedIndex(int iIndex);

    bool IsEmpty();
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    void SetDeleteItem(bool bDel = false)
    {
        m_bDel = bDel;
    }
    void SetViewUnMsg(bool bView = false)
    {
        m_bView = bView;
    }
#endif

public:
    /******************************IListItemAction************************/
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);
    /***************************IFrameListDelegate************************/
    virtual bool OnLastItemNext()
    {
        return false;
    }
    virtual bool OnFirstItemPrevious()
    {
        return false;
    }
    virtual void OnCleanContent(CFrameListBase * pFrameList) {}
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    virtual void OnFrameListDetach(CFrameListBase * pFrameList) {}
    virtual void OnFrameListAttach(CFrameListBase * pFrameList) {}
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    void BindDataDelegate(DataDelegate * pDatad);
    void UnBindDataDelegate();

public:
    //向后翻页并选中第一项
    void GoNextPage();
    //向前翻页并选中第一项
    void GoPrePage();

private:
    CSettingItem * FindItemByContentWgt(QWidget *
                                        pWidget); //根据item的内容控件找到对应的item
    void HideAllItem();

    bool CheckCanReused(int iStDataIdx);

protected:
    void ResetConfigedItem(int iOldIndex, int iNewIndex);

signals:
    void listItemAction(int eActionType, QString strItemAction);

private:
    DataDelegate  * m_pDataDelegate;
    CFrameList   *  m_pFramelist;
    QWPVector       m_vecData;
    QWidget    *    m_pItemParent;
    SettingItemList m_vecItem;
    int             m_iCurrentIndex; //当前预设的选中项
    bool            m_bIsEditPage; //是否为编辑型页面的列表
    int             m_iConfigedIndex;//当前配置中的项，使用与单选列表
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    bool            m_bDel;
    bool            m_bView;
#endif
};

#endif

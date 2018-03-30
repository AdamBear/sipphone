/*******************************************************************************************/
/*****为了在不同机型上面可以使用不同的控件，这里将对列表的一些操作初步封装出来****/
/*******************************************************************************************/

#ifndef _SUBPAGE_DELEGATE_H_
#define _SUBPAGE_DELEGATE_H_
#include "uikernel/iframelistdelegate.h"
#include "uikernel/ilistitemaction.h"
#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include <QObject>
#include <QLineEdit>

typedef std::vector<CSettingItem *> SettingItemList;

class SubpageListDeleagate : public QObject, public IFrameListDelegate, public IListItemAction
{
    Q_OBJECT

public:
    SubpageListDeleagate();
    ~SubpageListDeleagate();

    /***************************一般接口*****************************/
    //设置列表数据
    virtual void SetContent(QWPVector * pVecWP);

    // 刷新控件状态 : 编辑属性
    virtual void UpdateItemStatus(bool bEanbled);

    //刷新列表，重新加载数据
    virtual void UpdateList();

    virtual bool ProcessMouse(QObject* pObj);
    int GetEditItemCount();
    bool IsEditItemByIndex(int nIndex);
    bool IsEditItemByDataIndex(int nIndex);
    void AdjustFrameList(bool bReset = false);
    bool IsListReady();

    bool IsLastEditItem();
    int GetLastEditItemIndex();
    virtual void SetKeyboardEnterType();
    //绑定一个列表
    void BindFramelist(CFrameList * pList);

    void UnBindFramelist();

    void SetItemParent(QWidget * parent);

    //设置列表基本属性
    void InitSettingListStyle();

    //设置焦点到指定的输入框，以及附加操作（选中行）
    void SetErrorFocus(QLineEdit * pLineEdit);

    void SelectIndex(int nIndex = 0, bool bEnter = false);//马上选中指定行

    //显示出来的时候的操作
    void RefreshFocus();

    //清空列表，bDeleteContenWgt：是否同时删除列表item上面的控件
    void ClearContent(bool bDeleteContenWgt);

    //预设选中行，不会立即生效，列表刷新时将会选中该行
    void SetCurrentIndex(int iIndex);

    //处理按键事件
    bool ProcessKeyPress(int keyCode);

    QString GetCurrentItemAction();

    int GetFocusItemDataIndex();

    int GetCurrentRow();//选中了当前页的第几行

    int GetCurrentIndex();//获取选中的数据项的dataindex

    // 获取存取Item vec
    SettingItemList & GetSetItemList()
    {
        return m_vecItem;
    }

    void SetConfigedIndex(int iIndex);
    int GetConfigedIndex();

    void SetNoClickBG();
    void SetItemClickBG(const char * pszTopBG, const char * pszMidBG, const char * pszBottomBG);

    bool IsEditPage()
    {
        return m_bIsEditPage;
    }

    void SelectIndexByAction(QString strAction);
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

    virtual int GetMaxPagePageItemCount();

public:
    //向后翻页并选中第一项
    void GoNextPage();
    //向前翻页并选中第一项
    void GoPrePage();

    CSettingItem * FindItemByContentWgt(QWidget * pWidget); //根据item的内容控件找到对应的item

protected:

    void HideAllItem();

protected:
    void ResetConfigedItem(int iOldIndex, int iNewIndex);
public:
    bool FocusPreNextEditableWidget(bool bNext);

signals:
    void listItemAction(int eActionType, QString strItemAction);

protected:
    CFrameList * m_pFramelist;
    QWidget  *  m_pItemParent;
    SettingItemList m_vecItem;
    int m_iCurrentIndex; //当前预设的选中项
    bool m_bIsEditPage; //是否为编辑型页面的列表
    int m_iConfigedIndex; //当前配置中的项，使用于单选列表
    yl::string m_strItemClickTopBG;
    yl::string m_strItemClickMidBG;
    yl::string m_strItemClickBottomBG;

    bool m_bIsListReady;                // 显示全键盘时，列表项准备全才显示
    bool m_bKeyChangePage; // 上下方向键切页 全键盘闪烁问题
};



#endif

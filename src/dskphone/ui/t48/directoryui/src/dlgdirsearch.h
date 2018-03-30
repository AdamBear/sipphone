// Search dialog of directory module to search some contact.
// Created by yyj.

#ifndef __DLG_DIR_SEARCH_H__
#define __DLG_DIR_SEARCH_H__

#include "uimanager/basedialog.h"
#include "uikernel/iframelistdelegate.h"
#include "uikernel/ilistitemaction.h"
#include "directorycommon.h"

#if IF_FEATURE_GBBOOK
#include "baseui/comboboxlistitem.h"
#endif

class QFocusEvent;
class CFrameList;
class CListItem;
class CBroadSoftSearchLayout;
class CDirectoryListDelegate;
#if IF_FEATURE_GBBOOK
class CGenbandSearchLayout;
#endif
class CDlgDirSearch : public CBaseDialog, public IFrameListDelegate, public IListItemAction
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirSearch();
    }

public:
    CDlgDirSearch(QWidget * parent = NULL);
    virtual ~CDlgDirSearch(void);

// IFrameListDelegate
public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext();
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious();

    virtual void OnKeyboardShow();

    virtual void OnKeyboardHide();

    // 调整framelist位置，确保输入框显示
    void AdjustFrameList(bool bReset);

    bool IsLastEditItem(CListItemPtr pListItem);
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList);
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    // 是否已经绑定特定的列表框
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    // 数据代理解除绑定
    virtual void OnFrameListDetach(CFrameListBase * pFrameList);
    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);
    virtual CListItem* GetSearchItem(int nSearchType);
public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void UpdateWnd();

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 焦点改变.
    virtual void OnFocusChanged(bool bFocused);

    void InitPreciseSearch(int nDirType);

    void InitSearchType();
    bool CheckSearchType();

    CFrameList * GetContactFrameList() const;

    // 设置搜索类型
    void SetSearchType(int nSearchType);
    int GetSearchType() const;

    // 设置联系人类型
    void SetContactType(int nContactType);
    int GetContactType() const;

    void SetInitSearchKey(int nKeyCode);

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

public Q_SLOTS:
    void OnEditKeyOkClick();
    void OnKeyWordChanged(const QString & strText);
    void OnTimerOut();
    void OnSearchTypeChanged(int iIndex);

    void OnSearchEditFocus(bool bFocus);
    void OnComboBoxSelect(int iIndex);

protected:
    virtual bool OnAction(const QString & strAction);

    // 焦点变化刷新SoftKey
    void OnCtrlFocusChanged(QFocusEvent * pEvent);

    // 点击IME
    void OnBtnIMEClick();
    // 点击删除
    void OnBtnDeleteClick();
    void OnBtnSearch();
    void BeginSearch();

    void SearchContact(const QString & strKey);
    void StopTimer();

    bool OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnRightPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnUpPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnDownPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnMousePressEventFilter(QObject * pObj, QEvent * pEvent);
    CListItem * GetUpDownFilterFocusItem(QObject * pEventObj, bool & bComboBoxFilter);
    bool SwitchItemFocus(CListItem * pListItem, bool bPreItem);

    void CheckSearchType(QWidget * pLineEdit, int & nSearchType);

    void ReleaseLayout();
    void ResetUIHelperLayout();

    void InitBsftSearchLayout();
#if IF_FEATURE_GBBOOK
    void InitGBSearchLayout();
#endif

protected:
    CFrameList * m_pSearchList;
    CFrameList * m_pContactList;
    CBroadSoftSearchLayout * m_pBsftSearchLayout;

#if IF_FEATURE_GBBOOK
    CGenbandSearchLayout * m_pGBSearchLayout;

    qComboBoxEx * m_pSearchComboBox;
#endif

    // 普通搜索的联系人类型
    int m_nContactType;
    // 搜索类型
    int m_nSearchType;
    // 搜索类型数量
    int m_nSearchCount;
    // 具体的搜索类型数组
    int m_arrSearchType[DIRECTORY_SEARCH_TYPE_MAX];

    // 记录界面刷新时是否界面返回
    bool m_bReturn;

    // 用于记录返回搜索界面时是否显示全键盘
    bool m_bShowKeyBoard;

    QTimer m_timer;
};

#endif

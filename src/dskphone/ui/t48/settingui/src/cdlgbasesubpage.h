#ifndef HEAD_DLG_BASE
#define HEAD_DLG_BASE

#include "uimanager/basedialog.h"
#include "uimanager/moduimanager.h"
#include "setting/include/common.h"
#include "uikernel/iframelistdelegate.h"
#include "settingui/src/settingitembase.h"
#include "settingui/src/subpagelistdelegate.h"
#include "settingui/src/settinguimisc.h"
#include <vector>

class QLineEdit;
class CFrameList;

// 输入框数据错误信息
struct InputDataErrorInfo
{
    QString     strErrorContent;
    QLineEdit * pLineEdit;

    InputDataErrorInfo()
    {
        pLineEdit = NULL;
    }
};

const int MSG_ID_SHOW_NOTE_BEFORE_BACK(1000);

// 子页面的一个基类
// 用于覆盖CBaseDialog中的某些函数
class CDlgBaseSubPage : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgBaseSubPage(QWidget * parent = NULL);
    virtual ~CDlgBaseSubPage();

public:
    virtual bool LoadPageData() = 0;    // 读取页面数据
    virtual bool IsStatusChanged() = 0; // 查看页面状态是否改变
    virtual bool SavePageData() = 0;    // 保存页面数据
    virtual void OnBack() {};           // 用于退出界面时的特殊操作
    virtual bool SaveCompleted()
    {
        return true;   // 保存成功后的其他操作
    }
    virtual bool OnAction(const QString & strAction)
    {
        return false;
    }
    virtual void SetDefaultIndex(int nIndex = 0,
                                 bool bEnter = false);//设置界面默认索引,需在updateWnd之前设置

    // 设置子页面的模式
    // 需要根据不同模式进行一些属性特化的子页面需要重新实现
    virtual void SetPageMode(PAGE_MODE ePageMode);

    inline PAGE_MODE GetPageMode() const
    {
        return m_ePageMode;
    }
    inline const QWPVector & GetWPVector() const
    {
        return m_vecWidgetPair;
    }

    //遍历界面控件，如果有输入框处于预编辑状态，则设置预编辑字符有效
    //只能获取界面的直接子控件，对于有非直接子控件的界面，需要重写该函数
    virtual void ReturnAllEdit();

    //卸载界面所有的输入法
    //只遍历界面的直接子控件,对于有非直接子控件的界面,需要重写该函数
    virtual void UninstallAllIME();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    //根据焦点控件重新设置softkey
    void ResetSoftKey(CArraySoftKeyBarData & objWndData);
    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 下面三个虚函数用于屏蔽CBaseDial的函数
    virtual void Init();
    virtual void Uninit();

    virtual void SetData(void * pData = NULL);

    // 设置SubPageAction
    virtual void SetOpenSubPageAction(const yl::string & strSubPageAction);

    virtual bool IsSameSettingDialog(const yl::string & strAction)
    {
        return m_strSubPageAction == strAction;
    }

    inline yl::string GetSubPageAction() const
    {
        return m_strSubPageAction;
    }

    // 判断是否应该关闭滚动条
    virtual bool IsCloseVerticalScrollBar() const;

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData) {}

    // 设置子页面后的操作
    virtual void AfterSetSubPage(void * pData) {}

    //当前选中行改变时候的操作
    virtual void OnCurrentIndexChanged(int iDataIndex);

    //点击(选中时候按OK键)
    virtual void OnListItemClicked(QString strItemAction);

    //窗口显示到界面上时的操作
    virtual void UpdateWnd();

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    //是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent);

    //添加列表数据，由于framelist的特性，在页面显示出来之后调用AddWidget之后务必再调用UpdateWnd()，否则列表不生效
    virtual void AddWidget(QWPVector & vec, bool bAddIndex = true, bool bAddMark = true
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                           , bool bChangeStyleOnPress = false
#endif
                           , bool bChangeStyleOnSelect = false
#endif
                          );

    //重新显示出来的时候的操作
    virtual void OnReShow();

    virtual void AdjustWgtPair(QWPVector & vec, bool bAddIndex, bool bAddMark
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                               , bool bChangeStyleOnPress
#endif
                               , bool bChangeStyleOnSelect
#endif
                              );

    virtual void OnKeyboardShow();

    virtual void OnKeyboardHide();

    bool isListReady();

    //是否被缓存
    bool ShouldBeCache() const;
    void SetBeCache(bool bCache);

    //是否为顶层显示的界面
    bool IsTopPage();
    //设置当前行，不会马上选中，在调用UpdateWnd()后生效
    void SetCurrentIndex(int iIndex);

    int GetCurrentIndex();

    void SetItemClickBackGround(const char * pszTopBG, const char * pszMidBG,
                                const char * pszBottomBG); //设置点击是的背景图片

    void SetNoClickBG();

    // 窗口初始化完成后，需要额外的操作，比如一些阻塞或同步的动作
    virtual void ExtraDialogInitData();

    //判断是否已经存在于列表当中
    bool IsWidgetInList(QWidget * pWidget);

    //获取当前行的item的action
    QString GetCurrentItemAction();

    void SelectIndex(int iDataIndex);//选中行

    // 处理弹出框回调
    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox) {}

    void SetConfigedIndex(int iIndex);

    // 设置界面是否可编辑
    inline void SetSubPageEnabled(bool bEnabled)
    {
        m_bPageEnable = bEnabled;
    }

    // 设置界面是否可编辑
    inline bool GetSubPageEnabled()
    {
        return m_bPageEnable;
    }

    virtual bool PorcessBackBtnClick()
    {
        return false;
    }
    virtual bool PorcessHomeBtnClick()
    {
        return false;
    }

    // 是否有Item
    bool IsHasValidItem();

protected:
    //清除列表数据，但不删除成员控件，以便重新布局，适用于下拉框联动,一般与UpdateWnd成对使用
    virtual void MakeReadyForRelayout();
    //清除所有内容，包括delete成员控件，执行之后，页面成员控件指针必须赋值为NULL，
    //否则出现野指针，慎用
    virtual void ClearAllContent();

    bool CheckEventValid(QObject * pObject, QEvent * pEvent);

    virtual void FilterItemByUserLevel(QWPVector & vec);

    virtual void OnFilterWPVec(); //三级权限过滤之后的特殊处理

signals:
    void currentFocusWgtChange(QRect wgtRect);

    void ProcessPageAction(const QString & strAction);

    // 子页面需要改变softkey的时候向发射该信号，接收者会重新刷新Softkey
    void RefreshSoftkey();

    // 子页面需要改变标题的时候发射该信号，接收者会重新刷新title
    void RefreshTitle();
    //子页面发出信号返回上一界面（保存时由子界面弹出提示框保存成功）
    void BtnBackClick();

    void openpage(const SubMenuData &);

    void openpage(const yl::string strAction);

    //子页面发出信号，用于退出当前界面并新打开一个子页面
    void backandopenpage(const SubMenuData &);

    void replaceCurrentPage(const SubMenuData &);

    void RefreshTimer();
public:
    //设置一个过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    //设置内容错误后，需要将焦点和索引设置到相应的lineEdit行
    void SetErrorFocus(QLineEdit * pLineEdit);

    // 获取焦点控件对应的标志
    int GetFocusItemFlag();
    // 传递由OnListItemAction函数过滤的信号
    virtual void OnExtraAction(int eActionType) {}
public slots:
    void OnListItemAction(int eActionType, QString strItemAction);

private:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    static const int MESSAGE_BOX_EXIST_TIME = 3000; // setting模块弹出messageBox存在的时间
    yl::string m_strSubPageAction;
    PAGE_MODE   m_ePageMode;    // 页面的类型
    bool        m_bCache;       // 是否在settingwnd中缓存
    bool        m_bCloseVerticalScrollBar;
    CFrameList * m_pGeneralFrame;
    SubpageListDeleagate * m_pListDelegate;

    QWPVector   m_vecWidgetPair;//存放页面控件数据

    bool m_bPageEnable; // 界面编辑属性
};

#endif//end HEAD_DLG_BASE

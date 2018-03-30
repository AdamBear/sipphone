#ifndef CUI_REGION_INPUT
#define CUI_REGION_INPUT

#include "cuiregionbase.h"
#include "dlginput.h"

// 前向声明
class AutoResizeLab;
class CScrollPage;
class CUIRegionBase;

class CUIRegionInput : public CUIRegionBase
{
    Q_OBJECT

public:
    CUIRegionInput(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionInput();

    // 根据数据更新界面
    virtual void Update(void * pData);

    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    //返回焦点所在lineEdit序号
    virtual int GetCurrentItem();

    //提供给逻辑层接口，返回map 型的所有lineEdit数据
    virtual bool GetPageData(MapPageData & PageData);

#if !IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    virtual VecSoftkeyParam & GetSoftkey();
#endif

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();

    virtual bool ProcessKey(int nKey);

public slots:
    //焦点改变后的槽函数
    void LineEditFocusChange();

private:
    // 获取页面的默认softkey数据
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

    void SubmitInputInfo();

protected:
    DlgInput     *    m_pInput;      // input控件
    AutoResizeLab  *  m_pLabTitle;   // 显示Title的label
    INPUT_TYPE        m_eInputType;  // 输入界面的类型
    VecSoftkeyParam    m_vecSoftkeyLess;// 过滤输入法按钮后的softkey
    bool m_bIsDefSoftKey;                   //是否为默认softkey

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecGlobalSoftkey;
#else
    std::vector<InputField *>  m_listInputField;
#endif
};

#endif // CUI_REGION_TEXT_SCREEN

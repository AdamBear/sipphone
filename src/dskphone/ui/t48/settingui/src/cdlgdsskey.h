#ifndef CDLGDSSKEY_H
#define CDLGDSSKEY_H

#include "setting/include/menucallback.h"
#include "contacts/directory/include/directorystructure.h"
#include "cdlgbasesubpage.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "dsskeyuilogic/include/moddsskeyuilogic.h"

class CDlgDssKey : public CDlgBaseSubPage, public CMenuCallback
{
    Q_OBJECT

public:
    explicit CDlgDssKey(QWidget * parent = 0);
    ~CDlgDssKey() {}

    // 重载的回调函数
    void OnMenuCallback(void * pData);

    virtual void SetOpenSubPageAction(const yl::string & strSubPageAction);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgDssKey();
    }

    virtual void Init();

    virtual void Uninit();

    virtual bool IsSameSettingDialog(const yl::string & strAction);
    // 重写接受按钮消息
    virtual void OnExtraAction(int eActionType);
    // 写入联系人数据
    void writeData(QString name, QString number);


protected slots:
    //设置不同选项时，对应显示不同的界面。
    void OnCmbBoxTypeCurrentIndexChanged(int nIndex);

    //用于切换不同账号时，显示对应账号的信息，事实上只有选择line时要用到
    void OnCmbBoxAccountIDCurrentIndexChanged(int nIndex);

    // Index改变
    void OnCmbBoxIndexCurrentIndexChanged(int nIndex);

    //打开地址薄
    void OnOpenDirectory(const QString & str);

private:

    // 解析
    static int ParseIdByClickAction(const yl::string & strClickAction, const yl::string & strMask);

    // KeyEvent模式切换处理
    void SetViewByType(DssKey_Type eType);

    //读取当前界面的值
    bool GetPageData();

    //通过控制层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //通过控制层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged()
    {
        return true;
    }

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //将当前保存的配置值在界面上显示出来
    void UpdateData();

    //将当前保存的配置值在界面上显示出来
    void InitDsskeyType();

    //为type下拉框添加数据
    void AddCmbBoxItemData(bool bKeyEvent);

    //为Account ID下拉框添加数据
    void AddAccountIDData();

    //为Index下拉框添加数据，也有可能添加的是group的数据
    void AddIndexData(bool bIndex = true);

    //添加选项
    void PushQWidgetPair(LPCSTR lpName, QWidget * qWidget, int type = 0);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //根据点击事件，判断界面类型
    void SetItemTypeByAction(const yl::string & strAction);

    // 设置所有控件是否启用
    void SetAllWidgetEnable(bool bEnable);

private:
    // 将当前保存的配置值在界面上显示出来
    void GetLogicData();

    // 获取Dsskey类型
    DssKey_Type GetDsskeyType();

    // 重设焦点
    void ResetFocus();

    // 配置单个控件
    void ConfigSingleWidget(QWidget * pWidget, DssKey_Type eType, DSSKEY_ATTR_ITEM eAttrItem,
                            bool bEnabled);

    // 获取控件值
    bool GetWidgetValue(QComboBox * pWidget, int & iValue);

    bool GetWidgetValue(QLineEdit * pWidget, yl::string & strValue);

    // 配置控件值
    bool ConfigCombox(QComboBox * pWidget, int & iValue);

    // 连接信号嘈
    void ConnectSignals();

    // 断开信号嘈
    void DisconnectSignals();

    // 屏蔽信号嘈
    void BlockSignals(bool bBlock);

private:

    DsskeyPageInfo  m_PageInfo;         // dsskey 页面信息
    DssKeyUIData
    m_DssKeyData;       // 加载后，实际显示的数据，可能因为类型找不到而变更数据
    DssKeyUIData    m_OldDssKeyData;    // 加载时，读取的旧配置数据

private:
    QComboBox   *   m_pCmbBoxType;
    QComboBox   *   m_pCmbBoxAccountID;
    QComboBox   *   m_pCmbBoxIndex;
    QComboBox   *   m_pCmbBoxKeyType;

    QLineEdit   *   m_pEditLabel;
    QLineEdit   *   m_pEditValue;
    QLineEdit   *   m_pEditExt;

    VecCandidateValue        m_vecKeyList;
    VecCandidateValue        m_vecKeyEvent;
    int                      m_iDirectoryWriteType;
};

#endif // CDLGDSSKEY_H

#ifndef _EDK_MANAGER_H
#define _EDK_MANAGER_H

#include <map>
#include "uicommon/uimanager/basedialog.h"
#include "singletonclassdefine.h"

#define EDK_SOFTKEY_PROGRAM "Programablekey"
#define EDK_SOFTKEY_PROGRAM_LENGTH 14
#define EDK_MAP_ROOT_NODE   "root"
#define EDK_MAP_ITEM_NODE   "item"
#define EDK_MAP_HINT_ATTR   "hint"
#define EDK_MAP_ACTION_ATTR "action"
#define EDK_MAP_SF_NODE "softkeymap"
#define EDK_MAP_DEFAULT_LABEL   "defaultlabel"
#define EDK_MAP_UI_NODE "uimap"
#define EDK_MAP_DIRECTORY_NODE  "directorymap"
#define EDK_MAP_TYPE_ATTR   "type"
#define EDK_MAP_TYPE_SFID   "softkey_id"
#define EDK_MAP_FILE    "edkmap.xml"

class CEdkUIAction
{
public:
    CEdkUIAction();
    virtual ~CEdkUIAction();
    //打开UI操作
    bool OpenUIAction(yl::string strUIAction, int nIndex = 0);
    //通过key查找value m_mapOpenUI
    yl::string FindValueByKey(yl::string strKey);
    //通过value查找key m_mapOpenUI
    yl::string FindKeyByValue(yl::string strValue);
    //通过value查找key m_mapEnterDirectoryUI
    yl::string FindKeyByIndex(int nIndex);

protected:
    bool OpenUI(yl::string strUIAction, int nIndex = 0);
    //初始化映射
    void Init();
    //从文件加载映射
    void LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode);
    //根据规则进入联系人界面
    bool EnterDirectoryUI(int nType);
    //几个特殊界面特殊判断  有些在黑白屏上是不同界面在彩屏上是同个界面
    bool SpecialUIOpen(yl::string strUIAction);

private:
    std::map<yl::string, yl::string>  m_mapOpenUI;      //UI和相应打开参数的映射
    std::map<yl::string, int>  m_mapEnterDirectoryUI;   //directory打开的特殊映射
};

class CEdkSoftkeyAction
{
public:
    CEdkSoftkeyAction();
    virtual ~CEdkSoftkeyAction();
    //执行softkey
    bool DoSoftkeyAction(yl::string strHint);
    //通过key查找value m_mapSoftkeyAction
    yl::string FindValueByKey(yl::string strKey);
    //通过value查找key m_mapSoftkeyAction
    yl::string FindKeyByValue(yl::string strValue);
    //通过programablekey的index查找映射中的提示模式值 m_mapDefaultLabel
    yl::string GetProgramKeyDefaultLabel(int nProgramIndex);

protected:
    bool DoEdkSoftkeyAction(yl::string strHint);
    //判断softkey在当前UI是否存在
    bool IsActionExist(yl::string strHint, yl::string & strAction);
    //插入当前UI的EDKsoftkey
    void InsertEDKSoftkeyData(CArraySoftKeyBarData & arrSoftKeyBarData);
    void Init();
    //从文件加载映射
    void LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode);
    //通过softkey的type获取提示模式值
    yl::string GetDefaultLabelByType(yl::string strType);
private:
    std::map<yl::string, yl::string>
    m_mapSoftkeyAction;    //存取softkey的提示模式和action的映射
    std::map<yl::string, yl::string>
    m_mapDefaultLabel;     //存取softkeyType和提示模式值的映射
};

class CEdkUIManager
{
public:
    CEdkUIManager();
    virtual ~CEdkUIManager();
    bool DoUIAction(yl::string strUIAction, int nIndex = 0);
    bool DoSoftkeyAction(yl::string strHint);
    //刷新softkey和title
    void RefreshUI2Hint(CBaseDialog * pBaseDlg);
    yl::string GetHintNameBySoftkeyAction(yl::string strAction);
    yl::string GetHintNameByUIAction(yl::string strAction);
    QString GetHintNameByUIAction(QString strAction);
    //获取当前窗口的提示模式的值
    yl::string GetCurPageHint();
    //设置当前窗口提示模式的值
    void SetCurPageHint(yl::string strHint, CBaseDialog * pDlg);
    yl::string GetHintByIndex(int nIndex);

protected:
    void RereshSoftHint(CBaseDialog * pBaseDlg);
    void RereshTitleHint(CBaseDialog * pBaseDlg);

private:
    CEdkUIAction * m_pedkUIAction;
    CEdkSoftkeyAction * m_pedkSoftkeyAction;
};

// 获取全局唯一的EDKUI管理对象
#define _EDKUIManager GetEDKUIManagerInstance()
CEdkUIManager & GetEDKUIManagerInstance();

#endif

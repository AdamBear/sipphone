#ifndef __SETTINGUI_LINEEDIT_ITEM_DATA_H__
#define __SETTINGUI_LINEEDIT_ITEM_DATA_H__

#include "settinguibaseitemdata.h"
#include "ylstl/ylstring.h"

enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_TYPE_LINEEDIT      = 0x00030001,
};

class CSettingUILineEditItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUILineEditItemData();
    virtual ~CSettingUILineEditItemData();

    static int GetLineEditDataItemType();
    static CSettingUILineEditItemData * GetLineEditDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateLineEditDataItem();

    // 解析Item数据
    static bool ParserLineEditItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    virtual bool SetItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool GetItemData(CSettingUIItemDataBase & itemDatas);

    virtual bool LoadItemData(CSettingUIItemDataBase & itemDatas);
    virtual bool SaveItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool IsNeedSaveData()
    {
        return true;
    }

public:
    int GetMaxLength();
    void SetMaxLength(int iMaxLength);
    // 设置最大、小值域
    int GetMinValue();
    void SetMinValue(int nMinValue);
    int GetMaxValue();
    void SetMaxValue(int nMaxValue);

    bool IsPassword();
    void SetPassword(bool bPassword);
    bool IsAllowEmpty();
    void SetAllowEmpty(bool bAllowEmpty);

    const yl::string & GetIME();
    void SetIME(const yl::string & strIME);
    const yl::string & GetDefaultIME();
    void SetDefaultIME(const yl::string & strDefaultIME, bool bConfig = true);
    void SetDefaultIMEByLogicData(const yl::string& strDefaultIme, bool bPassword);
    const yl::string & GetRegularExpressions();
    void SetRegularExpressions(const yl::string & strRE);
    const yl::string & GetCheckBaseID();
    void SetCheckBaseID(const yl::string & strCheckBaseID);
    const yl::string & GetCheckBaseValue();
    void SetCheckBaseValue(const yl::string & strCheckBaseValue);
    // 设置、获取内容为空时，提示信息
    const yl::string & GetHintText();
    void SetHintText(const yl::string & strHintText);
    // 设置、获取校验方法类型
    const yl::string & GetCheckMethodType();
    void SetCheckMethodType(const yl::string & strCheckMethodType);

    bool IsDeleteSoftkeyEnable();
    void SetDeleteSoftkeyEnable(bool bEnable);

    int GetDeleteSoftkeyIndex();
    void SetDeleteSoftkeyIndex(int nIndex);

protected:
    yl::string SettingIMEType2InputIMEType(const yl::string & strSetType);
    yl::string SettingDefaultType2InputType(const yl::string & strSetType);
    // 设置值域区间（闭区间）
    void SetValueRange(const yl::string & strValueRange);

protected:
    int             m_iMaxLength;       // 最大输入字符长度
    int             m_nMinValue;
    int             m_nMaxValue;
    bool            m_bPassword;        // 是否是密码输入框
    bool            m_bAllowEmpty;      // 是否允许空值
    bool            m_bDeleteSoftkey;     // 是否存在删除sofktey
    int             m_nDeleteSoftkeyIndex;  // 删除sofktey索引
    yl::string      m_strIME;           // 需要安装的输入法
    yl::string      m_strDefaultIME;    // 默认输入法
    yl::string      m_strRE;            // 正则表达式
    yl::string      m_strCheckBaseID;   // 根据这个控件ID进行数据校验判断
    yl::string      m_strCheckBaseValue;// 控件ID的值是这个时，才进行数据校验判断
    yl::string      m_strHintText;      // 内容为空时，提示文字
    yl::string      m_strCheckMethodType;   // 校验方法类型
};

typedef CSettingUILineEditItemData   *  CSettingUILineEditItemDataPtr;

#endif


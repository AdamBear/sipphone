#ifndef __SETTINGUI_BASE_ITEM_DATA_H__
#define __SETTINGUI_BASE_ITEM_DATA_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "settinguisoftkeydata.h"
#include "settinguimessagedata.h"

#include "settinguicommondata.h"

#include "xmlparser/pugixml.hpp"
#include "xmlparser/xmlparser.hpp"

class CSettingUIErrorInfo;

enum
{
    // 列表项类型(每个继承类使用一个字节来区分)
    SETTING_ITEM_TYPE_BASE      = 0x01000000,
    SETTING_ITEM_TYPE_MARK      = 0x00FF0000,
};

class CSettingUIItemDataBase
{
public:
    CSettingUIItemDataBase(int eItemType = SETTING_ITEM_TYPE_BASE);
    virtual ~CSettingUIItemDataBase();

    static int GetBaseDataItemType();
    static CSettingUIItemDataBase * GetBaseDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateBaseDataItem();
    virtual void Reset();

    // 解析Item数据，不加入到列表中，返回false（Group中Title就不需要加入）
    static bool ParserItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    // 加载、保存配置值到内存中
    virtual bool LoadItemData(CSettingUIItemDataBase * pItemData);
    virtual bool SaveItemData(const CSettingUIItemDataBase * pItemData);

    // 获取Item点击事件
    virtual const yl::string & GetItemAction();
    // 获取Item焦点事件
    virtual const yl::string & GetItemFocusAction();

    virtual bool IsNeedSaveData()
    {
        return false;
    }

public:
    // 获取、设置控件类型
    int GetWgtType();
    void SetWgtType(int eWgtType);
    // 获取、设置Item类型
    int GetItemType();
    void SetItemType(int eItemType);
    // 获取、设置配置值类型
    int GetValueType();
    void SetValueType(int eValueType);

    // 获取、设置Item过滤时是否可被复位
    bool IsResetable();
    void SetResetable(bool bResetable);
    // 获取、设置Item是否可编辑
    bool IsEnable();
    void SetEnable(bool bEnable);
    // 获取、设置Item是否显示
    bool IsShow();
    void SetShow(bool bShow);
    // 获取、设置Item是否是第2级菜单
    bool IsSecondLevel();
    void SetSecondLevel(bool bSecondLevel);
    // 设置、获取界面数据保存后，提示是否重启话机
    bool IsConfigChangedReboot();
    void SetConfigChangedReboot(bool bConfigChangedReboot);

    // 获取、设置Item ID
    const yl::string & GetItemID();
    void SetItemID(const yl::string & strItemID);
    // 获取、设置Item显示名称
    const yl::string & GetDisplayName();
    void SetDisplayName(const yl::string & strDisplayName);
    // 获取、设置Item显示名称是否加序号
    bool IsShowIndex();
    void SetShowIndex(bool bShowIndex);
    // 获取、设置Item显示名称是否加冒号
    bool IsShowColon();
    void SetShowColon(bool bShowColon);
    // 获取、设置Item配置名称（key）
    const yl::string & GetConfigID();
    void SetConfigID(const yl::string & strConfigID);
    // 获取、设置Item三级权限名称
    const yl::string & GetAuthorityID();
    void SetAuthorityID(const yl::string & strAuthorityID);
    // 获取、设置Item配置值
    const yl::string & GetValue();
    void SetValue(const yl::string & strValue);
    // 获取、设置Extra Data
    void * GetExtraData();
    void SetExtraData(void * pExtraData);

    // 添加、删除Item保存后，需要发的消息
    bool AddItemMessageData(CSettingUIMessageDataPtr pMsgData);
    bool DelItemMessageData(CSettingUIMessageDataPtr pMsgData);

    // 获取、设置保存时，数据校验失败提示信息
    CSettingUIErrorInfo * GetItemErrorInfo();
    void SetItemErrorInfo(CSettingUIErrorInfo * pItemErrorInfo);

    // 获取Sotkey信息
    CSettingUISoftkeyData * GetListSoftkeyData();
    // 获取保存后，需要发的消息列表
    LIST_SETTING_MSG_DATA GetListMessageData();
    LIST_SETTING_MSG_DATA* GetPointListMessageData();

    // 设置、获取附加信息
    const yl::string & GetInnerExtraData() const;
    void SetInnerExtraData(const yl::string & strInnerExtraData);
    // 设置、获取附加信息类型
    int GetInnerExtraDataType() const;
    void SetInnerExtraDataType(int nInnerExtraDataType);

    int TransStringToValueType(const yl::string & strTypeString);

protected:
    int                     m_eWgtType;             // Widget类型
    int                     m_eItemType;            // Item类型
    int                     m_eValueType;           // Wgt值类型
    bool                    m_bResetable;           // 动态过滤时，是否可被复位
    bool                    m_bEnable;              // 是否可编辑
    bool                    m_bShow;                // 是否显示
    bool                    m_bSecondLevel;         // 是否是第2级菜单
    bool                    m_bConfigChangedReboot; // 配置修改后，是否提示重启
    bool                    m_bShowIndex;           // Item显示名称是否加序号
    bool                    m_bShowColon;           // Item显示名称后面是否加冒号
    yl::string              m_strID;                // Item ID
    yl::string              m_strDisplayName;       // Item显示名称
    yl::string              m_strConfigID;          // Item对应配置名称
    yl::string              m_strAuthorityID;       // Item对应三级权限名称
    yl::string              m_strValue;             // Wgt值
    void          *
    m_pExtraData;           // 打开下一层界面携带的附加信息，用户外面设置
    int                     m_nExtraData;
    yl::string
    m_strExtraData;         // 打开下一层界面携带的附加信息，配置表中设置
    int                     m_eExtraDataType;       // 附加信息类型，配置表设置
    LIST_SETTING_MSG_DATA    m_listItemMsg;          // Item保存后发的消息
    CSettingUISoftkeyData * m_pSoftkeyData;         // 项拥有焦点时显示的Softkey
    CSettingUIErrorInfo  *  m_pErrorInfo;           // 校验失败时提示信息
};

typedef CSettingUIItemDataBase   *  CSettingUIItemDataBasePtr;

#endif


#pragma once

#define INVALID_INDEX (-1)
#define DSSKEY_DEFAULT_INDEX  "0"
#define DSSKEY_MAX_EDIT_LENTH  255

enum DSSKEY_ATTR_ITEM
{
    DSSKEY_ATTR_ITEM_NONE = 0,
    DSSKEY_ATTR_ITEM_TYPE,
    DSSKEY_ATTR_ITEM_KEYEVENT,
    DSSKEY_ATTR_ITEM_LINE,
    DSSKEY_ATTR_ITEM_LABEL,
    DSSKEY_ATTR_ITEM_VALUE,
    DSSKEY_ATTR_ITEM_EXT,
    DSSKEY_ATTR_ITEM_INDEX,
    DSSKEY_ATTR_ITEM_LINE_VALUE,
};

enum DSSKEY_UI_TYPE
{
    DSSKEY_UI_NONE = -1,
    DSSKEY_UI_LINEKEY,
    DSSKEY_UI_MEMORYKEY,
    DSSKEY_UI_EXPKEY,
    DSSKEY_UI_SOFTKEY,
};

enum DSSKEY_READONLY_MASK
{
    READONLY_NONE = 0,
    READONLY_EXCEPT_LABEL,
    READONLY_ALL_ITEM,
};

enum DSSKEY_BUTTON_TYPE
{
    DSSKEY_BUTTON_TYPE_NONE,
    DSSKEY_BUTTON_TYPE_DIR_LABEL,
    DSSKEY_BUTTON_TYPE_DIR_VALUE,
    DSSKEY_BUTTON_TYPE_DIR_LABEL_VALUE,
};

struct CDsskeyItemAttr
{
public:
    int        iAttrType;
    bool       bPasswd;

    CDsskeyItemAttr() :  bPasswd(false), strShow(NULL), strRead(NULL),
        strImeType(NULL), strName(NULL), strRegexpID(NULL), strDefault(NULL),
        m_eButtonType(DSSKEY_BUTTON_TYPE_NONE)
    {}

private:
    yl::string * strShow;
    yl::string * strRead;
    yl::string * strImeType;
    yl::string * strName;
    yl::string * strRegexpID;
    yl::string * strDefault;
    DSSKEY_BUTTON_TYPE	m_eButtonType;

    static yl::string strDefaultValue;

public:
    void SetIme(yl::string * pStrValue)
    {
        strImeType = pStrValue;
    }

    void SetName(yl::string * pStrValue)
    {
        strName = pStrValue;
    }

    void SetRegexpID(yl::string * pStrValue)
    {
        strRegexpID = pStrValue;
    }

    void SetReadOnly(yl::string * pStrValue)
    {
        strShow = pStrValue;
    }

    void SetVisible(yl::string * pStrValue)
    {
        strRead = pStrValue;
    }

    void SetDefault(yl::string * pStrValue)
    {
        strDefault = pStrValue;
    }

    void SetButtonType(DSSKEY_BUTTON_TYPE eButtonType)
    {
        m_eButtonType = eButtonType;
    }

    const yl::string & GetIme();

    const yl::string & GetName();

    const yl::string & GetRegexp();

    const yl::string & GetDefault();

    DSSKEY_BUTTON_TYPE GetButtonType();

    bool GetPasswdFlag()
    {
        return bPasswd;
    }

    bool IsReadOnly();

    bool IsVisible();
};

typedef YLVector<CDsskeyItemAttr *> VecAttrItem;

// 用于combox控件的显示
struct CDsskeyPair
{
    int         iIndex;
    yl::string  strValue;

    CDsskeyPair() : iIndex(0) {}
    CDsskeyPair(int i, const yl::string & str) : iIndex(i), strValue(str) {}
};

typedef YLVector<CDsskeyPair> VecCandidateValue;

// Dsskey 信息，只读信息，不需要从UI保存
struct DssKeyDataEx
{
    yl::string          strServer;
    yl::string          strUserName;
    VecCandidateValue   vecValue;
};

typedef YLList<int>                 vecDsskeySet;
typedef YLVector<yl::string>            vecString;

// struct DSS key data
struct DssKeyUIData
{
    DssKey_Type         m_eType;                // DSS key type.
    int                 m_iLineIndex;           // Line Index
    int                 m_iIndex;               // Index Id.
    yl::string          m_strValue;             // Value.
    yl::string          m_strExtension;         // Extension.
    yl::string          m_strLabel;             // Label
    yl::string          m_strShortLabel;                // Short Label

    VecCandidateValue   m_vecData;              // extra data

    DssKeyUIData()
    {
        m_eType = DT_NA;
        m_iLineIndex = -1;   // Line Index
        m_iIndex = 0;
    }

    void Reset()
    {
        m_eType = DT_NA;
        m_iLineIndex = -1;   // Line Index
        m_iIndex = 0;
        m_strValue.clear();
        m_strExtension.clear();
        m_strLabel.clear();
        m_strShortLabel.clear();
        //m_vecData.clear(); //不需要清空
    }
};

struct DsskeyPageInfo
{
    DSSKEY_UI_TYPE      m_eUIType;              // ui type
    int                 m_iIndex;         // DSSKey index in moudle
    int                 m_iDsskeyID;       // DsskeyID

    DsskeyPageInfo()
    {
        m_eUIType = DSSKEY_UI_NONE;
        m_iIndex = INVALID_INDEX;
        m_iDsskeyID = INVALID_INDEX;
    }

    // 生成逻辑层序号
    void SetIndex(int iIndex, int iMoudleIndex);
};

bool dsskey_data_check_imp(int dsskeyID, const DssKeyUIData & dsskeyData, CDsskeyPair & errorTips);

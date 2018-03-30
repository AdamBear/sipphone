#ifndef _SETTING_UI_COMMON_H_
#define _SETTING_UI_COMMON_H_

#include "settingui/src/settinguifeaturesdefine.h"
#include "ylstl/ylstring.h"
#include <QVariant>
class CMsgBoxCallBackAction;

//Action Define
#define ACTION_SAVE_DATA    "SaveData"
#define ACTION_QUIT         "Back"
#define ACTION_NEXT_PAGE    "NextPage"

//XSI密码弹出框的ID
const int MSGBOX_ID_POP_XSI_PWD = 0xffff;

enum PCPORT_MODE
{
    BRIDGE = 0,
    ROUTER
};

//call control当前弹出的提示框类型
enum EMessageBoxType
{
    EMBT_NONE = 0,
    EMBT_LOAD,
    EMBT_SAVE,
    EMBT_DELETE
};

enum SETTING_PAGE_STYLE
{
    SETTING_PAGE_STYLE_NO_MENU,
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    SETTING_PAGE_STYLE_WITH_MENU,
#endif
};

// 打开CPasswordDlg界面的数据结构
class CPasswordDlgData
{
public:
    CPasswordDlgData()
    {
        InitData();
    }

    ~CPasswordDlgData() {}

    // 初始化Softkey数据
    void InitData()
    {
        m_pMsgBoxCallBack = NULL;
        m_extraData = NULL;
    }

    // 重载赋值操作符
    CPasswordDlgData & operator=(const CPasswordDlgData & other)
    {
        // 证同测试
        if (this == &other)
        {
            return *this;
        }

        this->m_pMsgBoxCallBack = other.m_pMsgBoxCallBack;
        this->m_extraData = other.m_extraData;
        return *this;
    }

public:
    CMsgBoxCallBackAction * m_pMsgBoxCallBack; //添加信号接收对象指针成员变量
    QVariant m_extraData;
};

Q_DECLARE_METATYPE(CPasswordDlgData);

#endif

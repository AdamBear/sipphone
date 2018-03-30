#ifndef _DSSKEY_MOD_STRUCTURE_HEADER_
#define  _DSSKEY_MOD_STRUCTURE_HEADER_
#include "dsskey_enumtypes.h"
#include <ylstl/yllist.h>
#include <ylstl/ylstring.h>
#include "lamp/ledlamp/include/modledlamp.h"

#define INVALID_DSSKEY_ID ((int)-1)

// DssKeyLogicData
struct DssKeyLogicData
{
    DssKeyLogicData()
    {
        clear();
    }

    int iLineID;                // 账号的ID

    DssKeyLockType eLockType; //Lock Type
    DssKey_Type eDKType;        // The DK_Type.
    yl::string strLabel;            // The label.
    yl::string strShortLabel;       // short label
    yl::string strValue;            // 号码、URL等……
    yl::string strExtension;        // 扩展数据
    DSSKEY_MODULE_TYPE eType; //扩展台类型
    int iLineIndex;             // 账号的位置，-1表示为Auto,当为BLA类型时，由于没有Auto选项，故需要调整+1
    //int iSelectedIndex;           // 进入菜单页面的初始焦点
    int iDssKeyID;
    int iStatus;
    YLList<int> listAuxiliaryState;     // 辅助状态链表
    int iTranslateLength;//翻译长度
#if IF_BUG_XMLBROWSER_EXP_LED
    LIGHT_Type eLightType;  //XML Browser控制亮灯
#endif

    DssKeyLogicData & operator=(const DssKeyLogicData & data)
    {
        if (this != &data)
        {
            eLockType = data.eLockType;
            eDKType = data.eDKType;
            strLabel = data.strLabel;
            strShortLabel = data.strShortLabel;
            strValue = data.strValue;
            strExtension = data.strExtension;
            eType = data.eType;
            iLineIndex = data.iLineIndex;
            //iSelectedIndex = data.iSelectedIndex;
            iDssKeyID = data.iDssKeyID;
            iStatus = data.iStatus;
            iLineID = data.iLineID;
            iTranslateLength = data.iTranslateLength;
            listAuxiliaryState = data.listAuxiliaryState;
#if IF_BUG_XMLBROWSER_EXP_LED
            eLightType = data.eLightType;
#endif
        }

        return *this;
    }

    bool operator==(const DssKeyLogicData & data) const
    {
        if (this != &data)
        {
            if (iLineID == data.iLineID &&
                    eLockType == data.eLockType &&
                    eDKType == data.eDKType &&
                    strLabel == data.strLabel &&
                    strExtension == data.strExtension &&
                    strValue == data.strValue &&
                    eType == data.eType &&
                    iLineIndex == data.iLineIndex &&
                    /*iSelectedIndex == data.iSelectedIndex &&*/
                    iDssKeyID == data.iDssKeyID &&
                    iStatus == data.iStatus &&
                    iTranslateLength == data.iTranslateLength
                    && strShortLabel == data.strShortLabel
#if IF_BUG_XMLBROWSER_EXP_LED
                    && eLightType == data.eLightType
#endif
               )
            {
                return true;
            }

            return false;
        }
        return true;
    }

    bool isDataEqual(const DssKeyLogicData & data) const
    {
        return  this->operator ==(data);
    }

    // 写入配置的值是否改变
    bool isConfigChanged(const DssKeyLogicData & data) const
    {
        if (this == &data)
        {
            return false;
        }

        return iLineIndex != data.iLineIndex || eDKType != data.eDKType
               || strLabel != data.strLabel || strValue != data.strValue
               || strExtension != data.strExtension || strShortLabel != data.strShortLabel;
    }

    // 状态是否需要改变
    bool isStatusChanged(const DssKeyLogicData & data) const
    {
        if (this == &data)
        {
            return false;
        }

        return iLineIndex != data.iLineIndex || eDKType != data.eDKType
               || iStatus != data.iStatus || strValue != data.strValue
               || strExtension != data.strExtension;
    }

    void clear()
    {
        eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
        eDKType = DT_NONE;
        eType = DMT_NON;
        strShortLabel.clear();
        strLabel.clear();
        strValue.clear();
        strExtension.clear();
        iLineIndex = -1;
        //iSelectedIndex = -1;
        iDssKeyID = -1;
        iStatus = -1;
        iLineID = -1;
        iTranslateLength = -1;
        listAuxiliaryState.clear();
#if IF_BUG_XMLBROWSER_EXP_LED
        eLightType = LT_NONE;
#endif
    }

    // 重置，dsskey其它信息不修改
    void reset()
    {
        eDKType = DT_NA;
        iLineIndex = -1;
        clear_value();
    }

    void clear_value()
    {
        eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
        strShortLabel.clear();
        strLabel.clear();
        strValue.clear();
        strExtension.clear();
        iStatus = -1;
        iTranslateLength = -1;
        listAuxiliaryState.clear();
#if IF_BUG_XMLBROWSER_EXP_LED
        eLightType = LT_NONE;
#endif
    }
};

#endif


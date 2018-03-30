#include "../include/moddsskey.h"
#include "../include/dsskeystructure.h"
#include "../include/idsskeyimplaction.h"
#include "../include/dsskey_enumtypes.h"
#include "../include/idsskeyimplaction.h"

#if LINUX_SYSTEM
class CTestClass: public IDsskeyImplAction
{
public:
    CTestClass(DssKey_Type dsskeyType): IDsskeyImplAction(dsskeyType)
    {
        m_bClick = false;
        m_bLongClick = false;
        m_bBind = false;

        m_bDebind = false;
        m_bTimer = false;
        m_bIsBinderType = false;

        m_bSettimer = false;
        m_bGetCallOut = false;
    }
    virtual ~CTestClass()
    {

    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        printf("conf key click\n");
        m_bClick = true;

    }


public:
    bool m_bClick;
    bool m_bLongClick;
    bool m_bBind;

    bool m_bDebind;
    bool m_bTimer;
    bool m_bIsBinderType;

    bool m_bSettimer;
    bool m_bGetCallOut;
};

class CNaActionClass: public IDsskeyImplAction
{
public:
    CNaActionClass(DssKey_Type dsskeyType): IDsskeyImplAction(dsskeyType)
    {

        m_bClick = false;
        m_bLongClick = false;
        m_bBind = false;

        m_bDebind = false;
        m_bTimer = false;
        m_bIsBinderType = false;

        m_bSettimer = false;
        m_bGetCallOut = false;
    }
    virtual ~CNaActionClass()
    {

    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        m_bClick = true;

    }


public:
    bool m_bClick;
    bool m_bLongClick;
    bool m_bBind;

    bool m_bDebind;
    bool m_bTimer;
    bool m_bIsBinderType;

    bool m_bSettimer;
    bool m_bGetCallOut;
};
#include <gtest/gtest.h>

TEST(dsskey, dsskey_Init)
{
    dsskey_Init();

    int amount  = dsskey_GetDSSKeyAmount();

    amount = dsskey_GetModulesAmount();

    EXPECT_EQ(3, amount);
}

CTestClass * pClass = new CTestClass(DT_CONF);
CNaActionClass * pNaClass = new CNaActionClass(DT_NA);

#if 0
#include "../src/dsskeyactionmanager.h"
#include "../src/dsskeyaction.h"
TEST(dsskey, dsskey_Action)
{
    int type = DT_NA;
    for (; type < DT_LAST; type++)
    {
        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction((DssKey_Type)type);
        EXPECT_TRUE(pAction != NULL);
        EXPECT_TRUE(pAction->GetDsskeyType() == (DssKey_Type)type);
        DssKeyLogicData logicData;
        pAction->OnKeyPress(1, logicData, NULL);
        if (type == DT_CONF)
        {
            printf("CONF_Click\n");
            EXPECT_EQ(true, pClass->m_bClick);
        }
        if (type == DT_NA)
        {
            printf("NA_Click\n");
            EXPECT_EQ(true, pNaClass->m_bClick);
        }

    }
}

TEST(dsskey, dsskey_add)
{
    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {
        DSSKEY_MODULE_TYPE  type = dsskey_GetModuleType(*iter);

        DssKeyLogicData logicData;

        dsskey_GetData(*iter, logicData);

        EXPECT_TRUE(logicData.eType == DMT_LINEKEY_MODULE);

        EXPECT_TRUE(type == DMT_LINEKEY_MODULE);


        // dsskey_SetDsskeyTimerID(*iter, 1);

        /* UINT timerID = dsskey_GetRelatedTimerID(*iter);

         EXPECT_EQ(1, timerID);*/


        dsskey_SetDssekeyExtFiled(*iter, "strField", "strValue");

        yl::string strValue("");
        bool bRet = dsskey_GetDssekeyExtFiled(*iter, "strField", strValue);

        EXPECT_TRUE(bRet);

        EXPECT_STREQ("strValue", strValue.c_str());

        bRet = dsskey_GetDssekeyExtFiled(*iter, "asdf", strValue);

        EXPECT_TRUE(bRet);

        EXPECT_STREQ("", strValue.c_str());

    }

    dsskeyList.clear();
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_PROGRAMEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    iter =  dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {
        DSSKEY_MODULE_TYPE  type = dsskey_GetModuleType(*iter);

        EXPECT_TRUE(type == DMT_PROGRAMEKEY_MODULE);

        DssKeyLogicData logicData;

        dsskey_GetData(*iter, logicData);

        EXPECT_TRUE(logicData.eType == DMT_PROGRAMEKEY_MODULE);
        //dsskey_SetDsskeyTimerID(*iter, 1);

        //UINT timerID = dsskey_GetRelatedTimerID(*iter);

        //EXPECT_EQ(1, timerID);


        dsskey_SetDssekeyExtFiled(*iter, "strField", "strValue");

        yl::string strValue("");
        bool bRet = dsskey_GetDssekeyExtFiled(*iter, "strField", strValue);

        EXPECT_TRUE(bRet);

        EXPECT_STREQ("strValue", strValue.c_str());

        bRet = dsskey_GetDssekeyExtFiled(*iter, "asdf", strValue);

        EXPECT_TRUE(bRet);

        EXPECT_STREQ("", strValue.c_str());

    }
}
/************************************************************************/
// 函数描述  ： 按键处理
// 参数说明 ：
//     [iDssKeyID] DssKey的ID
// 返回值 ： 无
/************************************************************************/
TEST(dsskey, dsskey_OnDsskeyClick)
{
    bool conf_set = false;
    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {
        DssKey_Type  type = dsskey_GetDsskeyType(*iter);
        dsskey_OnDsskeyClick(*iter);

        dsskey_OnDssKeyLongClick(*iter);


        if (type == DT_NA)
        {
            EXPECT_EQ(true, pNaClass->m_bClick);
            pNaClass->m_bClick = false;
        }

        bool ret = dsskey_SetDsskeyType(*iter, DT_CONF);

        EXPECT_EQ(true, ret);
        type = dsskey_GetDsskeyType(*iter);
        dsskey_OnDsskeyClick(*iter);

        dsskey_OnDssKeyLongClick(*iter);
        if (type == DT_CONF)
        {
            printf("conference \n");
            EXPECT_EQ(true, pClass->m_bClick);
            //EXPECT_EQ(true, pClass->m_bLongClick);
            pClass->m_bClick = false;
            pClass->m_bLongClick = false;
        }
    }
}


TEST(dsskey, dsskey_programekey)
{
    YLList<int> programekey;
    dsskey_GetAllDsskeyDataByModule(programekey, DMT_PROGRAMEKEY_MODULE);
    YLList<int>::ListIterator iter = programekey.begin();
    for (; iter != programekey.end(); iter++)
    {
        DssKeyLogicData data;
        dsskey_GetData(*iter, data);
        printf("id[%d] type[%d] label[%s] value[%s]\n", data.iDssKeyID, (int)data.eDKType,
               data.strLabel.c_str(), data.strValue.c_str());
    }

    programekey.clear();

    dsskey_GetAllDsskeyDataByModule(programekey, DMT_LINEKEY_MODULE);
    iter = programekey.begin();
    for (; iter != programekey.end(); iter++)
    {
        DssKeyLogicData data;
        dsskey_GetData(*iter, data);
        printf("linkey id[%d] type[%d] value[%s]\n", data.iDssKeyID, (int)data.eDKType,
               data.strValue.c_str());
    }
}

/************************************************************************/
// 接口  ： dsskey_OnDssKeyLongClick(int iDssKeyID)
// 功能  ： Linekey按键长按的处理
// 参数说明 ： [iDssKeyID] DssKey的ID
// 返回值 ： 无
/************************************************************************/
TEST(dsskey, dsskey_OnDssKeyLongClick)
{
    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);

    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        DssKey_Type  type = dsskey_GetDsskeyType(*iter);
        dsskey_OnDssKeyLongClick(*iter);
    }

    bool bret = dsskey_OnDssKeyLongClick(-1);
    EXPECT_EQ(false, bret);
}


/************************************************************************/
// 函数描述  ： 更新dsskey状态
// 参数说明 ：
//     [iDssKeyID] DssKey的ID
//     [eStatus] DssKey的状态
// 返回值 ： 更新是否成功
/************************************************************************/
TEST(dsskey, dsskey_UpdateStatus)
{
    dsskey_UpdateStatus(-1, DS_LINE_NONE, DS_LINE_FIRST, DS_LINE_END);

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);

    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        DssKey_Type  type = dsskey_GetDsskeyType(*iter);
        dsskey_UpdateStatus(*iter, DS_LINE_NONE, DS_LINE_FIRST, DS_LINE_END);
    }
}

// 移除状态
TEST(dsskey, dsskey_RemoveStatus)
{
    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    dsskey_RemoveStatus(-1,  DS_LINE_FIRST, DS_LINE_END);

    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        DssKey_Type  type = dsskey_GetDsskeyType(*iter);
        dsskey_RemoveStatus(*iter, DS_LINE_FIRST, DS_LINE_END);
    }
}

/*******************************************************************/
// 函数描述:   获取EXP数量
// 参数：      无
// 返回:       exp的数量
/*******************************************************************/
TEST(dsskey, dsskey_GetEXPNum)
{
    int expNum = dsskey_GetEXPNum();

    EXPECT_EQ(0, expNum);
}

/*******************************************************************/
// 函数描述:   获取EXP类型
// 参数：      无
// 返回:       返回Exp管理模块的当前EXP类型
/*******************************************************************/
TEST(dsskey, dsskey_GetEXPType)
{
    DEVICE_TYPE_DEF expType = dsskey_GetEXPType();

    EXPECT_EQ(DT_INVALID, expType);
}


/************************************************************************/
// 函数描述 ： 是否EXP类型的按键
// 参数说明 ：
//     [iDssKeyID] DssKey的ID
// 返回值 ： true：是EXP   false: 不是EXP
/************************************************************************/
TEST(dsskey, dsskey_IsEXPKey)
{
    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        bool bret = dsskey_IsEXPKey(*iter);

        EXPECT_EQ(true, bret);
    }
}



//===========================================================================
//获取DssKey属性的状态
TEST(dsskey, dsskey_GetData)
{
#if 1
    DssKeyLogicData logicData;
    bool bret = dsskey_GetData(-1, logicData);
    EXPECT_EQ(false, bret);


    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        DssKeyLogicData logicData;
        bool bret = dsskey_GetData(*iter, logicData);


        EXPECT_EQ(*iter, logicData.iDssKeyID);


        if (logicData.eDKType == DT_NA)
        {
            logicData.eDKType = DT_LINE;
            dsskey_SetDsskeyData(*iter, logicData);
        }

        bret = dsskey_SetDsskeyStatus(*iter, DS_LINE_UNREGISTERED);

        logicData.clear();
        bret = dsskey_GetData(*iter, logicData);

        EXPECT_EQ(*iter, logicData.iDssKeyID);
        EXPECT_EQ((int)DS_LINE_UNREGISTERED, logicData.iStatus);
    }
#endif
}

TEST(dsskey, dsskey_GetData1)
{
    DssKeyLogicData logicData;
    YLList<DssKeyLogicData> listOutput;
    dsskey_GetData(DMT_LINEKEY_MODULE, 0, 10, listOutput);
}






/*******************************************************************
** 函数描述:   EHS控制接口
** 参数：      [in] iIndex:ESH在EXP中的序号
** 参数：      [in] iCtrlCode:ESH控制码
** 返回:       空
********************************************************************/
TEST(dsskey, dsskey_EHSCtrl)
{
    for (int i = -50; i < 50 ; i ++)
    {
        for (int j = -50; j < 50 ; j++)
        {
            dsskey_EHSCtrl(i, j);
        }
    }

}


/*******************************************************************
** 函数描述:   更新DND状态
** 参数：      [in] bDndOn: DND开启状态
** 返回:       空
********************************************************************/
TEST(dsskey, dsskey_UpdateDNDStatus)
{
    dsskey_UpdateDNDStatus(true);
    dsskey_UpdateDNDStatus(false);
}


/*******************************************************************
** 函数描述:   更新FWD状态
** 参数：      [in] bFwdOn: FWD开启状态
** 返回:       空
********************************************************************/
TEST(dsskey, dsskey_UpdateFWDStatus)
{
    dsskey_UpdateFWDStatus(true);
    dsskey_UpdateFWDStatus(false);
}


/*******************************************************************
** 函数描述:   是否SCA类型的linekey
** 参数：      [in] iKeyIndex: linekey序号，0~x
** 返回:       TRUE: 是；FALSE: 否
********************************************************************/
TEST(dsskey, dsskey_IsSCATypeLinekey)
{
    DssKeyLogicData logicData;
    bool bret = dsskey_IsSCATypeLinekey(-1);
    EXPECT_EQ(false, bret);
    bret = dsskey_IsSCATypeLinekey(0);
    EXPECT_EQ(false, bret);
    bret = dsskey_IsSCATypeLinekey(1);
    EXPECT_EQ(false, bret);

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        bret = dsskey_IsSCATypeLinekey(*iter);
        EXPECT_EQ(false, bret);
    }
}


/*******************************************************************
** 函数描述:   当DND或FWD状态有改变时更新Dsskey
** 参数：      [in] iAccount: 账号ID
** 返回:       TRUE: 更新成功；FALSE: 更新失败
********************************************************************/
TEST(dsskey, dsskey_UpdateStatusByDNDOrFWDChanged)
{
    for (int i = -100; i < 100; i++)
    {
        dsskey_UpdateStatusByDNDOrFWDChanged(i);
    }

}


/*******************************************************************
** 函数描述:   设置linekey的callpark状态
** 参数：      [in] iAccount: 账号ID
               [in] bOn: 是否开启SCA CallPark
** 返回:       TRUE: 更新成功；FALSE: 更新失败
********************************************************************/
TEST(dsskey, dsskey_ProcessCallParkNotify)
{
    for (int i = -100; i < 100; i ++)
    {
        dsskey_ProcessCallParkNotify(i, false);
        dsskey_ProcessCallParkNotify(i, true);
    }

}






//获取dsskey数量
TEST(dsskey, dsskey_GetDSSKeyAmount)
{
    int amount  = dsskey_GetDSSKeyAmount();
}



//设置DSSKey数据
/*******************************************************************
** 函数描述:    设置DSSKey数据
** 参数：        [in] dsskeyID: dsskey id
                [in] dsskeyLogicData: dsskey数据
** 返回:      设置结果
********************************************************************/
TEST(dsskey, dsskey_SetDsskeyData)
{
    DssKeyLogicData logicData;
    bool bRet = dsskey_SetDsskeyData(-1, logicData);
    EXPECT_EQ(false, bRet);

    bRet = dsskey_SetDsskeyData(0, logicData);
    EXPECT_EQ(true, bRet);

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {

        DssKeyLogicData logicData;
        DssKey_Type  type = dsskey_GetDsskeyType(*iter);
        dsskey_OnDsskeyClick(*iter);


        if (type == DT_NA)
        {
            EXPECT_EQ(true, pNaClass->m_bClick);
            pNaClass->m_bClick = false;
        }


        logicData.eDKType = DT_CONF;
        logicData.strExtension =  "extension";
        logicData.strLabel =  "label";
        logicData.strValue =  "value";
        bool ret = dsskey_SetDsskeyData(*iter, logicData);
        EXPECT_EQ(true, ret);

        DssKeyLogicData logicDataOut;

        ret = dsskey_GetData(*iter, logicDataOut);

        EXPECT_EQ(true, ret);

        EXPECT_TRUE(logicData.eDKType == logicDataOut.eDKType);
        EXPECT_STREQ(logicData.strExtension.c_str(), logicDataOut.strExtension.c_str());
        EXPECT_STREQ(logicData.strLabel.c_str(), logicDataOut.strLabel.c_str());
        EXPECT_STREQ(logicData.strValue.c_str(), logicDataOut.strValue.c_str());
    }
}


//获取dsskey状态链表
TEST(dsskey, dsskey_GetDsskeyStatus)
{
    Dsskey_Status status = dsskey_GetDsskeyStatus(-1);

    EXPECT_EQ(DS_NON, status);

    status = dsskey_GetDsskeyStatus(0);
    EXPECT_EQ(DS_NON, status);

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_NON, status);

        dsskey_SetDsskeyStatus(*iter, DS_ACD_WRAPUP);

        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_WRAPUP, status);

        dsskey_SetDsskeyStatus(*iter, DS_ACD_UNAVAILABLE);

        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_UNAVAILABLE, status);

        dsskey_SetDsskeyStatus(*iter, DS_AUTO_PICKUP_OFF);

        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_UNAVAILABLE, status);





        YLList<Dsskey_Status> statusList;


        dsskey_GetDsskeyStatus(*iter, statusList);

        EXPECT_EQ(2, statusList.size());


        dsskey_RemoveStatus(*iter, DS_ACD_FIRST, DS_ACD_LAST);

        statusList.clear();

        dsskey_GetDsskeyStatus(*iter, statusList);

        EXPECT_EQ(1, statusList.size());

        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_AUTO_PICKUP_OFF, status);

        dsskey_SetDsskeyStatus(*iter, DS_NON);

        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_NON, status);



        statusList.clear();

        dsskey_GetDsskeyStatus(*iter, statusList);

        EXPECT_EQ(0, statusList.size());

        dsskey_SetDsskeyStatus(*iter, DS_LINE_NONE);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_NONE, status);

        dsskey_SetDsskeyStatus(*iter, DS_LINE_UNREGISTERED);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_UNREGISTERED, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_REGISTERING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_REGISTERING, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_REGISTERED);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_REGISTERED, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_USING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_USING, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_DISABLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_DISABLE, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_FWD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_FWD, status);
        dsskey_SetDsskeyStatus(*iter, DS_LINE_DND);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(1, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_LINE_DND, statusList.back());


        // blf状态
        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_IDLE);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_IDLE, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_RINGING);


        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_RINGING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_TALKING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_TALKING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_UNKOWN);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_UNKOWN, statusList.back());


        //Broadsoft SCA Status
        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_IDLE);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_IDLE, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_SEIZED);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_SEIZED, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_PROGRESSING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_PROGRESSING, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_ALERTING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_ALERTING, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_ACTIVE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_ACTIVE, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_HELD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_HELD, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_HELD_PRIVATE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_HELD_PRIVATE, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_BRIDGE_ACITVE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_BRIDGE_ACITVE, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_BRIDGE_HELD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_BRIDGE_HELD, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLA_STATUS_UNKOWN);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_STATUS_UNKOWN, statusList.back());



        //BLA Draft Status
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_IDLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_IDLE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_RINGING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_RINGING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_TALKING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_TALKING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_HELD_PUBLIC);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_HELD_PUBLIC, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_UNKOWN);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_UNKOWN, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_HOLD);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_HOLD, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_CALLOUT);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_CALLOUT, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLF_STATUS_PARK);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_STATUS_PARK, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_CANCEL_TRYING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_CANCEL_TRYING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_TERMINATED);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_TERMINATED, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLA_DRAFT_STATUS_HELD_PRIVATE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(2, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLA_DRAFT_STATUS_HELD_PRIVATE, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_BLF_LED_MODE1);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(3, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_LED_MODE1, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_BLF_LED_MODE2);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(4, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_BLF_LED_MODE2, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_SCA_CALLPARK_NON);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_CALLPARK_NON, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_IDLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_IDLE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_SEIZED);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_SEIZED, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_PROGRESSING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_PROGRESSING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_ALERTING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_ALERTING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_ACTIVE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_ACTIVE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_HELD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_HELD, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_HELD_PRIVATE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_HELD_PRIVATE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_BRIDGE_ACITVE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_BRIDGE_ACITVE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_BRIDGE_HELD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_BRIDGE_HELD, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_STATUS_UNKOWN);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_STATUS_UNKOWN, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SCA_CALLPARK);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(5, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SCA_CALLPARK, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_AUTO_PICKUP_ON);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(6, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_AUTO_PICKUP_ON, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_AUTO_PICKUP_OFF);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(6, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_AUTO_PICKUP_OFF, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_ACD_IDLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(7, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_IDLE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_ACD_AVAILABLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(7, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_AVAILABLE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_ACD_UNAVAILABLE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(7, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_UNAVAILABLE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_ACD_WRAPUP);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(7, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_ACD_WRAPUP, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_SESSION_NORMAL_USING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_NORMAL_USING, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_HIDE);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_HIDE, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_HOLD);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_HOLD, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_CALL_IN);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_CALL_IN, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_CALL_Out);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_CALL_Out, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_TALK);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_TALK, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_SESSION_DIAL);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_SESSION_DIAL, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_NO_SESSION);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(8, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_NO_SESSION, statusList.back());


        dsskey_SetDsskeyStatus(*iter, DS_FWD_USING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(9, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_FWD_USING, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_DND_USING);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(10, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_DND_USING, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_URL_OK);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(11, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_URL_OK, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_URL_NETWORK_UNKNOW);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(11, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_URL_NETWORK_UNKNOW, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_URL_NETWORK_UNAVAILABLE);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(11, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_URL_NETWORK_UNAVAILABLE, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_HOTELING_ON);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(12, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_HOTELING_ON, statusList.back());
        dsskey_SetDsskeyStatus(*iter, DS_HOTELING_OFF);
        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(12, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_HOTELING_OFF, statusList.back());

        dsskey_SetDsskeyStatus(*iter, DS_RECORD_USING);

        statusList.clear();
        dsskey_GetDsskeyStatus(*iter, statusList);
        EXPECT_EQ(13, statusList.size());
        status = dsskey_GetDsskeyStatus(*iter);
        EXPECT_EQ(DS_RECORD_USING, statusList.back());

    }
}

//获取DSSKey状态
TEST(dsskey, dsskey_GetDsskeyStatus_LIST)
{
    YLList<Dsskey_Status> listStatus;
    bool bRet = dsskey_GetDsskeyStatus(-1, listStatus);

    EXPECT_EQ(false, bRet);
    EXPECT_EQ(0, listStatus.size());

    bRet = dsskey_GetDsskeyStatus(0, listStatus);

    EXPECT_EQ(true, bRet);
    EXPECT_EQ(0, listStatus.size());

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, dsskeyList.size());
    YLList<int>::ListIterator iter = dsskeyList.begin();
    for (; iter != dsskeyList.end(); iter++)
    {
        YLList<Dsskey_Status> listStatus;
        bRet = dsskey_GetDsskeyStatus(*iter, listStatus);

        EXPECT_EQ(true, bRet);
        EXPECT_EQ(0, listStatus.size());

        dsskey_SetDsskeyStatus(*iter, DS_ACD_WRAPUP);

        bRet = dsskey_GetDsskeyStatus(*iter, listStatus);

        EXPECT_EQ(true, bRet);
        EXPECT_NE(0, listStatus.size());

        EXPECT_EQ(1, listStatus.size());

        EXPECT_EQ(DS_ACD_WRAPUP, listStatus.front());

    }
}


//获取某一个模块所有的DSSKey数据
TEST(dsskey, dsskey_GetAllDsskeyDataByModule)
{
    YLList<int> listOutput;
    dsskey_GetAllDsskeyDataByModule(listOutput, DMT_LINEKEY_MODULE);

    EXPECT_NE(0, listOutput.size());
    listOutput.clear();
    dsskey_GetAllDsskeyDataByModule(listOutput, DMT_MEMKEY_MODULE);
    EXPECT_NE(0, listOutput.size());
    listOutput.clear();
    dsskey_GetAllDsskeyDataByModule(listOutput, DMT_PROGRAMEKEY_MODULE);
    EXPECT_NE(0, listOutput.size());
}

//获取共有多少DSSKey管理模块
TEST(dsskey, dsskey_GetModulesAmount)
{
    int amount = dsskey_GetModulesAmount();
    EXPECT_EQ(3, amount);
}

//获取指定数据的DSSKey类型
TEST(dsskey, dsskey_GetDsskeyType)
{
    DssKey_Type type = dsskey_GetDsskeyType(-1);

    EXPECT_EQ(DT_NONE, type);
    YLList<int> listOutput;
    dsskey_GetAllDsskeyDataByModule(listOutput, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, listOutput.size());
    YLList<int>::ListIterator iter = listOutput.begin();
    for (; iter != listOutput.end(); iter++)
    {
        DssKey_Type type = dsskey_GetDsskeyType(*iter);

        EXPECT_NE(DT_NONE, type);

        DssKeyLogicData logicData;

        dsskey_GetData(*iter, logicData);

        EXPECT_EQ(type, logicData.eDKType);


        dsskey_SetDsskeyType(*iter, DT_CONF);


        logicData.clear();
        type = dsskey_GetDsskeyType(*iter);

        EXPECT_EQ(DT_CONF, type);


        dsskey_GetData(*iter, logicData);

        EXPECT_EQ(type, logicData.eDKType);


    }
}


//获取DSSKey模块链表
TEST(dsskey, dsskey_GetModulesList)
{
    YLList<int> listModuleTypes;

    dsskey_GetModulesList(listModuleTypes);

    EXPECT_EQ(3, listModuleTypes.size());

    YLList<int>::ListIterator iter = listModuleTypes.begin();
    for (; iter != listModuleTypes.end(); iter++)
    {
        printf("moudle types %d\n", *iter);
    }
}


//重新加载指定模块的数据
TEST(dsskey, dsskey_ReloadDSSKeyData)
{
    YLList<int> listModuleTypes;

    dsskey_GetModulesList(listModuleTypes);

    EXPECT_EQ(3, listModuleTypes.size());

    YLList<int>::ListIterator iter = listModuleTypes.begin();
    for (; iter != listModuleTypes.end(); iter++)
    {
        bool bRet = dsskey_ReloadDSSKeyData(*iter);

        EXPECT_EQ(true, bRet);

    }
}


TEST(dsskey, dsskey_GetUnboundDsskeyByAccountID)
{

}


TEST(dsskey, dsskey_GetDsskeyValue)
{
    yl::string str = dsskey_GetDsskeyValue(-1);

    EXPECT_STREQ("", str.c_str());
    YLList<int> listOutput;
    dsskey_GetAllDsskeyDataByModule(listOutput, DMT_LINEKEY_MODULE);
    EXPECT_NE(0, listOutput.size());
    YLList<int>::ListIterator iter = listOutput.begin();
    for (; iter != listOutput.end(); iter++)
    {
        yl::string str = dsskey_GetDsskeyValue(*iter);

        yl::string str1 = dsskey_GetDsskeyLabel(*iter);

        yl::string str2 = dsskey_GetDsskeyExtension(*iter);

        EXPECT_STREQ("value", str.c_str());

        EXPECT_STREQ("label", str1.c_str());

        EXPECT_STREQ("extension", str2.c_str());
        DssKeyLogicData logicData;

        dsskey_GetData(*iter, logicData);

        EXPECT_STREQ(str.c_str(), logicData.strValue.c_str());


        dsskey_SetDsskeyValue(*iter, "value");

        dsskey_SetDsskeyExtension(*iter, "ext");
        dsskey_SetDsskeyLabel(*iter, "label");


        logicData.clear();
        str = dsskey_GetDsskeyValue(*iter);

        EXPECT_STREQ("value", str.c_str());


        str1 = dsskey_GetDsskeyLabel(*iter);

        EXPECT_STREQ("label", str1.c_str());


        str2 = dsskey_GetDsskeyExtension(*iter);

        EXPECT_STREQ("ext", str2.c_str());

        dsskey_GetData(*iter, logicData);

        EXPECT_STREQ("value", logicData.strValue.c_str());

        EXPECT_STREQ("label", logicData.strLabel.c_str());
        EXPECT_STREQ("ext", logicData.strExtension.c_str());
    }
}


TEST(dsskey, dsskey_IsBindType)
{

    int dt = DT_NA;
    for (; dt != DT_LAST; ++dt)
    {
        dsskey_IsBindType((DssKey_Type)dt);
    }

}


TEST(dsskey, dsskey_GetMaxExpNum)
{
    int  amount = dsskey_GetMaxExpNum();
    EXPECT_NE(0, amount);
}


TEST(dsskey, dsskey_GetDsskeyID)
{
    dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, 0);
}


TEST(dsskey, dsskey_RefressDsskeyByType)
{
    int dt = DT_NA;
    for (; dt != DT_LAST; dt++)
    {
        dsskey_RefressDsskeyByType((DssKey_Type)dt);
    }
}






//获取呼出号码
TEST(dsskey, dsskey_GetCallOutNumber)
{
    yl::string strOutput;
    bool bRet = dsskey_GetCallOutNumber(-1, strOutput);

    EXPECT_EQ(false, bRet);
}


TEST(dsskey, dsskey_GetDsskeyLine)
{
    int line = dsskey_GetDsskeyLine(-1);


}


TEST(dsskey, dsskey_IsDsskey)
{
    bool bRet = dsskey_IsDsskey(-1);

    EXPECT_EQ(false, bRet);
}


TEST(dsskey, dssKey_GetEXPTypeByIndex)
{

}



TEST(dsskey, dsskey_OnExtendedPress)
{

}


TEST(dsskey, dsskey_OnTimer)
{

}



bool bUICalled = false;
bool ondsskey_update(int dsskeyID, Dsskey_Status dsskeyStatus, const DssKeyLogicData * logicData,
                     void * pExtraData)
{
    printf("ondsskey_update %d %d %p %p \n", dsskeyID, dsskeyStatus, logicData, pExtraData);
    bUICalled = true;
    return true;
}

TEST(dsskey, dsskey_setUpdateFunCallback)
{
    dsskey_setUpdateFunCallback(ondsskey_update);
    dsskey_refreshDSSKeyUI(-1);
    dsskey_refreshDSSKeyUI(0);
    dsskey_refreshDSSKeyUI(1);

    YLList<int> dsskeyList;
    dsskey_GetAllDsskeyDataByModule(dsskeyList, DMT_LINEKEY_MODULE);
    YLList<int>::ListIterator iter = dsskeyList.begin();

    for (; iter != dsskeyList.end(); iter++)
    {
        bool bret = dsskey_refreshDSSKeyUI(*iter);
    }
}


//获取programekey链表,
TEST(dsskey, dsskey_getProgrameKeyList)
{

}
#endif

#include "dsskeydataentrymanager.h"

TEST(dsskey, dsskey_GetDsskeyByKeyIndex)
{
    int iProModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_PROGRAMEKEY_MODULE);
    DSSKEY_INFO("iProModuleNum[%d]", iProModuleNum);
    for (int i = 0; i < iProModuleNum; ++i)
    {
        EXPECT_EQ(dsskey_GetDsskeyID(DMT_PROGRAMEKEY_MODULE, 0, i), dsskey_GetDsskeyByKeyIndex(i));
    }

    int iLineModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_LINEKEY_MODULE);
    DSSKEY_INFO("iLineModuleNum[%d]", iLineModuleNum);
    for (int i = 0; i < iLineModuleNum; ++i)
    {
        EXPECT_EQ(dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, i),
                  dsskey_GetDsskeyByKeyIndex(iProModuleNum + i));
    }

    int iMemModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_MEMKEY_MODULE);
    DSSKEY_INFO("iMemModuleNum[%d]", iMemModuleNum);
    for (int i = 0; i < iMemModuleNum; ++i)
    {
        EXPECT_EQ(dsskey_GetDsskeyID(DMT_MEMKEY_MODULE, 0, i),
                  dsskey_GetDsskeyByKeyIndex(iProModuleNum + iLineModuleNum + i));
    }

    for (int i = 0; i < 6; ++i)
    {
        _DsskeyManager.ResetModuleDsskeyData(DMT_EXP40_MODULE, i);
    }

    int iModuleNumSum = iProModuleNum + iLineModuleNum + iMemModuleNum;
    /*  for (int i=0; i<MAX_EXP_NUM; ++i)*/
    for (int i = 0; i < dsskey_GetEXPNum(); ++i)
    {
        int iExpModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_EXP40_MODULE, i);
        DSSKEY_INFO("iExpModuleNum[%d]", iExpModuleNum);
        for (int j = 0; j < iExpModuleNum; ++j)
        {
            EXPECT_EQ(dsskey_GetDsskeyID(DMT_EXP40_MODULE, i, j),
                      dsskey_GetDsskeyByKeyIndex(iModuleNumSum + j));
        }
        iModuleNumSum += iExpModuleNum;
    }

}

TEST(dsskey, dsskey_GetDsskeyIndexByID)
{
    int iProModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_PROGRAMEKEY_MODULE);
    DSSKEY_INFO("iProModuleNum[%d]", iProModuleNum);
    for (int i = 0; i < iProModuleNum; ++i)
    {
        EXPECT_EQ(i, dsskey_GetDsskeyIndexByID(dsskey_GetDsskeyID(DMT_PROGRAMEKEY_MODULE, 0, i)));
    }

    int iLineModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_LINEKEY_MODULE);
    DSSKEY_INFO("iLineModuleNum[%d]", iLineModuleNum);
    for (int i = 0; i < iLineModuleNum; ++i)
    {
        EXPECT_EQ(iProModuleNum + i, dsskey_GetDsskeyIndexByID(dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0,
                  i)));
    }

    int iMemModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_MEMKEY_MODULE);
    DSSKEY_INFO("iMemModuleNum[%d]", iMemModuleNum);
    for (int i = 0; i < iMemModuleNum; ++i)
    {
        EXPECT_EQ(iProModuleNum + iLineModuleNum + i,
                  dsskey_GetDsskeyIndexByID(dsskey_GetDsskeyID(DMT_MEMKEY_MODULE, 0, i)));
    }

    int iModuleNumSum = iProModuleNum + iLineModuleNum + iMemModuleNum;
    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        int iExpModuleNum = _DsskeyManager.GetModuleDsskeyNum(DMT_EXP40_MODULE, i);
        DSSKEY_INFO("iExpModuleNum[%d]", iExpModuleNum);
        for (int j = 0; j < iExpModuleNum; ++j)
        {
            EXPECT_EQ(iModuleNumSum + j, dsskey_GetDsskeyIndexByID(dsskey_GetDsskeyID(DMT_EXP40_MODULE, i, j)));
        }
        iModuleNumSum += iExpModuleNum;
    }
}

#endif

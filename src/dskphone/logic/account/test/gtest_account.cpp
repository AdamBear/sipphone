#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <ETLLib.hpp>
#include <netmsgtypedefine.h>
#include <configiddefine.h>
#include <service_config.h>

#include <dsklog/log.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include <cfgservice/service_config.h>
#include <devicelib/phonedevice.h>

#include "modaccount.h"
#include "accountmanager.h"

#define ACCOUNT_TEST_THREAD "test_account"

#define TEST_MESSAGE_PRINT(msg, w, l ) \
    printf("Send Message "#msg"["#w"]["#l"]\n");\
    printf("Message["#msg"] Reply: [%d]\n", msgSendMsgToThread(msgGetThreadByName(ACCOUNT_TEST_THREAD), msg, w, l))

#define TEST_MESSAGE(msg, w, l ) msgSendMsgToThread(msgGetThreadByName(ACCOUNT_TEST_THREAD), msg, w, l)

/*****************
模拟函数 Start
****************/
void commonUnits_ResetToFatory(ResetOptionType eOption) {}


LRESULT inline SendToAccount(UINT uMsg, WPARAM wParam, LPARAM lParam, int ex_size = 0,
                             void * ex = NULL)
{
    msgObject msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.ResetExtraData(ex_size, ex);
    return acc_AccountOnMessage(msg);
}
//刷新账号配置
//开关账号 Enable/Disable
void inline reconfig_account(int iAccount)
{
    SendToAccount(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccount);
}

//开关账号 Enable/Disable
void inline turn_account(int iAccount, bool bEnable)
{
    configParser_SetCfgItemIntValue(kszAccountEnable, iAccount, bEnable, CONFIG_LEVEL_PHONE);
    reconfig_account(iAccount);
}

//重置账号，Disable所有账号
void inline reset_account()
{
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        turn_account(i, false);
    }
}


//注册账号
void inline register_account(int iAccount)
{
    if (acc_IsAccountEnable(iAccount) == false)
    {
        turn_account(iAccount, true);
    }
    SendToAccount(SIP_REG_UPDATE_REGISTER, iAccount, LS_REGISTERED);
}

//注册账号失败
void inline faild_account(int iAccount, int faildid = -1, const yl::string & info = "")
{
    char szBuff[256] = {0};
    sprintf(szBuff, "<info failid=\"%d\" failreason=\"%s\"/>", faildid, info.c_str());
    SendToAccount(
        SIP_REG_UPDATE_REGISTER,
        iAccount,
        LS_REGISTER_FAIL,
        strlen(szBuff) + 1,
        szBuff);
}

void inline set_voice_mail(int iAccount, int iReaded, int iUnreaded)
{
    sip_mwi_info_t mm;
    memset((void *)&mm, 0, sizeof(mm));
    mm.old_msg = iReaded;

    SendToAccount(
        SIP_MSG_UPDATE_MWI_INFO, iAccount, iUnreaded, sizeof(mm), &mm);
}

/*****************
模拟函数 End
****************/


/***************************
Test Start
****************************/

TEST(AccountTest, GetAccountNum)
{
    char phoneAccountNum[1000];
    phoneAccountNum[19] = 1;
    phoneAccountNum[21] = 2;
    phoneAccountNum[23] = 3;
    phoneAccountNum[42] = 12;
    phoneAccountNum[46] = 16;
    phoneAccountNum[48] = 16;

    EXPECT_EQ(phoneAccountNum[devicelib_GetPhoneType()],
              acc_AccountNum()) << "PhoneType = " << devicelib_GetPhoneType();
}

TEST(AccountTest, GetState)
{
    reset_account();
    EXPECT_EQ(LS_DISABLE, acc_GetState(0));

    turn_account(0, true);
    register_account(0);
    EXPECT_EQ(LS_REGISTERED, acc_GetState(0));

    EXPECT_EQ(LS_UNKNOW, acc_GetState(-1));
    EXPECT_EQ(LS_UNKNOW, acc_GetState(7));
}

TEST(AccountTest, RegNumTest)
{
    reset_account();
    EXPECT_EQ(0, acc_GetRegisteredAccountNum());

    turn_account(0, true);
    register_account(0); //一个账号注册
    EXPECT_EQ(1, acc_GetRegisteredAccountNum());

    turn_account(1, true);
    register_account(1); //多个账号注册
    EXPECT_EQ(2, acc_GetRegisteredAccountNum());

    turn_account(1, false); //注销一个账号
    EXPECT_EQ(1, acc_GetRegisteredAccountNum());

    turn_account(7, false); //注销无效账号
    EXPECT_EQ(1, acc_GetRegisteredAccountNum());

    turn_account(1, false); //注销未注册的账号
    EXPECT_EQ(1, acc_GetRegisteredAccountNum());
}

TEST(AccountTest, GetDefIDTest)
{
    reset_account();
    EXPECT_EQ(-1, acc_GetDefaultAccount());

    turn_account(0, true);
    register_account(0);
    EXPECT_EQ(0, acc_GetDefaultAccount());

    turn_account(1, true);
    register_account(1);
    EXPECT_EQ(0, acc_GetDefaultAccount());

    turn_account(0, false);
    EXPECT_EQ(1, acc_GetDefaultAccount());

    turn_account(1, false);
    EXPECT_EQ(-1, acc_GetDefaultAccount());

    turn_account(1, true);
    register_account(1);
    EXPECT_EQ(1, acc_GetDefaultAccount());
}

TEST(AccountTest, SetDefIDTest)
{
    configParser_SetConfigInt(kszAccountDefaultID, 0, CONFIG_LEVEL_PHONE);
    reset_account();
    turn_account(0, true);
    register_account(0);
    turn_account(1, true);
    register_account(1);

    acc_SetDefaultAccount(1);
    EXPECT_EQ(1, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(1);
    EXPECT_EQ(1, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(-1);
    EXPECT_EQ(1, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(1, true);
    EXPECT_EQ(1, acc_GetDefaultAccount());
    EXPECT_EQ(1, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(0, true);
    EXPECT_EQ(0, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(0, true);
    EXPECT_EQ(0, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(-1, true);
    EXPECT_EQ(0, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    turn_account(1, false);
    acc_SetDefaultAccount(1);
    EXPECT_EQ(0, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));

    acc_SetDefaultAccount(1, true);
    EXPECT_EQ(0, acc_GetDefaultAccount());
    EXPECT_EQ(0, configParser_GetConfigInt(kszAccountDefaultID));
}

TEST(AccountTest, SwitchDefAccTest)
{
    configParser_SetConfigInt(kszAccountDefaultID, 0, CONFIG_LEVEL_PHONE);
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(0);


    acc_SwitchDefaultAccount();
    EXPECT_EQ(0, acc_GetDefaultAccount());

    register_account(1);
    acc_SwitchDefaultAccount();
    EXPECT_EQ(1, acc_GetDefaultAccount());

    if (acc_AccountNum() >= 3)
    {
        register_account(2);
        acc_SetDefaultAccount(2);
        acc_SwitchDefaultAccount();
        EXPECT_EQ(0, acc_GetDefaultAccount());

        acc_SetDefaultAccount(1);
        acc_SwitchDefaultAccount();
        EXPECT_EQ(2, acc_GetDefaultAccount());

        acc_SetDefaultAccount(1);
        acc_SwitchDefaultAccount(false);
        EXPECT_EQ(0, acc_GetDefaultAccount());
    }
    else
    {
        acc_SwitchDefaultAccount();
        EXPECT_EQ(0, acc_GetDefaultAccount());
    }
}

TEST(AccountTest, GetUseAccTest)
{
    reset_account();
    turn_account(0, true);
    turn_account(1, true);

    register_account(0);
    EXPECT_EQ(0, acc_GetUsableAccount());

    register_account(1);
    acc_SetDefaultAccount(1);
    EXPECT_EQ(1, acc_GetUsableAccount());

    EXPECT_EQ(0, acc_GetUsableAccount(true));

    reset_account();
    EXPECT_EQ(-1, acc_GetUsableAccount());
}

TEST(AccountTest, AccAvailableTest)
{
    reset_account();
    EXPECT_EQ(false, acc_ExistAvailableAccount());

    turn_account(0, true);
    faild_account(0);
    EXPECT_EQ(false, acc_ExistAvailableAccount());

    register_account(0);
    EXPECT_EQ(true, acc_ExistAvailableAccount());

    turn_account(0, false);
    EXPECT_EQ(false, acc_ExistAvailableAccount());
}

TEST(AccountTest, PickUpCodeTest)
{
//TODO
    configParser_SetConfigString(kszDPickupCode, "GlobalDirectPickup", CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszGPickupCode, "GlobalGroupPickup", CONFIG_LEVEL_PHONE);

    configParser_SetCfgItemStringValue(kszAccountDPickupCode, 0, "AccountDirectPickup",
                                       CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountGPickupCode, 0, "", CONFIG_LEVEL_PHONE);

    EXPECT_EQ("AccountDirectPickup", acc_GetPickupCodeByType(0, PT_DIRECTED));
    EXPECT_EQ("GlobalGroupPickup", acc_GetPickupCodeByType(0, PT_GROUP));

    configParser_SetCfgItemStringValue(kszAccountDPickupCode, 0, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountGPickupCode, 0, "AccountGroupPickup",
                                       CONFIG_LEVEL_PHONE);

    EXPECT_EQ("GlobalDirectPickup", acc_GetPickupCodeByType(0, PT_DIRECTED));
    EXPECT_EQ("AccountGroupPickup", acc_GetPickupCodeByType(0, PT_GROUP));
}

TEST(AccountTest, GetIDByNameTest)
{
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "Account", CONFIG_LEVEL_PHONE);
    EXPECT_EQ(0, acc_GetAccountIDByName("Account"));

    EXPECT_EQ(-1, acc_GetAccountIDByName("InvalidAccount"));

    EXPECT_EQ(-1, acc_GetAccountIDByName("InvalidAccount", "InvalidServer"));

    configParser_SetCfgItemStringValue(kszAccountUserName, 1, "Account", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 0, 1, "Server1", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 1, 1, "Server2", CONFIG_LEVEL_PHONE);
    turn_account(0, true);
    turn_account(1, true);
    //需要填坑，配置项多项可变参数

    EXPECT_EQ(0, acc_GetAccountIDByName("Account", "Server1")) << "Account[0] Server0 =";
    EXPECT_EQ(1, acc_GetAccountIDByName("Account", "Server2"));
}

TEST(AccountTest, GetAccTextTest)
{
    configParser_SetCfgItemStringValue(kszAccountLabel, 0, "Label", CONFIG_LEVEL_PHONE);
    EXPECT_EQ("Label", acc_GetAccountShowText(0));

    configParser_SetCfgItemStringValue(kszAccountLabel, 0, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountDisplayName, 0, "DisplayName", CONFIG_LEVEL_PHONE);
    EXPECT_EQ("DisplayName", acc_GetAccountShowText(0));


    //最优先显示Custom
    acc_SetCustomShowText(0, "CustomName");
    EXPECT_EQ("CustomName", acc_GetAccountShowText(0));
    //撤销Custom
    acc_SetCustomShowText(0, "");

    configParser_SetCfgItemStringValue(kszAccountDisplayName, 0, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountRegisterName, 0, "RegisterName", CONFIG_LEVEL_PHONE);
    EXPECT_EQ("RegisterName", acc_GetAccountShowText(0));

    configParser_SetCfgItemStringValue(kszAccountRegisterName, 0, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "UserName", CONFIG_LEVEL_PHONE);
    EXPECT_EQ("UserName", acc_GetAccountShowText(0));

    EXPECT_EQ("", acc_GetAccountShowText(-1));
}

TEST(AccountTest, VoiceMailTest)
{
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    //Unread
    register_account(0);
    set_voice_mail(0, 0, 5);
    EXPECT_EQ(5, acc_GetUnReadVoiceMailNum(0));

    EXPECT_EQ(-1, acc_GetUnReadVoiceMailNum(9));

    register_account(1);
    set_voice_mail(1, 0, 6);
    EXPECT_EQ(11, acc_GetUnReadVoiceMailNum());

    turn_account(1, false);
    EXPECT_EQ(5, acc_GetUnReadVoiceMailNum());


    //Readed
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(0);
    register_account(1);
    set_voice_mail(0, 5, 0);
    EXPECT_EQ(5, acc_GetReadedVoiceMailNum(0));

    EXPECT_EQ(-1, acc_GetReadedVoiceMailNum(9));

    register_account(1);
    set_voice_mail(1, 6, 0);
    EXPECT_EQ(11, acc_GetReadedVoiceMailNum());

    turn_account(1, false);
    EXPECT_EQ(5, acc_GetReadedVoiceMailNum());

    //LastID
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(0);
    register_account(1);
    EXPECT_EQ(-1, acc_GetLastVMailAccountId());

    set_voice_mail(0, 0, 1);
    EXPECT_EQ(0, acc_GetLastVMailAccountId());

    set_voice_mail(1, 0, 2);
    EXPECT_EQ(1, acc_GetLastVMailAccountId());

    reset_account();
    EXPECT_EQ(-1, acc_GetLastVMailAccountId());
}

TEST(AccountTest, AccRegFaildTest)
{
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(0);
    EXPECT_EQ(0, acc_GetRegisterFailId(0));

    faild_account(0, 403, "Forbiden.");
    EXPECT_EQ(403, acc_GetRegisterFailId(0));

    EXPECT_EQ(0, acc_GetRegisterFailId(9));

    reset_account();
    register_account(0);
    EXPECT_EQ("", acc_GetRegisterFailInfo(0));

    faild_account(0, 403, "Forbiden.");
    EXPECT_EQ("Forbiden.", acc_GetRegisterFailInfo(0));

    EXPECT_EQ("", acc_GetRegisterFailInfo(9));
}

TEST(AccountTest, AccEnableTest)
{
    reset_account();
    EXPECT_EQ(false, acc_IsAccountEnable(0));

    turn_account(0, true);
    EXPECT_EQ(true, acc_IsAccountEnable(0));

    EXPECT_EQ(false, acc_IsAccountEnable(-1));
}

TEST(AccountTest, AccServer)
{
    reset_account();
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 0, 1, "Server", CONFIG_LEVEL_PHONE);
    turn_account(0, true);
    EXPECT_EQ("Server", acc_GetServerName(0));

    char szServerName[] = "BackServerName";
    SendToAccount(
        SIP_REG_UPDATE_SIP_SERVER,
        0,
        0,
        sizeof(szServerName),
        szServerName
    );
    EXPECT_EQ(szServerName, acc_GetServerName(0));

    EXPECT_EQ("", acc_GetServerName(9));
}

TEST(AccountTest, GetIdByServer)
{
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 0, 1, "Server1", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 1, 1, "Server2", CONFIG_LEVEL_PHONE);
    reconfig_account(0);
    reconfig_account(1);

    EXPECT_EQ(0, acc_GetAccountIDByServer("Server1"));
    EXPECT_EQ(1, acc_GetAccountIDByServer("Server2"));
    EXPECT_EQ(-1, acc_GetAccountIDByServer("ServerInvalid"));
}

TEST(AccountTest, GetIdByFullName)
{
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 0, 1, "Server1", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "Account1", CONFIG_LEVEL_PHONE);
    reconfig_account(0);

    EXPECT_EQ(0, acc_GetAccountIDByFullName("Account1@Server1")); //#
    EXPECT_EQ(-1, acc_GetAccountIDByFullName("AccontInvaild@ServerInvaild"));
}

TEST(AccountTest, GetIdByInfo)
{
    BasicUserInfo user;
    memset((void *)&user, 0, sizeof(user));
    user.strNumber = "Account1";
    user.strServer = "Server1";
    EXPECT_EQ(0, acc_GetAccountIDByUserInfo(&user));
}

void clear_server()
{
    //重置所有账号服务器值
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        for (int j = 1; j <= SIPSERVER_NUM; ++j)
        {
            configParser_SetCfgItemStringValue(kszAccountServerAddr, i, j, "", CONFIG_LEVEL_PHONE);
        }
        configParser_SetCfgItemStringValue(kszAccountSIPServerHost, i, "", CONFIG_LEVEL_PHONE);
        reconfig_account(i);
    }
}

TEST(AccountTest, GetNoEmptyServerAccount)
{
    clear_server();

    EXPECT_EQ(-1, acc_GetNoEmptyServerAccountID());

    //账号1有值
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 0, 1, "Server1", CONFIG_LEVEL_PHONE);
    reconfig_account(0);
    EXPECT_EQ(0, acc_GetNoEmptyServerAccountID());

    //账号2有值
    clear_server();
    reconfig_account(0);
    configParser_SetCfgItemStringValue(kszAccountServerAddr, 1, 1, "Server2", CONFIG_LEVEL_PHONE);
    reconfig_account(1);
    EXPECT_EQ(1, acc_GetNoEmptyServerAccountID()) << "Acount1.Server = " << acc_GetServerName(
                0).c_str(); //#
}

TEST(AccountTest, GetFirstUserNameAccount)
{
    reset_account();
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        configParser_SetCfgItemStringValue(kszAccountUserName, i, "", CONFIG_LEVEL_PHONE);
    }
    EXPECT_EQ(-1, acc_GetFirstUserNameAccount());

    turn_account(0, true);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "Username", CONFIG_LEVEL_PHONE);
    EXPECT_EQ(0, acc_GetFirstUserNameAccount());

    turn_account(1, true);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountUserName, 1, "Username", CONFIG_LEVEL_PHONE);
    EXPECT_EQ(1, acc_GetFirstUserNameAccount());

    turn_account(0, false);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "Username", CONFIG_LEVEL_PHONE);
    EXPECT_EQ(1, acc_GetFirstUserNameAccount());
}


TEST(AccountTest, IsAccountAvailable)
{
    reset_account();
    EXPECT_EQ(false, acc_IsAccountAvailable(0));

    turn_account(0, true);
    EXPECT_EQ(false, acc_IsAccountAvailable(0));

    register_account(0);
    EXPECT_EQ(true, acc_IsAccountAvailable(0));

    EXPECT_EQ(false, acc_IsAccountAvailable(9));
}

TEST(AccountTest, MSG_WEB2PHONE)
{
    const unsigned WEB2PHONE_QUERY_ACCOUNT_STATE = 0x30006;
    const unsigned WEB2PHONE_QUERY_ACCOUNT_ALLSTATE = 0x30005;

    remove("/tmp/tmpAllAccountState");
    SendToAccount(WEB2PHONE_QUERY_ACCOUNT_ALLSTATE, 0, 0);
    EXPECT_EQ(0, access("/tmp/tmpAllAccountState", F_OK));

    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        EXPECT_EQ(acc_GetState(i), SendToAccount(WEB2PHONE_QUERY_ACCOUNT_STATE, i, 0))
                << "Account ID is [" << i << "].";
    }
}

TEST(AccountTest, GetDirectIPAccount)
{
    reset_account();
    EXPECT_NE(-1, acc_GetDirectIPAccountID());
}

TEST(AccountTest, GetHetlingAccount)
{
    reset_account();
    EXPECT_EQ(-1, acc_GetHetlingAccountID());

    configParser_SetCfgItemIntValue(kszAccountEnableHoteling, 0, 1, CONFIG_LEVEL_PHONE);
    turn_account(0, true);
    register_account(0);
    EXPECT_EQ(0, acc_GetHetlingAccountID());

    configParser_SetCfgItemIntValue(kszAccountEnableHoteling, 0, 0, CONFIG_LEVEL_PHONE);
}

TEST(AccountTest, GetACDAccount)
{
    reset_account();
    EXPECT_EQ(-1, acc_GetAcdAccountID());

    configParser_SetCfgItemIntValue(kszAccountEnableACD, 0, 1, CONFIG_LEVEL_PHONE);
    turn_account(0, true);
    register_account(0);
    EXPECT_EQ(0, acc_GetAcdAccountID());

    configParser_SetCfgItemIntValue(kszAccountEnableACD, 0, 0, CONFIG_LEVEL_PHONE);
}

TEST(AccountTest, GetValidAccountList)
{
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(1);
    YLList<int> validAccounts = acc_GetAvailableAccountList();
    EXPECT_EQ(acc_GetRegisteredAccountNum(), validAccounts.size());

    for (YLList<int>::iterator pos = validAccounts.begin(); pos != validAccounts.end(); ++pos)
    {
        EXPECT_EQ(LS_REGISTERED, acc_GetState((*pos))) << "Account ID " << (*pos);
    }

    register_account(0);
    validAccounts = acc_GetAvailableAccountList();
    EXPECT_EQ(acc_GetRegisteredAccountNum(), validAccounts.size());

    for (YLList<int>::iterator pos = validAccounts.begin(); pos != validAccounts.end(); ++pos)
    {
        EXPECT_EQ(LS_REGISTERED, acc_GetState((*pos))) << "Account ID " << (*pos);
    }
}

TEST(AccountTest, GetDefaultCallMode)
{
    configParser_SetCfgItemIntValue(kszAccountCallMode, 0, CAVT_UNKNOWN, CONFIG_LEVEL_PHONE);
    EXPECT_EQ(CAVT_VIDEO, acc_GetDefaultCallMode(0));

    configParser_SetCfgItemIntValue(kszAccountCallMode, 0, CAVT_MIX, CONFIG_LEVEL_PHONE);
    EXPECT_EQ(CAVT_MIX, acc_GetDefaultCallMode(0));
}

TEST(AccountTest, HotdeskInterface)
{
    reset_account();
    turn_account(0, true);
    turn_account(1, true);
    register_account(1);
    register_account(0);

    EXPECT_EQ(2, acc_GetRegisteredAccountNum());
    acc_EraseAllAccount();
    EXPECT_EQ(0, acc_GetRegisteredAccountNum());

    configParser_SetConfigInt(kszAccountIsHotDeskAccount, 1, CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "UserName", CONFIG_LEVEL_PHONE);
    acc_EmptyAllAccount();
    EXPECT_EQ("", configParser_GetCfgItemStringValue(kszAccountUserName, 0));

    acc_LoadAccount(0);
    turn_account(0, true);
    register_account(0);
    EXPECT_EQ(LS_REGISTERED, acc_GetState(0));

    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        acc_LoadAccount(i);
    }

}

TEST(AccountTest, LogoutBeforeReset)
{
    turn_account(0, true);
    turn_account(1, true);

    acc_LogoutBeforeReset();

    EXPECT_EQ(false, acc_IsAccountEnable(0));
    EXPECT_EQ(false, acc_IsAccountEnable(1));
}

TEST(AccountTest, GetAccountInfo)
{
    yl::string strInfo;
    reset_account();
    turn_account(0, true);
    configParser_SetCfgItemStringValue(kszAccountLabel, 0, "Label", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountDisplayName, 0, "DisplayName", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszAccountUserName, 0, "UserName", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemIntValue(kszAnonymousCallSwitch, 0, 1, CONFIG_LEVEL_PHONE);

    acc_GetAccountInfo(strInfo);
    yl::string::size_type npos = yl::string::npos;
    EXPECT_NE(npos, strInfo.find("Account0=Enabled|Label|UserName|DisplayName|On|"));
}

/***************************
Test End
****************************/



/******************************
Boot Start
*******************************/
/*************************************************
gcov 在程序退出时生成数据文件
进入msgkit系统的程序无法正常退出，怎么办？
gcov使用__gcov_flush函数刷新.gcna文件，程序中直接调用。
在源代码中直接声明该函数，C++代码中注意extern。
extern "C" void __gcov_flush(void);
在需要的地方调用。
***************************************************/
#if IS_COVER_TEST
extern "C" void __gcov_flush(void);
#endif

class EndExit : public testing::Environment
{
public:
    //该函数在所有用例执行完好执行
    virtual void SetUp()
    {

    }
    //该函数在所有用例执行完后执行
    virtual void TearDown()
    {
#if IS_COVER_TEST
        //刷新gcov，生成gcna
        __gcov_flush();
#endif
    }
};


LRESULT CALLBACK account_test_msg(msgObject * pMessage)
{
    switch (pMessage->message)
    {
    case TM_INIT:
        devicelib_Init(false);
        acc_Init();

        //初始化后调用RUN_ALL_TESTS运行测试用例
        return RUN_ALL_TESTS();
        break;
    default:
        break;
    };
    return acc_AccountOnMessage(*pMessage);
}

int main(int argc, char * argv[])
{
    //设置gtest环境
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new EndExit);

    //进入msgkit消息系统
    mkit_handle_t accountTest = mkRegistThread(ACCOUNT_TEST_THREAD, account_test_msg);
    mkRunThread(accountTest);
    return 0;
}

/******************************
Boot End
*******************************/



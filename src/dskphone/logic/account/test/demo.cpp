#include "modaccount.h"


int main()
{
    //初始化Account模块
    acc_Init();

    //获取默认账号ID，目前有通过服务器名获取，注册名获取，获取第一个可用账号或者默认账号来获取ID
    //ID的值既为账号的LineID
    int nAccount = acc_GetDefaultAccount();

    //判断ID是否有效，acc全部接口对AccountID有容错，如果没有必要，可以跳过此步
    bool IsValid = acc_IsAccountIDValid(nAccount);

    //开始获取账号信息，带有状态的信息和部分常用信息直接提供接口，不常用信息可以通过configParser的接口直接读取配置
    //获取服务器名
    yl::string ServerName = acc_GetServerName(nAccount);
    //获取Label
    yl::string Label = acc_GetLabel(nAccount);
    //获取账号状态
    LINE_STATE eState = acc_GetState(nAccount);
    /*
    目前账号信息接口还有：
    注册名                          acc_GetRegisterName
    显示名                          acc_GetDisplayName
    用户名                          acc_GetUsername
    BLA类型                         acc_GetBLAType
    同步状态                        acc_GetSyncSubscribe
    注册失败ID                      acc_GetRegisterFailId
    最后收到VoicMail的AccoutID      acc_GetLastVMailAccountId
    未读VoiceMail条数               acc_GetUnReadVoiceMailNum
    已读VoiceMail条数               acc_GetReadedVoiceMailNum
    账号显示的名称                  acc_GetAccountShowText
    */

    //默认账号操作
    //设定默认账号（不写入配置）
    acc_SetDefaultAccount(nAccount);
    //设定默认账号，并写入配置
    acc_SetDefaultAccount(nAccount, true);

    //切换默认账号到下一个可用账号
    acc_SwitchDefaultAccount();
    //切换默认账号的上一个可用账号
    acc_SwitchDefaultAccount(true);

    return 0;
}

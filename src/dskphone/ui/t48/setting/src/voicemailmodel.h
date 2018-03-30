#ifndef SETTING_VOICEMAILMODEL_H_
#define SETTING_VOICEMAILMODEL_H_

struct VoiceMailConfigData;
struct VoiceMailConfigListData;
struct VoiceMailInfoData;
struct VoiceMailInfoListData;

namespace NS_SETTING
{
// Class CVoiceMailModel, 记录Voice mail的配置.
class CVoiceMailModel
{
public:
    // 获取Voice Mail配置项列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetConfigList(VoiceMailConfigListData * pData) const;

    // 设置Voice Mail配置项列表.
    // | refData | 是传入的配置列表.
    void SetConfigList(VoiceMailConfigListData & refData);

    // 获取所有账号当前有的Voice Mail列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetInfoList(VoiceMailInfoListData * pData) const;

    // 根据下标获取某个Voice Mail信息.
    // | pData | [out] 是buffer.
    // | nAccountId | 为账号id.
    // 如果获取成功则返回true, 否则返回false.
    bool GetInfoByAccountId(VoiceMailInfoData * pData, int nAccountId) const;

    // 根据账号id获取语音信箱号码.
    // | pData | 是buffer.
    // | nAccountId | 为账号id.
    // 如果获取成功则返回true, 否则返回false.
    bool GetMailNumberByAccountId(VoiceMailConfigData * pData,
                                  int nAccountId) const;

    // 是否有未读的VoiceMail.
    // 如果有则返回true, 否则返回false.
    bool HasUnRead();

private:
    // Ctor and Dtor.
    CVoiceMailModel();
    ~CVoiceMailModel();

private:
    friend class CVoiceMailController;
};

}  // namespace NS_SETTING

#endif  // SETTING_VOICEMAILMODEL_H_

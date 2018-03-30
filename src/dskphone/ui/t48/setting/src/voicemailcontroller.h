#ifndef SETTING_VOICEMAILCONTROLLER_H_
#define SETTING_VOICEMAILCONTROLLER_H_

#include "commondefine.h"

#include "ETLLib.hpp"
#include "ETLLib/ETLMsgSystem.h"

struct VoiceMailConfigListData;
struct VoiceMailInfoListData;

namespace NS_SETTING
{
class CVoiceMailModel;

// Class CVoiceMailController.
class CVoiceMailController
{
public:
    // 获取单键.
    static CVoiceMailController * GetInstance();
    // 销毁单键.
    static void ReleaseInstance();

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

    // 是否有未读的VoiceMail
    bool HasUnRead();

    // 获取所有账号未读Voice Mail的数量
    int GetTotalUnRead() const;

    // 点击某个下标对应的Voice Mail.
    // | nIndex | 是某个下标, 从0开始.
    void ClickInfoByIndex(int nIndex);

    // Message按键处理
    bool OnMsgKeyPress();

private:
    // Ctor and Dtor.
    CVoiceMailController();
    ~CVoiceMailController();

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    bool MyInit();
    // 清除.
    // 如果清除成功则返回true, 否则返回false.
    bool MyFinal();

private:
    static CVoiceMailController * m_pInst; // 单键实例.
    CVoiceMailModel * m_pModel; // Voice Mail的Model.
    static bool m_bDelayVoiceMailTone;
    static bool m_bVoicePoping;

    DISALLOW_COPY_AND_ASSIGN(CVoiceMailController);
};

}  // namespace NS_SETTING

#define g_pVoiceMailController (NS_SETTING::CVoiceMailController::GetInstance())

#endif  // SETTING_VOICEMAILCONTROLLER_H_

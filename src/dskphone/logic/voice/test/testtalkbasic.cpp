#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "voicetestfixture.h"

#include <keymap.h>
#include "modvoice.h"
#include "voicemanager.h"

using namespace testing;

/**
 * 免提声道呼出
 * 1. idle界面点击免提键进入拨号界面
 * 2. 输入号码并呼出
 * 3. 回铃界面响回铃音
 * 4. 对方接起，建立通话
 * 5. 点击Hold，挂起通话
 * 6. 播放Hold提示音
 * 7. 点击Resume，恢复通话
 * 8. 对方挂机，结束通话，回到idle
 */
TEST_F(ModVoiceTest, HandfreeCallOut)
{
    // 1.1 点击免提键
    voice_OnKeyPress(PHONE_KEY_HANDFREE);
    // 1.2 停止铃声
    voice_StopRing();
    // 1.3 响拨号音
    voice_PlayTone(TONE_DIAL);
    // 2.1 拨号按键音
    voice_PlayKeyTone(PHONE_KEY_2);
    // 2.2 停止Tone
    voice_StopTone(TONE_DIAL);
    // 2.3 按#呼出
    voice_PlayKeyTone(PHONE_KEY_POUND);
    // 3 响回铃音
    voice_PlayTone(TONE_RINGBACK);
    // 4.1 停止Tone
    voice_StopTone();
    // 4.2 开始通话
    const int iCallID = 1;
    ipvp_speech_t objTalkParam;
    voice_StartTalk(iCallID, &objTalkParam);
    // 5 Hold
    voice_Hold(iCallID, true);
    // 6 响Hold提示音
    voice_PlayTone(TONE_HOLD_ALERT, 300, CHANNEL_HANDFREE);
    // 7 Resume
    voice_Hold(iCallID, false);
    // 8.1 结束通话
    voice_StopTalk(iCallID);
    // 8.2 切换到idle通道
    voice_SwitchChannel(CHANNEL_IDLE);
}

/**
 * 来电摘手柄接起
 * 1. 来电响铃
 * 2. 摘手柄接起，建立通话
 * 3. 对方Hold，通话被挂起
 * 4. 播放Held提示音
 * 5. 对方Resume，通话恢复
 * 6. 点击Mute，通话静音
 * 7. 再次点击Mute，解除静音，通话恢复
 * 8. 挂手柄，结束通话，回到idle
 */
TEST_F(ModVoiceTest, HandsetCallIn)
{

}

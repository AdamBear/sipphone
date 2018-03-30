#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "voicetestfixture.h"

#include "modvoice.h"
#include "voicemanager.h"

using namespace testing;

// GUI先于VPM启动
TEST_F(ModVoiceTest, Init)
{
    // 模拟GUI启动时VPM尚未启动完成
    EXPECT_CALL(m_objMockMkService, GetData(_, MKSRV_ID_IPVP, 0)).Times(1).WillOnce(Return(0));
    voice_Init();
    // VPM启动完成后voice模块初始化
    EXPECT_CALL(m_objMockMkService, GetData(_, MKSRV_ID_IPVP, 0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(m_objMockVPM, Init(_, 0)).Times(1).WillOnce(Return(0)); // VPM初始化接口
    EXPECT_CALL(m_objMockVPM, SetMode(_, IPVP_MODE_HS)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(m_objMockVPM, SetVolume(_, m_iVolumeToneHandset)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(m_objMockEtlMsgHandler, NotifyApp(_, VOICE_CHANGE_CHANNEL, CHANNEL_HANDSET, _)).Times(AtLeast(1)).WillRepeatedly(Return(TRUE)); // 通道切换消息
    EXPECT_CALL(m_objMockEtlMsgHandler, NotifyApp(_, VOICE_VPM_START, _, _)).Times(1).WillOnce(Return(TRUE)); // voice模块初始化完成消息
    // 触发VPM启动完成消息
    msgObject msg;
    msg.message = MKIT_MSG_SRVREG;
    msg.wParam = MKSRV_ID_IPVP;
    TriggerEtlMsgCallback(msg);
    // 释放资源
    EXPECT_CALL(m_objMockVPM, SetMode(_, IPVP_MODE_IDLE)).Times(1).WillOnce(Return(0));
    CVoiceManager::ReleaseInstance();
}

// GUI后于VPM启动
TEST_F(ModVoiceTest, InitAfterVPM)
{
    // 模拟GUI启动时VPM已经启动完成
    EXPECT_CALL(m_objMockMkService, GetData(_, MKSRV_ID_IPVP, 0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(m_objMockVPM, Init(_, 0)).Times(1).WillOnce(Return(0)); // VPM初始化接口
    EXPECT_CALL(m_objMockVPM, SetMode(_, IPVP_MODE_HS)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(m_objMockVPM, SetVolume(_, m_iVolumeToneHandset)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(m_objMockEtlMsgHandler, NotifyApp(_, VOICE_CHANGE_CHANNEL, CHANNEL_HANDSET, _)).Times(AtLeast(1)).WillRepeatedly(Return(TRUE)); // 通道切换消息
    EXPECT_CALL(m_objMockEtlMsgHandler, NotifyApp(_, VOICE_VPM_START, _, _)).Times(1).WillOnce(Return(TRUE)); // voice模块初始化完成消息
    voice_Init();
    // 释放资源
    EXPECT_CALL(m_objMockVPM, SetMode(_, IPVP_MODE_IDLE)).Times(1).WillOnce(Return(0));
    CVoiceManager::ReleaseInstance();
}

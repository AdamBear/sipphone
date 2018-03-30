#ifndef __MOCK_VPM_H__
#define __MOCK_VPM_H__

#include <gmock/gmock.h>

#include <service_ipvp.h>

class MockVPM
{
public:
    MOCK_METHOD2(Init, int(mkit_handle_t caller, int reset));
    MOCK_METHOD2(SetMode, int(mkit_handle_t caller, int mode));
    MOCK_METHOD4(SetSlave, int(mkit_handle_t caller, int slave, int dir, int control));
    MOCK_METHOD2(SetVolume, int(mkit_handle_t caller, int volume));
    MOCK_METHOD2(SetMute, int(mkit_handle_t caller, int mute));
    MOCK_METHOD3(DTMFPlay, int(mkit_handle_t caller, char dtmf, int sync));
    MOCK_METHOD5(DTMFSend, int(mkit_handle_t caller, int callID, char dtmf, int dir, int sync));
    MOCK_METHOD5(TonePlay2, int(mkit_handle_t caller, int toneid, int mix, unsigned short duration,
                                int sync));
    MOCK_METHOD2(ToneStop, int(mkit_handle_t caller, int sync));
    MOCK_METHOD4(AudioPlay, int(mkit_handle_t caller, const char * file, int loop, int sync));
    MOCK_METHOD2(AudioStop, int(mkit_handle_t caller, int sync));
    MOCK_METHOD3(MediaPlay2, int(mkit_handle_t caller, const char * file, int sync));
    MOCK_METHOD2(MediaStop, int(mkit_handle_t caller, int sync));
    MOCK_METHOD2(MediaPause, int(mkit_handle_t caller, int sync));
    MOCK_METHOD2(MediaResume, int(mkit_handle_t caller, int sync));
    MOCK_METHOD3(MediaSeek, int(mkit_handle_t caller, unsigned int locate, int sync));
    MOCK_METHOD3(MediaGetInfo, int(mkit_handle_t caller, ipvp_pinfo_t * info, const char * file));
    MOCK_METHOD4(SpeechStart, int(mkit_handle_t caller, int callID, ipvp_speech_t * speech, int sync));
    MOCK_METHOD3(SpeechStop, int(mkit_handle_t caller, int callID, int sync));
    MOCK_METHOD4(SpeechModify, int(mkit_handle_t caller, int callID, ipvp_speech_t * speech, int sync));
    MOCK_METHOD4(SpeechHold, int(mkit_handle_t caller, int callID, int hold, int sync));
    MOCK_METHOD3(ConferMerge, int(mkit_handle_t caller, const ipvp_confer_t * confer, int sync));
    MOCK_METHOD3(ConferSplit, int(mkit_handle_t caller, const ipvp_confer_t * confer, int sync));
};

extern MockVPM * g_pMockVPM;

#endif // !__MOCK_VPM_H__

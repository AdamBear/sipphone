#include "mockvpm.h"

MockVPM * g_pMockVPM = NULL;

int ipvp_init(mkit_handle_t caller, int reset)
{
    return g_pMockVPM ? g_pMockVPM->Init(caller, reset) : 0;
}

int ipvp_set_mode(mkit_handle_t caller, int mode)
{
    return g_pMockVPM ? g_pMockVPM->SetMode(caller, mode) : 0;
}

int ipvp_set_slave(mkit_handle_t caller, int slave, int dir, int control)
{
    return g_pMockVPM ? g_pMockVPM->SetSlave(caller, slave, dir, control) : 0;
}

int ipvp_set_volume(mkit_handle_t caller, int volume)
{
    return g_pMockVPM ? g_pMockVPM->SetVolume(caller, volume) : 0;
}

int ipvp_set_mute(mkit_handle_t caller, int mute)
{
    return g_pMockVPM ? g_pMockVPM->SetMute(caller, mute) : 0;
}

int ipvp_dtmf_play(mkit_handle_t caller, char dtmf, int sync)
{
    return g_pMockVPM ? g_pMockVPM->DTMFPlay(caller, dtmf, sync) : 0;
}

int ipvp_dtmf_send(mkit_handle_t caller, int callID, char dtmf, int dir, int sync)
{
    return g_pMockVPM ? g_pMockVPM->DTMFSend(caller, caller, dtmf, dir, sync) : 0;
}

int ipvp_tone_play2(mkit_handle_t caller, int toneid, int mix, unsigned short duration, int sync)
{
    return g_pMockVPM ? g_pMockVPM->TonePlay2(caller, toneid, mix, duration, sync) : 0;
}

int ipvp_tone_stop(mkit_handle_t caller, int sync)
{
    return g_pMockVPM ? g_pMockVPM->ToneStop(caller, sync) : 0;
}

int ipvp_mcu_audio_play(mkit_handle_t caller, int gid, const char *file, int loop, int sync)
{
    return g_pMockVPM ? g_pMockVPM->AudioPlay(caller, file, loop, sync) : 0;
}

int ipvp_mcu_audio_stop(mkit_handle_t caller, int gid, int sync)
{
    return g_pMockVPM ? g_pMockVPM->AudioStop(caller, sync) : 0;
}

int ipvp_mcu_media_play2(mkit_handle_t caller, int gid, const char *file, int sync)
{
    return g_pMockVPM ? g_pMockVPM->MediaPlay2(caller, file, sync) : 0;
}

int ipvp_mcu_media_stop(mkit_handle_t caller, int gid, int sync)
{
    return g_pMockVPM ? g_pMockVPM->MediaStop(caller, sync) : 0;
}

int ipvp_mcu_media_pause(mkit_handle_t caller, int gid, int sync)
{
    return g_pMockVPM ? g_pMockVPM->MediaPause(caller, sync) : 0;
}

int ipvp_mcu_media_resume(mkit_handle_t caller, int gid, int sync)
{
    return g_pMockVPM ? g_pMockVPM->MediaResume(caller, sync) : 0;
}

int ipvp_mcu_media_seek(mkit_handle_t caller, int gid, unsigned int locate, int sync)
{
    return g_pMockVPM ? g_pMockVPM->MediaSeek(caller, locate, sync) : 0;
}

int ipvp_mcu_media_getinfo(mkit_handle_t caller, int gid, ipvp_pinfo_t *info, const char* file)
{
    return g_pMockVPM ? g_pMockVPM->MediaGetInfo(caller, info, file) : 0;
}

int ipvp_mcu_audio_start(mkit_handle_t caller, int callID, int gid, ipvp_speech_t *speech, int sync)
{
    return g_pMockVPM ? g_pMockVPM->SpeechStart(caller, callID, speech, sync) : 0;
}

int ipvp_speech_stop(mkit_handle_t caller, int callID, int sync)
{
    return g_pMockVPM ? g_pMockVPM->SpeechStop(caller, callID, sync) : 0;
}

int ipvp_speech_modify(mkit_handle_t caller, int callID, ipvp_speech_t * speech, int sync)
{
    return g_pMockVPM ? g_pMockVPM->SpeechModify(caller, callID, speech, sync) : 0;
}

int ipvp_speech_hold(mkit_handle_t caller, int callID, int hold, int sync)
{
    return g_pMockVPM ? g_pMockVPM->SpeechHold(caller, callID, hold, sync) : 0;
}

int ipvp_mcu_audio_merge(mkit_handle_t caller, int gid, const ipvp_confer_t *confer, int sync)
{
    return g_pMockVPM ? g_pMockVPM->ConferMerge(caller, confer, sync) : 0;
}

int ipvp_mcu_audio_split(mkit_handle_t caller, int gid, const ipvp_confer_t *confer, int sync)
{
    return g_pMockVPM ? g_pMockVPM->ConferSplit(caller, confer, sync) : 0;
}

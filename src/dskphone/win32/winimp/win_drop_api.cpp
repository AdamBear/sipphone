#include "config.h"

#include <cstdio>
#include <cstring>
#include <dsklog/log.h>

#define TRACE_FUCTION_NORETURN \
    { do \
    {\
        printf("[%s]:[%d]\n", __FUNCTION__, __LINE__ )  ; \
    } while (0 ) ;  }

#define TRACE_FUCTION \
    { TRACE_FUCTION_NORETURN ; return 0 ; }

/***********************************************ipvp************************************************/
#include "service_ipvp.h"
#include "service_ipvp_api.h"

const char * ipvp_version(void) TRACE_FUCTION
const char * ipvp_build(void) TRACE_FUCTION
int ipvp_dev_init(int * samplerate, int * chan, int * Hz) TRACE_FUCTION
int ipvp_dev_exit(void) TRACE_FUCTION
int ipvp_dev_framing(void * spk[], void * mic[], void * ref[], size_t ch, size_t fs) TRACE_FUCTION
int ipvp_dev_surface(int id, int active, void * surface, void * extra) TRACE_FUCTION
int ipvp_lib_init(ipvp_init_t * init, size_t init_size) TRACE_FUCTION
int ipvp_lib_exit(void) TRACE_FUCTION
int ipvp_init(mkit_handle_t caller, int reset) TRACE_FUCTION
int ipvp_exit(mkit_handle_t caller) TRACE_FUCTION
int ipvp_set_power(mkit_handle_t caller, int power_level) TRACE_FUCTION
int ipvp_get_power(mkit_handle_t caller) TRACE_FUCTION
int ipvp_alive(mkit_handle_t caller) TRACE_FUCTION
int ipvp_get_capacity(mkit_handle_t caller, ipvp_capacity_t * capacity) TRACE_FUCTION
int ipvp_get_capset(mkit_handle_t caller, ipvp_capset_t * capset) TRACE_FUCTION
int ipvp_config_update(mkit_handle_t caller) TRACE_FUCTION
int ipvp_config_jib(mkit_handle_t caller, int adaptive, int jibmin, int jibnor,
                    int jibmax) TRACE_FUCTION
int ipvp_config_cng(mkit_handle_t caller, int vad_enable, int cng_enable, int cng_rx_gain,
                    int cng_tx_gain) TRACE_FUCTION
int ipvp_config_sidetone(mkit_handle_t caller, int handset_gain, int headset_gain) TRACE_FUCTION
int ipvp_config_aec(mkit_handle_t caller, int aec_enable, int hec_enable) TRACE_FUCTION
int ipvp_config_agc(mkit_handle_t caller, int rx_enable,  int tx_enable) TRACE_FUCTION
int ipvp_config_rtcp(mkit_handle_t caller, int rtcp_enable, int interval, int rtcpxr,
                     const char * cname) TRACE_FUCTION
int ipvp_config_dtmf(mkit_handle_t caller, ipvp_dtmfcfg_t * dtmfcfg) TRACE_FUCTION
int ipvp_config_tone(mkit_handle_t caller, int toneid, const char * tone_string) TRACE_FUCTION
int ipvp_config_gain(mkit_handle_t caller, int handsfree_gain, int handset_gain,
                     int headset_gain) TRACE_FUCTION
int ipvp_config_rtp(mkit_handle_t caller, ipvp_rtpcfg_t * rtpcfg) TRACE_FUCTION
int ipvp_config_g726(mkit_handle_t caller, int bit_reverse) TRACE_FUCTION
int ipvp_config_vqmon(mkit_handle_t caller, int warning, int critical,
                      const char * item) TRACE_FUCTION
int ipvp_config_QoS(mkit_handle_t caller, int audiotos, int videotos) TRACE_FUCTION
int ipvp_config_ans(mkit_handle_t caller, int ans_en) TRACE_FUCTION
int ipvp_config_bandwidth(mkit_handle_t caller, int bw_in, int bw_out) TRACE_FUCTION
int ipvp_set_mode(mkit_handle_t caller, int mode) TRACE_FUCTION
int ipvp_set_DEV(mkit_handle_t caller, int mic_mode, int spk_mode) TRACE_FUCTION
int ipvp_set_SPK(mkit_handle_t caller, int mode, int on) TRACE_FUCTION
int ipvp_set_MIC(mkit_handle_t caller, int mode, int on) TRACE_FUCTION
int ipvp_get_HPE(mkit_handle_t caller, int hpev) TRACE_FUCTION
int ipvp_get_mode(mkit_handle_t caller) TRACE_FUCTION
int ipvp_set_slave(mkit_handle_t caller, int slave, int dir, int control) TRACE_FUCTION
int ipvp_get_slave(mkit_handle_t caller, int slave) TRACE_FUCTION
int ipvp_select_mic(mkit_handle_t caller, int mic_mode, unsigned short ctrl,
                    unsigned short mask) TRACE_FUCTION
int ipvp_set_volume(mkit_handle_t caller, int volume) TRACE_FUCTION
int ipvp_get_volume(mkit_handle_t caller) TRACE_FUCTION
int ipvp_set_gain(mkit_handle_t caller, int gain, int dir) TRACE_FUCTION
int ipvp_get_gain(mkit_handle_t caller, int dir) TRACE_FUCTION
int ipvp_set_osin_gain(mkit_handle_t caller, int gain) TRACE_FUCTION
int ipvp_set_osout_gain(mkit_handle_t caller, int gain) TRACE_FUCTION
int ipvp_get_osin_gain(mkit_handle_t caller) TRACE_FUCTION
int ipvp_get_osout_gain(mkit_handle_t caller) TRACE_FUCTION
int ipvp_set_mute(mkit_handle_t caller, int mute) TRACE_FUCTION
int ipvp_get_mute(mkit_handle_t caller) TRACE_FUCTION
int ipvp_dtmf_play(mkit_handle_t caller, char dtmf, int sync) TRACE_FUCTION
int ipvp_dtmf_send(mkit_handle_t caller, int callID, char dtmf, int dir, int sync) TRACE_FUCTION
int ipvp_tone_play(mkit_handle_t caller, int toneid, int mix, int sync) TRACE_FUCTION
int ipvp_tone_play2(mkit_handle_t caller, const char * seqs, int mix, int sync) TRACE_FUCTION
int ipvp_tone_stop(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_audio_play(mkit_handle_t caller, const char * file, int dir, int loop,
                    int sync) TRACE_FUCTION
int ipvp_audio_stop(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_audio_pause(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_audio_resume(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_record_start(mkit_handle_t caller, ipvp_record_t * info, int sync) TRACE_FUCTION
int ipvp_record_start2(mkit_handle_t caller, const char * file, int sync) TRACE_FUCTION
int ipvp_record_stop(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_record_pause(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_record_resume(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_echo_test(mkit_handle_t caller, int control) TRACE_FUCTION
int ipvp_loop_test(mkit_handle_t caller, int control) TRACE_FUCTION
int ipvp_loop_video(mkit_handle_t caller, int control, int videoSRC, int videoDST) TRACE_FUCTION
int ipvp_audio_train_start(mkit_handle_t caller, int mode, const char * file) TRACE_FUCTION
int ipvp_audio_train_term(mkit_handle_t caller) TRACE_FUCTION
int ipvp_audio_tuning_start(mkit_handle_t caller, int mode) TRACE_FUCTION
int ipvp_audio_tuning_term(mkit_handle_t caller) TRACE_FUCTION
int ipvp_audio_motion(mkit_handle_t caller, int on_off) TRACE_FUCTION
int ipvp_media_play(mkit_handle_t caller, ipvp_playback_t * play, int sync) TRACE_FUCTION
int ipvp_media_play2(mkit_handle_t caller, const char * file, int sync) TRACE_FUCTION
int ipvp_media_stop(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_media_pause(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_media_resume(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_media_FFFR(mkit_handle_t caller, int step, int sync) TRACE_FUCTION
int ipvp_media_seek(mkit_handle_t caller, unsigned int locate, int sync) TRACE_FUCTION
int ipvp_media_getinfo(mkit_handle_t caller, ipvp_pinfo_t * info, const char * file) TRACE_FUCTION
int ipvp_speech_start(mkit_handle_t caller, int callID, ipvp_speech_t * speech,
                      int sync) TRACE_FUCTION
int ipvp_speech_stop(mkit_handle_t caller, int callID, int sync) TRACE_FUCTION
int ipvp_speech_modify(mkit_handle_t caller, int callID, ipvp_speech_t * speech,
                       int sync) TRACE_FUCTION
int ipvp_speech_hold(mkit_handle_t caller, int callID, int hold, int sync) TRACE_FUCTION
int ipvp_confer_merge(mkit_handle_t caller, const ipvp_confer_t * confer, int sync) TRACE_FUCTION
int ipvp_confer_split(mkit_handle_t caller, const ipvp_confer_t * confer, int sync) TRACE_FUCTION
int ipvp_get_speeches(mkit_handle_t caller) TRACE_FUCTION
int ipvp_get_conferes(mkit_handle_t caller) TRACE_FUCTION
int ipvp_get_audio_stats(mkit_handle_t caller, int callID, ipvp_astats_t * stats) TRACE_FUCTION
int ipvp_get_video_stats(mkit_handle_t caller, int callID, ipvp_vstats_t * stats) TRACE_FUCTION
int ipvp_get_share_stats(mkit_handle_t caller, int callID, ipvp_vstats_t * stats) TRACE_FUCTION
int ipvp_get_speech_stats(mkit_handle_t caller, int callID, ipvp_stats_t * stats) TRACE_FUCTION
int ipvp_get_display(mkit_handle_t caller, int videoDST, int * width, int * height,
                     int * fps) TRACE_FUCTION
int ipvp_get_capture(mkit_handle_t caller, int videoSRC, int * width, int * height,
                     int * fps) TRACE_FUCTION
int ipvp_set_display(mkit_handle_t caller, int videoDST, int width, int height,
                     int fps) TRACE_FUCTION
int ipvp_video_start(mkit_handle_t caller, int callID, ipvp_video_t * video, int sync) TRACE_FUCTION
int ipvp_video_modify(mkit_handle_t caller, int callID, ipvp_video_t * video,
                      int sync) TRACE_FUCTION
int ipvp_video_stop(mkit_handle_t caller, int callID, int sync) TRACE_FUCTION
int ipvp_shared_create(mkit_handle_t caller, int callID, ipvp_video_t * video,
                       int sync) TRACE_FUCTION
int ipvp_shared_destroy(mkit_handle_t caller, int callID, int sync) TRACE_FUCTION
int ipvp_shared_start(mkit_handle_t caller, int callID, ipvp_video_t * video,
                      int sync) TRACE_FUCTION
int ipvp_shared_stop(mkit_handle_t caller, int callID, int sync) TRACE_FUCTION
int ipvp_video_hold(mkit_handle_t caller, int callID, int hold, int sync) TRACE_FUCTION
int ipvp_video_mute(mkit_handle_t caller, int callID, int videoID, int mute, const char * file,
                    int sync) TRACE_FUCTION
int ipvp_video_merge(mkit_handle_t caller, const ipvp_confer_t * confer, int sync) TRACE_FUCTION
int ipvp_video_split(mkit_handle_t caller, const ipvp_confer_t * confer, int sync) TRACE_FUCTION
int ipvp_video_req_keyframe(mkit_handle_t caller, int callID, int videoID, int sync) TRACE_FUCTION
int ipvp_video_set_quality(mkit_handle_t caller, int callID, int videoID, ipvp_quality_t * quality,
                           int sync) TRACE_FUCTION
int ipvp_video_set_layout(mkit_handle_t caller, int devDST, ipvp_layout_t layout[], int layout_nr,
                          int sync) TRACE_FUCTION
int ipvp_video_set_source(mkit_handle_t caller, int callID, int videoID, int videoSRC,
                          int sync) TRACE_FUCTION
int ipvp_video_snapshot(mkit_handle_t caller, int videoID, const char * file,
                        int sync) TRACE_FUCTION
int ipvp_OSD_setup(mkit_handle_t caller, int videoID, int osdID, ipvp_osd_t * osd,
                   int sync) TRACE_FUCTION
int ipvp_OSD_clear(mkit_handle_t caller, int videoID, int osdID, int sync) TRACE_FUCTION
int ipvp_OSD_reset(mkit_handle_t caller, int sync) TRACE_FUCTION
int ipvp_request_notify(mkit_handle_t caller, int event) TRACE_FUCTION
int ipvp_cancel_notify(mkit_handle_t caller, int event) TRACE_FUCTION
int ipvp_dump_stat(mkit_handle_t caller, char * buff, size_t buff_max) TRACE_FUCTION


/***********************************************ipvp************************************************/
#include "ldap/yealink_ldap.h"

void yealink_ldap_config_init(void)  TRACE_FUCTION_NORETURN
void yealink_ldap_config_exit(void)  TRACE_FUCTION_NORETURN
int yealink_ldap_set_option(enum yealink_ldap_option ldap_opt, void * value)  TRACE_FUCTION
const char * yealink_ldap_get_version(void)  TRACE_FUCTION
int yealink_ldap_log_init(int log_mode, yealink_ldap_log_redirect_cb redir_cb)  TRACE_FUCTION
int yealink_ldap_set_log_level(yealink_ldap_log_level level)  TRACE_FUCTION
int yealink_ldap_bind(void ** yealinkld)  TRACE_FUCTION
int yealink_ldap_ubind(void ** yealinkld)  TRACE_FUCTION
int yealink_ldap_search(void ** yealinkld, int * pmsgid, char * search_key)  TRACE_FUCTION
int yealink_ldap_result(void ** yealinkld, int msgid, struct timeval * timeout,
                        yealink_ldap_link * hp)  TRACE_FUCTION
int yealink_ldap_abandon(void ** yealinkld, int msgid)  TRACE_FUCTION
int yealink_ldap_memfree(yealink_ldap_link result)  TRACE_FUCTION
int yealink_ldap_memfree_cert(yealink_ldap_link_cert result)  TRACE_FUCTION
int yealink_ldap_default_search(struct timeval * timeout, yealink_ldap_link * hp)  TRACE_FUCTION
int yealink_ldap_get_uc_rootca(char * user, char * pw, char * host, yealink_ldap_link_cert * outhp,
                               void * ref)  TRACE_FUCTION
int yealink_ldap_err2string(int err, char * errbuf, int errbuflen)  TRACE_FUCTION
int yealink_ldap_result_intergated(void ** yealinkld, int msgid, struct timeval * timeout,
                                   yealink_ldap_link_user * hp) TRACE_FUCTION
int yealink_ldap_memfree_integrated(yealink_ldap_link_user result) TRACE_FUCTION
int yealink_ldap_default_search_integrated(struct timeval * timeout,
    yealink_ldap_link_user * hp) TRACE_FUCTION

#ifdef  __cplusplus
extern "C" {
#endif
#include "xmpp/include/libxmpp.h"
#ifdef  __cplusplus
};
#endif

int xmpp_open(XMPP_CONN * conn, XMPP_CONN_INFO * conn_info)  TRACE_FUCTION
int xmpp_request(XMPP_CONN * conn, int req, char ** xml_stream, size_t * stream_len,
                 ...)  TRACE_FUCTION
int xmpp_close(XMPP_CONN * conn)  TRACE_FUCTION
int xmpp_destroy()  TRACE_FUCTION
const char * xmpp_get_version(void)  TRACE_FUCTION
xmpp_errno_t    xmpp_errno(XMPP_CONN * conn)
{
  TRACE_FUCTION_NORETURN ;
  return XMPP_ERR_NOT_CONNECTED ;
}
char    *   xmpp_strerror(XMPP_CONN * conn)  TRACE_FUCTION
void        xmpp_perror(XMPP_CONN * conn)  TRACE_FUCTION_NORETURN
char    *   xmpp_strerror_internal(XMPP_CONN * conn)  TRACE_FUCTION
void    xmpp_set_idle_timeout(XMPP_CONN * conn, int timeout)  TRACE_FUCTION_NORETURN
void    xmpp_log_set_level(int level)  TRACE_FUCTION_NORETURN
void    xmpp_log_set_print()  TRACE_FUCTION_NORETURN
void    xmpp_log_set_syslog()  TRACE_FUCTION_NORETURN
void    __xmpp_log(int level, const char * file, const char * function, int line,
                   const char * format, ...)  TRACE_FUCTION_NORETURN
int xmpp_set_features(XMPP_CONN * conn, int options, void * value) TRACE_FUCTION


/***********************************************h323************************************************/
#include "service_h323_interface.h"
int h323_ext_params_init(h323_ext_params_t ** params) TRACE_FUCTION
int h323_ext_params_add(h323_ext_params_t ** params, int type, void * buf,
                        int buf_len) TRACE_FUCTION
int h323_ext_params_clone(h323_ext_params_t ** dst, h323_ext_params_t * src) TRACE_FUCTION
void * h323_ext_params_get(h323_ext_params_t * params, int type, int * data_len) TRACE_FUCTION
int h323_ext_params_del(h323_ext_params_t * params, int type) TRACE_FUCTION
int h323_ext_params_free(h323_ext_params_t ** params) TRACE_FUCTION
int h323_service_get_version(mkit_handle_t caller, int type, char * buf,
                             unsigned int size) TRACE_FUCTION
int h323_service_init(mkit_handle_t caller) TRACE_FUCTION
int h323_service_start(mkit_handle_t caller) TRACE_FUCTION
int h323_service_stop(mkit_handle_t caller) TRACE_FUCTION
int h323_service_exit(mkit_handle_t caller) TRACE_FUCTION
int h323_service_get_option(mkit_handle_t caller, int option, void * buf,
                            unsigned int size) TRACE_FUCTION
int h323_service_set_option(mkit_handle_t caller, int option, void * value,
                            unsigned int size) TRACE_FUCTION
int h323_request_notify(mkit_handle_t caller, int evt) TRACE_FUCTION
int h323_cancel_notify(mkit_handle_t caller, int evt) TRACE_FUCTION
int h323_query_reg_status(mkit_handle_t caller) TRACE_FUCTION
int h323_call_new_cid(mkit_handle_t caller) TRACE_FUCTION
int h323_call_invite(mkit_handle_t caller, int cid, int call_mask, int bandwidth, char * callee,
                     h323_h264_param_t * h264param) TRACE_FUCTION
int h323_call_invite2(mkit_handle_t caller, int cid, h323_ext_params_t * params) TRACE_FUCTION
int h323_call_ringing(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_hangup(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_hangup2(mkit_handle_t caller, int cid,
                      h323_call_end_reason_code_e reason) TRACE_FUCTION
int h323_call_response(mkit_handle_t caller, int cid, int reponse_code) TRACE_FUCTION
int h323_call_set_video_media(mkit_handle_t caller, int cid, int enable) TRACE_FUCTION
int h323_call_set_bandwidth(mkit_handle_t caller, int cid, int bandwidth) TRACE_FUCTION
int h323_call_set_h264_param(mkit_handle_t caller, int cid, h323_h264_param_t * param) TRACE_FUCTION
int h323_call_set_call_param(mkit_handle_t caller, int cid,
                             h323_ext_params_t * params) TRACE_FUCTION
int h323_call_accept(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_reject(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_renegotiate(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_send_dtmf(mkit_handle_t caller, int cid, const char * dtmf) TRACE_FUCTION
int h323_call_local_mute(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_local_un_mute(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_camera_action_start(mkit_handle_t caller, int cid,
                                  h323_camare_action_e action) TRACE_FUCTION
int h323_call_camera_action_stop(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_camera_preset_activate(mkit_handle_t caller, int cid, int preset_num) TRACE_FUCTION
int h323_call_camere_preset_store(mkit_handle_t caller, int cid, int preset_num) TRACE_FUCTION
int h323_call_open_ext_video(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_close_ext_video(mkit_handle_t caller, int cid) TRACE_FUCTION
int h323_call_video_frame_sync(mkit_handle_t caller, int cid, int is_ext) TRACE_FUCTION
int h323_call_flow_ctrl_rate(mkit_handle_t caller, int cid,
                             h323_flow_ctrl_para_t * flow_para) TRACE_FUCTION
int h323_call_get_remote_useragent(mkit_handle_t caller, int cid, char * remoter,
                                   int size) TRACE_FUCTION
int h323_call_send_user_custom_info(mkit_handle_t caller, int cid, char * info,
                                    int info_size) TRACE_FUCTION

/***********************************************blueth************************************************/
#include "bluetooth/btkit_api.h"
void            btkit_set_log_level(int level) TRACE_FUCTION_NORETURN
int             bt_init(const btkit_cfg_t * cfg) TRACE_FUCTION
int             bt_init(const btkit_cfg_t * cfg, btkit_callback_t * cb) TRACE_FUCTION
int             bt_exit(void) TRACE_FUCTION
const char   *  btkit_build(void) TRACE_FUCTION
const char   *  btkit_version(void) TRACE_FUCTION
int  btkit_create(btkit_handle_t * hnd, btkit_create_t * create) TRACE_FUCTION
int  btkit_destroy(btkit_handle_t hnd) TRACE_FUCTION
int  btkit_power_set(btkit_handle_t hnd,  int enable) TRACE_FUCTION
int  btkit_discoverable_set(btkit_handle_t hnd, int enable, int time_out) TRACE_FUCTION
int  btkit_name_set(btkit_handle_t hnd, const char * name) TRACE_FUCTION
int  btkit_property_get(btkit_handle_t hnd, btkit_property_t * property) TRACE_FUCTION
btkit_dev_type_t  btkit_type_get(btkit_handle_t hnd)
{
  return BT_UNKNOWN_TYPE ;
}
int  btkit_devices_sum_get(btkit_handle_t hnd) TRACE_FUCTION
int  btkit_devices_get(btkit_handle_t hnd, btkit_dev_instance_t * dev_hnd) TRACE_FUCTION
int  btkit_scan_start(btkit_handle_t hnd) TRACE_FUCTION
int  btkit_scan_stop(btkit_handle_t hnd) TRACE_FUCTION
int  btkit_dev_cancel_pair(btkit_handle_t hnd, btkit_dev_instance_t  dev_hnd) TRACE_FUCTION
int  btkit_dev_pair(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_unpair(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_delete(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
btkit_dev_type_t  btkit_dev_type_get(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd)
{
  return BT_UNKNOWN_TYPE ;
}
int  btkit_dev_pin_answer(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                          const char * pin) TRACE_FUCTION
int  btkit_dev_pin_authorize(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                             int right) TRACE_FUCTION
int  btkit_dev_pin_confirm(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                           int right) TRACE_FUCTION
int  btkit_dev_connected(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_paired(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_addr_get(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, char * addr) TRACE_FUCTION
int  btkit_dev_alias_set(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                         const char * name) TRACE_FUCTION
int  btkit_dev_connect(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
#ifdef BT_AUTO_RECONNECT
int  btkit_dev_reconnect(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
#endif
int  btkit_dev_release(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_mic_gain_set(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                            int gain) TRACE_FUCTION
int  btkit_dev_mic_gain_get(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                            int * gain) TRACE_FUCTION
int  btkit_dev_spk_gain_set(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                            int gain) TRACE_FUCTION
int  btkit_dev_spk_gain_get(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                            int * gain) TRACE_FUCTION
int  btkit_dev_ring_start(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_ring_stop(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_play_start(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_play_stop(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_dev_call_invite(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,  const char * number,
                           int * callid) TRACE_FUCTION
int  btkit_dev_call_in(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                       const char * number) TRACE_FUCTION
int  btkit_dev_call_answer(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                           int callid) TRACE_FUCTION
int  btkit_dev_call_hangup(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                           int callid) TRACE_FUCTION
int  btkit_dev_call_hold(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, int callid,
                         int is_hold) TRACE_FUCTION
int  btkit_dev_call_total(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                          int * total) TRACE_FUCTION
int  btkit_dev_call_status(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, int callid,
                           int * status) TRACE_FUCTION
int  btkit_dev_call_send_dtmf(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, int callid,
                              const char dtmf) TRACE_FUCTION
int  btkit_dev_property_get(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                            btkit_dev_t * property) TRACE_FUCTION
int             btkit_mb_get_property(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                                      btkit_mb_property_t * property) TRACE_FUCTION
int  btkit_pba_download_start(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                              void * data) TRACE_FUCTION
int  btkit_pba_download_stop(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd) TRACE_FUCTION
int  btkit_snd_mode_set(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, int callid,
                        int mode) TRACE_FUCTION

/***********************************************hpdev***********************************************/
#include "hpdev/hpdev.h"
int hpdev_helper_mounted(const char * mount_point) TRACE_FUCTION


/***********************************************sip***********************************************/
#include "service_sip_interface.h"
int sip_call_sca_hold_ex(mkit_handle_t caller, int line_id, int call_id, int is_pub, int hold,
                         int appearance_index) TRACE_FUCTION

/***********************************************headset***********************************************/
#include "usb_headset/src/usbaudio_api.h"

int usb_audio_volume_initialize(int card) TRACE_FUCTION
int usb_audio_volume_uninitialize() TRACE_FUCTION
int usb_audio_volume_get_range(int * min, int * max) TRACE_FUCTION
int usb_audio_volume_get(int * volume) TRACE_FUCTION
int usb_audio_volume_set(int volume) TRACE_FUCTION
int usb_audio_volume_mute(int mute) TRACE_FUCTION
int usb_audio_volume_inc() TRACE_FUCTION
int usb_audio_volume_dec() TRACE_FUCTION

#include "usb_headset/src/usbheadset_api.h"
const char * usb_headset_version(void) TRACE_FUCTION
const char * usb_headset_build(void) TRACE_FUCTION
int usb_headset_initialize(usbheadset_hotplug_callback_f usbheadset_added,
                           usbheadset_hotplug_callback_f usbheadset_removed, void * user_data) TRACE_FUCTION
int usb_headset_uninitialize() TRACE_FUCTION
int usb_headset_log_set(int log_level) TRACE_FUCTION

#include "usb_headset/src/usbhid_api.h"

usbhid_handle_t usb_hid_open(char * path, usbhid_button_in_data_raw_f raw_data_callback_f,
                             usbhid_button_in_data_translated_f translated_data_callback_f) TRACE_FUCTION

void usb_hid_close(usbhid_handle_t handle) TRACE_FUCTION_NORETURN
int usb_hid_set_offhook(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_led_ring(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_tel_ringer(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_mute(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_micphonemute(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_hold(usbhid_handle_t handle, int on) TRACE_FUCTION
int usb_hid_set_online(usbhid_handle_t handle, int on) TRACE_FUCTION

/************************************************exp**********************************************
#include "exp/src/expdriveheader/exp_drv.h"
int exp_helper_get_hwversion(int fd,int hop,int buff_len,char *buff) TRACE_FUCTION
int exp_helper_get_swversion(int fd,int hop,int buff_len,char *buff) TRACE_FUCTION
int exp_helper_init(int fd,int hop) TRACE_FUCTION
int exp_helper_reset(int fd,int hop) TRACE_FUCTION
int exp_helper_open(void) TRACE_FUCTION
void exp_helper_close(int fd) TRACE_FUCTION_NORETURN
int exp_helper_set_encrypt(int fd,int hop,int enc) TRACE_FUCTION
int exp_helper_get_encrypt(int fd,int hop,int *enc_result) TRACE_FUCTION
int exp_helper_set_led_status(int fd,int hop,int row,int col,int led_status) TRACE_FUCTION
int exp_helper_set_lcd_item(int fd,int hop,int screen,int row,int col,int icon_number,int flags,int str_len, char *str_buff) TRACE_FUCTION
int exp_helper_set_lcd_icon(int fd,int hop,int screen,int row,int col,int icon_number) TRACE_FUCTION
int exp_helper_set_lcd_bklight(int fd,int hop,int bk) TRACE_FUCTION
int exp_helper_set_lcd_contrast(int fd,int hop,int contrast) TRACE_FUCTION
int exp_helper_set_lcd_screen(int fd,int hop,int screen) TRACE_FUCTION
int exp_helper_set_lcd_testmode(int fd,int hop,int status) TRACE_FUCTION
int exp_helper_sync(int fd) TRACE_FUCTION
int exp_helper_get_hopcount(int fd,int *hop_count) TRACE_FUCTION
int exp_helper_get_hoptype(int fd,int hop,int *hop_type) TRACE_FUCTION
int exp_helper_set_ehs_hook(int fd,int hop,int hook) TRACE_FUCTION
int exp_helper_upgrade(int fd, const char*img_path) TRACE_FUCTION
int exp_helper_event(int fd,struct exp_event*event) TRACE_FUCTION

/************************************************keypad**********************************************
#include "devicelib/keypad_drv.h"
int keypad_helper_open(void) TRACE_FUCTION
void keypad_helper_close(int fd) TRACE_FUCTION_NORETURN
int keypad_helper_get_hwversion(int fd,char *buff,int len) TRACE_FUCTION
int keypad_helper_get_swversion(int fd,char *buff,int len) TRACE_FUCTION
int keypad_helper_set_encrypt(int fd,unsigned int *enc) TRACE_FUCTION
int keypad_helper_get_encrypt(int fd,unsigned int *enc) TRACE_FUCTION
int keypad_helper_set_watchdog(int fd,int watchdog) TRACE_FUCTION
int keypad_helper_get_watchdog(int fd,int*watchdog) TRACE_FUCTION
int keypad_helper_close_watchdog(int fd) TRACE_FUCTION
int keypad_helper_sync(void) TRACE_FUCTION
int keypad_helper_reset(int fd) TRACE_FUCTION
int keypad_helper_get_led(int fd,led64*led) TRACE_FUCTION
int keypad_helper_get_led_mask(int fd,led64*mask) TRACE_FUCTION
int keypad_helper_set_led_flick(int fd,led64 mask,int ms_on,int ms_off) TRACE_FUCTION
int keypad_helper_get_led_flick(int fd,struct led_flick* flick) TRACE_FUCTION
int keypad_helper_rst_led_flick(int fd) TRACE_FUCTION
int keypad_helper_verify(int fd,unsigned int code) TRACE_FUCTION
int keypad_helper_set_bklight(int fd,int bklight) TRACE_FUCTION
int keypad_helper_get_bklight(int fd,int *bklight) TRACE_FUCTION
int keypad_helper_set_fanspeed(int fd,int fanspeed) TRACE_FUCTION
int keypad_helper_get_fanspeed(int fd,int*fanspeed) TRACE_FUCTION
int pstn_helper_get_event(int fd,int *event) TRACE_FUCTION
int pstn_helper_cmd(int fd,int cmd) TRACE_FUCTION
int pstn_helper_dial(int fd,const char * num) TRACE_FUCTION
int pstn_helper_fsk(int fd,struct pstn_fsk* fsk) TRACE_FUCTION
int keypad_helper_test_key(int fd,struct input_event* event) TRACE_FUCTION
int keypad_helper_set_voice_hf(int fd,int voice_hf) TRACE_FUCTION
int keypad_helper_get_hsdsta(int fd,int *hsdsta) TRACE_FUCTION
int keypad_helper_get_mbssta(int fd,int *mbssta) TRACE_FUCTION  */

/***********************************************expusb***********************************************
#include "exp/src/expdriveheader/expusb_api.h"

int  expusb_helper_open(const char *path)  TRACE_FUCTION
void expusb_helper_close(int fd)  TRACE_FUCTION_NORETURN
int  expusb_helper_start(int fd, unsigned int port, unsigned int val)  TRACE_FUCTION
void* expusb_helper_mmap(int fd, size_t size)  TRACE_FUCTION
void expusb_helper_munmap(void *start, size_t size)  TRACE_FUCTION_NORETURN

int  expusb_helper_read(int fd, void *buf, size_t count)  TRACE_FUCTION

int  expusb_helper_write(int fd, const void *buf, size_t count)  TRACE_FUCTION

int  expusb_helper_hwver(int fd, struct expusb_hwver *version)  TRACE_FUCTION

int  expusb_helper_appver(int fd, struct expusb_swver *version)  TRACE_FUCTION

int  expusb_helper_loaderver(int fd, struct expusb_swver *version)  TRACE_FUCTION

int  expusb_helper_drvver(int fd, struct expusb_ver *version)  TRACE_FUCTION

int  expusb_helper_getrunmod(int fd, struct expusb_val *mod)  TRACE_FUCTION
int  expusb_helper_bootm(int fd, unsigned int port, unsigned int addr, int argc, const char*argv)  TRACE_FUCTION

int  expusb_helper_reset(int fd, int port, int val)  TRACE_FUCTION
int expusb_helper_memory_common(int fd, unsigned int port, unsigned int cmd, unsigned int bitw, unsigned int addr, void*buf, unsigned int count)  TRACE_FUCTION

int  expusb_helper_memory_read(int fd, unsigned int port, unsigned int bitw, unsigned int addr, void*buf, unsigned int count) TRACE_FUCTION
int  expusb_helper_memory_write(int fd, unsigned int port, unsigned int bitw, unsigned int addr, const void*buf, unsigned int count) TRACE_FUCTION

int  expusb_helper_verify(int fd, unsigned int port, unsigned int type, unsigned int addr, unsigned int size, unsigned int check) TRACE_FUCTION

int  expusb_helper_flash_erase(int fd, unsigned int port, unsigned int addr, unsigned int size)  TRACE_FUCTION

int expusb_helper_flash_common(int fd, unsigned int port, unsigned int cmd, unsigned int addr, void*buf, unsigned int count)  TRACE_FUCTION
int  expusb_helper_flash_read(int fd, unsigned int port, unsigned int addr, void*buf, unsigned int count)  TRACE_FUCTION
int  expusb_helper_flash_write(int fd, unsigned int port, unsigned int addr, const void*buf, unsigned int count)  TRACE_FUCTION

int  expusb_helper_led_set(int fd, struct expusb_led led)  TRACE_FUCTION

int  expusb_helper_led_get(int fd, struct expusb_led *led)  TRACE_FUCTION
int expusb_helper_fb_fscreeninfo(int fd, unsigned int port, struct fb_fix_screeninfo *finfo)  TRACE_FUCTION

int expusb_helper_fb_vscreeninfo(int fd, unsigned int port, struct fb_var_screeninfo *vinfo)  TRACE_FUCTION

int  expusb_helper_fb_flush(int fd, unsigned int port, unsigned short page, unsigned short display)  TRACE_FUCTION

int  expusb_helper_fb_fillrect(int fd, struct expusb_fillrect rect)  TRACE_FUCTION
int  expusb_helper_fb_pagedisp(int fd, unsigned int port, unsigned int page)  TRACE_FUCTION

int expusb_helper_fb_logodisp(int fd, unsigned int port)  TRACE_FUCTION

int  expusb_helper_setbklight(int fd, unsigned int port, unsigned int level)  TRACE_FUCTION

int  expusb_helper_getbklight(int fd, struct expusb_val *bklight)  TRACE_FUCTION

int  expusb_helper_rdsn(int fd, unsigned int port, char*buff, unsigned int len)  TRACE_FUCTION

int  expusb_helper_wrsn(int fd, unsigned int port, char*buff, unsigned int len)  TRACE_FUCTION
int expusb_helper_expnum_get(int fd, unsigned int *expnum)  TRACE_FUCTION */


/***********************************************hpdev***********************************************/
#include "hpdev/hpdev.h"

int hpdev_create(HPDEV_HANDLE * pHandle, HPDEV_CB hpdev_cb, void * private_data)  TRACE_FUCTION
int hpdev_destroy(HPDEV_HANDLE hHandle)  TRACE_FUCTION

int hpdev_run(HPDEV_HANDLE hHandle)  TRACE_FUCTION
int hpdev_halt(HPDEV_HANDLE hHandle)  TRACE_FUCTION

int hpdev_set_events(HPDEV_HANDLE hHandle, short events)  TRACE_FUCTION
int hpdev_poll(HPDEV_HANDLE hHandle, int time)  TRACE_FUCTION
int hpdev_process(HPDEV_HANDLE hHandle)  TRACE_FUCTION
int hpdev_dir_scan(HPDEV_HANDLE hHandle)  TRACE_FUCTION

int hpdev_helper_mount(const char * dev, const char * mount_point, const char * fs_type,
                       unsigned long mountflags, void * data)  TRACE_FUCTION
int hpdev_helper_umount(const char * mount_point)  TRACE_FUCTION

int hpdev_helper_format(const char * dev_node)  TRACE_FUCTION
int hpdev_helper_create_devnode(const char * path, int major, int minor)  TRACE_FUCTION

/***********************************************hpdev***********************************************
#include "exp/src/expdriveheader/hpdev_api.h"
int  hpdev_helper_open(void) TRACE_FUCTION
void hpdev_helper_close(int fd) TRACE_FUCTION_NORETURN
int  hpdev_helper_drvver(int fd, char *ver, size_t len) TRACE_FUCTION

**********************************************end***********************************************/
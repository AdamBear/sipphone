/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-11-29
*
****************************************************************** */
#ifndef DECT_DRV_H
#define DECT_DRV_H

#include <linux/ioctl.h>

#ifdef __KERNEL__
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/ip.h>
#else
#include <netinet/in.h>
#endif

/************************************************************************/
/*   APIs for /dev/cordless                                             */
/************************************************************************/
/*
* ioctl param
*/

struct cordless_init_t
{
    unsigned long  fifo_size;
    void          *image;
    unsigned long  image_size;
#define  CORDLESS_INIT_OPT_NONE         0x0000
#define  CORDLESS_INIT_OPT_MENU         0x0001
#define  CORDLESS_INIT_OPT_EEPROM       0x0002

#define  CORDLESS_INIT_OPT_L2C_SHIFT    8
#define  CORDLESS_INIT_OPT_L2C_MASK     0xFFFFFF00
#define  CORDLESS_INIT_OPT_L2C_TBR6     0x00000100
#define  CORDLESS_INIT_OPT_L2C_NEMO     0x00000200
#define  CORDLESS_INIT_OPT_L2C_AUTH     0x00000400
#define  CORDLESS_INIT_OPT_L2C_REPEATER     0x00000800

    int            options;
    unsigned char *eeprom;
    unsigned int   eeprom_len;
};

struct timing_request
{
    int           options;
    unsigned long timestamp;
    unsigned char interrupts[66 * 8];
};

struct config_write_reply
{
    int bytes_written_successfully;
    int result;
};

struct operate_request
{
#define OPC_INVALID     0xFFFFFFFF
#define OPC_NONE        0x00000000
#define OPC_MASK_CMD    0xFF000000
#define OPC_GET_RFTUN   0x01000000
#define OPC_SET_RFTUN   0x02000000
#define OPC_SET_CLOG    0x03000000
#define OPC_TEST_ON     0x04000000
#define OPC_TEST_OFF    0x05000000
#define OPC_GET_RFPI    0x06000000
#define OPC_SET_RFPI    0x07000000
#define OPC_GET_CONF    0x08000000
#define OPC_AUTO_REG    0x09000000
#define OPC_TESTCASE    0x0A000000

    int opc;

    unsigned int raw1;
    unsigned int raw2;
    unsigned int raw3;
};

/*
* ioctl CMD
*/
#define CORDLESS_IOC_MAGIC              'D'

#define CORDLESS_IOCINIT                _IOW(CORDLESS_IOC_MAGIC, 0, struct cordless_init_t *)
#define CORDLESS_IOCDEINIT              _IO (CORDLESS_IOC_MAGIC, 1)
#define CORDLESS_IOCMENU                _IO (CORDLESS_IOC_MAGIC, 4)
#define CORDLESS_IOCTIMING              _IOW(CORDLESS_IOC_MAGIC, 5, struct timing_request *)
#define CORDLESS_IOCCONFIG              _IOR(CORDLESS_IOC_MAGIC, 7, unsigned char *)
#define CORDLESS_IOCCONFIG_REPLY        _IOW(CORDLESS_IOC_MAGIC, 8, struct config_write_reply *)
#define CORDLESS_IOCCONFIG_SET_STATUS   _IOW(CORDLESS_IOC_MAGIC, 9, int *)
#define CORDLESS_IOCOPERATE             _IOW(CORDLESS_IOC_MAGIC, 10,struct operate_request *)

#define CORDLESS_IOC_MAXNR              10

/************************************************************************/
/*  APIs for /dev/voice?                                                */
/************************************************************************/
/*
*----------------------
* ioctl param:channel
*----------------------
*/
#define DECT_DRV_CODEC_MAX          6
#define DECT_DRV_CODEC_MAX_DYN     15

#define DECT_DRV_MAX_SDES_ITEMS     8
#define DECT_DRV_MAX_SDES_VALLEN    50

/*type of packet*/
#define DECT_DRV_PKT_INV            0
#define DECT_DRV_PKT_RTP            1
#define DECT_DRV_PKT_RTCP           2

/*capacity of driver*/
#define DECT_DRV_CAP_NONE           0x00000000
#define DECT_DRV_CAP_KMRTP          0x00000001
#define DECT_DRV_CAP_KMRTCP         0x00000002
#define DECT_DRV_CAP_KMSRTP         0x00000004

/*
* mode of RTP&RTCP
* same as RTCP_MODE_RX|RTCP_MODE_TX
*/
#define DECT_DRV_MODE_NONE     0x00000000 
#define DECT_DRV_MODE_SEND     0x00000001 
#define DECT_DRV_MODE_RECV     0x00000002
#define DECT_DRV_MODE_ALL      (DECT_DRV_MODE_SEND|DECT_DRV_MODE_RECV)

/*
* DTMF
*/
#define DECT_DRV_DTMF_NONE     0x00000000
#define DECT_DRV_DTMF_MARKER   0x10000000
#define DECT_DRV_DTMF_BLOCKRTP 0x20000000
#define DECT_DRV_DTMF_LOCAL    0x40000000
#define DECT_DRV_DTMF_NET      0x80000000

/*
*RTP
*/
#define DECT_DRV_RTP_PKG_HEAD_LEN     12 /*rtp pkg head len*/
#define DECT_DRV_RTPPACKET_GET_PAYLOAD(pktp)     ((ntohl( * ((unsigned int *)(pktp))) & 0x007f0000)>>16)

#define DECT_DRV_JITTER_BUFFER_MIN    60

/*
* SRTP
*/
#define DECT_DRV_SRTP_KEY_MAX  128

/*
* cng
*/
struct rtp_cng_cfg
{
    int  rx_level;
    int  rx_mode;
    int  tx_level;
    int  tx_mode;
    int  sid_update_max;
    int  vad_detect_level;
    int  vad_hangover;
};

/*
* jitter buffer
*/
/*adaptation type */
#define DECT_DRV_JIB_TYPE_FIXED         0
#define DECT_DRV_JIB_TYPE_ADAPTIVE      1
/*adaptation mode,default 8 */
#define DECT_DRV_JIB_MODE_ADAP_SILENCE  0 /* adaptation in silence periods only*/
#define DECT_DRV_JIB_MODE_ADAP_REQUEST  4 /* adaptation immediately on request*/
#define DECT_DRV_JIB_MODE_ADAP_FORCE    8 /* forced adaptation after defined time (SERVICETIME)*/

struct rtp_jib_cfg
{
    int   type;
    int   mode;
    int   negative_step;      /*default 40*/
    int   negative_threshold; /*NOT used*/
    int   positive_step;      /*default 40*/
    int   positive_threshold; /*default 3*/
    int   buffer_max;         /*default 150ms,in 10ms units */
    int   buffer_min;         /*default 20ms,in 10ms units */
    int   buffer_initial;     /*default 60ms,in 10ms units */
    int   threshold_service;  /*default 40000,as samples(10ms)*/
    int   threshold_resync;   /*default 3*/
};

/*
* codec
*/
struct rtp_rx_codec
{
    char rx_pt;
    char rx_name[DECT_DRV_CODEC_MAX_DYN];
};
struct rtp_tx_codec
{
    char tx_pt;
    char tx_name[DECT_DRV_CODEC_MAX_DYN];
};

/*
* voice RTP
*/
struct voice_rtp
{
    int                 rtp_size;

	/*sync setting*/
	int                 rtp_mode;
	int                 aud_opts;

    int                 timeorg; 
    int                 timestamp;
              
    /*recv codec,maybe more than one*/
    struct rtp_rx_codec rx_codec[DECT_DRV_CODEC_MAX];
    int                 rx_codec_event;
    int                 rx_ptime;
    int                 rx_ssrc;
    int                 rx_rtp_len;

    /*send codec,only one needed*/
    struct rtp_tx_codec tx_codec[1];
    int                 tx_codec_event;
    int                 tx_ptime;
    int                 tx_ssrc;

    /*other setting*/
    struct rtp_cng_cfg  cng;
    struct rtp_jib_cfg  jib;

    /*DTMF*/
    int                 dtmf_rfc2833_ends;
};

struct voice_rtcp
{
    int   rtcp_size;
    int   rtcp_mode;          
    char  rtcp_sdesItem[DECT_DRV_MAX_SDES_ITEMS][DECT_DRV_MAX_SDES_VALLEN];
    int   rtcp_interval;
};

struct voice_srtp
{
    int   srtp_size;
    int   srtp_mode;
    char  srtp_tx_key[DECT_DRV_SRTP_KEY_MAX];
    char  srtp_rx_key[DECT_DRV_SRTP_KEY_MAX];
};

/*
*----------------------
* ioctl param:kmode
*----------------------
*/
struct voice_kaddr
{
    int  flags;
    int  af_type;
    int  sk_fd;

    int  sa_slen;
    int  sa_dlen;

    union
    {
        struct sockaddr_in  sa4;
        struct sockaddr_in6 sa6;
    }src;
    union
    {
        struct sockaddr_in  sa4;
        struct sockaddr_in6 sa6;
    }dst;
};

struct voice_kmode
{
    int               size;
#define DECT_DRV_KM_NONE  0x00000000
#define DECT_DRV_KM_RTP   0x10000000
#define DECT_DRV_KM_RTCP  0x20000000
#define DECT_DRV_KM_ALL   (DECT_DRV_KM_RTP|DECT_DRV_KM_RTCP)

    int                flags;
    struct voice_kaddr rtp;
    struct voice_kaddr rtcp;
};

/*
*----------------------
* ioctl param:DTMF
*----------------------
*/
struct voice_dtmf_event
{
	int  options;
	int  status;
    int  event;
    int  volume;
    int  duration;
    int  duration_evt;
    int  duration_evt_max;
};

/*
*----------------------
* ioctl param:generic event
*----------------------
*/
struct voice_generic_event
{
    int  dynamicPT;
    int  eventID;
};

/*
*----------------------
* ioctl CMD:
*----------------------
*/
/* ioctl definitions */
#define VOICE_IOC_MAGIC         'V'

/*basic*/
#define VOICE_IOCGCAPACITY      _IOW(VOICE_IOC_MAGIC, 0x00, unsigned int *)

/*chan&codec*/
#define VOICE_IOCRTP_START      _IOW(VOICE_IOC_MAGIC, 0x10, struct voice_rtp *)
#define VOICE_IOCRTP_STOP       _IO (VOICE_IOC_MAGIC, 0x11)
#define VOICE_IOCRTP_UPDATE     _IOW(VOICE_IOC_MAGIC, 0x12, struct voice_rtp *)

#define VOICE_IOCRTCP_START     _IOW(VOICE_IOC_MAGIC, 0x13, struct voice_rtcp *)
#define VOICE_IOCRTCP_STOP      _IO (VOICE_IOC_MAGIC, 0x14)
#define VOICE_IOCRTCP_UPDATE    _IOW(VOICE_IOC_MAGIC, 0x15, struct voice_rtcp *)

#define VOICE_IOCGDEC_LATENCY   _IOR(VOICE_IOC_MAGIC, 0x16,unsigned  long*)
#define VOICE_IOCGENC_LATENCY   _IOR(VOICE_IOC_MAGIC, 0x17,unsigned  long*)

/*SRTP*/
#define VOICE_IOCSRTP_SETUP     _IOW(VOICE_IOC_MAGIC, 0x16, struct voice_srtp *)
#define VOICE_IOCSRTP_CLOSE     _IO (VOICE_IOC_MAGIC, 0x17)

/*generic*/
#define VOICE_IOCDTMF_SEND      _IOW(VOICE_IOC_MAGIC, 0x20, struct voice_dtmf_event *)
#define VOICE_IOCDTMF_RECV      _IOR(VOICE_IOC_MAGIC, 0x21, struct voice_dtmf_event *)
#define VOICE_IOCEVENT_GEN      _IOW(VOICE_IOC_MAGIC, 0x22, struct voice_generic_event *)
#define VOICE_IOCHWIO_FLUSH     _IO (VOICE_IOC_MAGIC, 0x23)

/*RTP&RTCP*/
#define VOICE_IOCKMODE_START    _IOW(VOICE_IOC_MAGIC, 0x30, struct voice_kmode *)
#define VOICE_IOCKMODE_STOP     _IO (VOICE_IOC_MAGIC, 0x31)
#define VOICE_IOCKMODE_UPDATE   _IOW(VOICE_IOC_MAGIC, 0x32, struct voice_kmode *)

#define VOICE_IOC_MAXNR         0x40

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __KERNEL__

#include <sys/ioctl.h>
/************************************************************************/
/*                        user helper API                               */
/************************************************************************/
/*
*----------------------------------
* helper of /dev/cordless
*----------------------------------
*/
static inline int cordless_helper_load(int fd, struct cordless_init_t *cload)
{
    if (!cload || fd<0 || ioctl(fd, CORDLESS_IOCINIT, cload) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_unload(int fd)
{
    if (fd<0 || ioctl(fd, CORDLESS_IOCDEINIT) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_menu(int fd)
{
    if (fd<0 || ioctl(fd, CORDLESS_IOCMENU) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_timing(int fd, struct timing_request *timing)
{
    if (!timing || fd<0 || ioctl(fd, CORDLESS_IOCTIMING, timing) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_cfg(int fd, unsigned char *cfg)
{
    if (!cfg || fd<0 || ioctl(fd, CORDLESS_IOCCONFIG, cfg) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_reply(int fd, struct config_write_reply *reply)
{
    if (!reply || fd<0 || ioctl(fd, CORDLESS_IOCCONFIG_REPLY, reply) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_set_status(int fd, int *status)
{
    if (!status || fd<0 || ioctl(fd, CORDLESS_IOCCONFIG_SET_STATUS, status) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_get_rftun(int fd, unsigned int *rftun)
{
    struct operate_request op=
    {
        op.opc = OPC_GET_RFTUN,
    };

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    *rftun = op.raw1;
    return 0;
}

static inline int cordless_helper_set_rftun(int fd, unsigned int rftun)
{
    struct operate_request op=
    {
        op.opc = OPC_SET_RFTUN,
        op.raw1= rftun,
    };

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_set_clog(int fd, int prefix, int logmod, int loglev)
{
    struct operate_request op=
    {
        op.opc = OPC_SET_CLOG,
        op.raw1= prefix,
        op.raw2= logmod,
        op.raw3= loglev,
    };

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_test(int fd, int on)
{
    struct operate_request op;

    if (on)
        op.opc = OPC_TEST_ON;
    else
        op.opc = OPC_TEST_OFF;

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_get_rfpi(int fd, unsigned char rfpi[5])
{
    struct operate_request op;

    op.opc = OPC_GET_RFPI;

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    rfpi[0] = (op.raw2>>0 ) & 0xff;
    rfpi[1] = (op.raw1>>24) & 0xff;
    rfpi[2] = (op.raw1>>16) & 0xff;
    rfpi[3] = (op.raw1>>8 ) & 0xff;
    rfpi[4] = (op.raw1>>0 ) & 0xff;

    return 0;
}

static inline int cordless_helper_set_rfpi(int fd, unsigned char rfpi[5])
{
    struct operate_request op;

    op.opc = OPC_SET_RFPI;
    op.raw2 = (rfpi[0]<<0 ) & 0x000000ff;
    op.raw1 = (rfpi[1]<<24) & 0xff000000;
    op.raw1|= (rfpi[2]<<16) & 0x00ff0000;
    op.raw1|= (rfpi[3]<<8 ) & 0x0000ff00;
    op.raw1|= (rfpi[4]<<0 ) & 0x000000ff;

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    return 0;
}

static inline int cordless_helper_auto_sub(int fd, int*hs, unsigned char ipei[5])
{
    struct operate_request op;

    if (!hs)
        return -EFAULT;

    op.opc = OPC_AUTO_REG;
    op.raw3 = *hs;
    op.raw2 = (ipei[0]<<0 ) & 0x000000ff;
    op.raw1 = (ipei[1]<<24) & 0xff000000;
    op.raw1|= (ipei[2]<<16) & 0x00ff0000;
    op.raw1|= (ipei[3]<<8 ) & 0x0000ff00;
    op.raw1|= (ipei[4]<<0 ) & 0x000000ff;

    if (fd<0 || ioctl(fd, CORDLESS_IOCOPERATE, &op) < 0)
        return -EINVAL;

    *hs     = op.raw3 & 0xff;
    ipei[0] = (op.raw2>>0 ) & 0xff;
    ipei[1] = (op.raw1>>24) & 0xff;
    ipei[2] = (op.raw1>>16) & 0xff;
    ipei[3] = (op.raw1>>8 ) & 0xff;
    ipei[4] = (op.raw1>>0 ) & 0xff;

    return 0;
}

/*
*----------------------------------
* helper of /dev/voice?
*----------------------------------
*/

/*APIs for basic*/
static inline int voice_helper_capacity(int fd)
{
    int cap = DECT_DRV_CAP_NONE;

    if (fd<0 || ioctl(fd, VOICE_IOCGCAPACITY, &cap) < 0)
        return DECT_DRV_CAP_NONE;

    return cap;
}

/*APIs for chan&codec*/
static inline int voice_helper_rtp_start(int fd, struct voice_rtp *rtp)
{
    rtp->rtp_size = sizeof(struct voice_rtp);

    if (fd<0 || ioctl(fd, VOICE_IOCRTP_START, rtp) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_rtp_stop(int fd)
{
    if (fd<0 || ioctl(fd, VOICE_IOCRTP_STOP, 0) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_rtp_update(int fd, struct voice_rtp *rtp)
{
    rtp->rtp_size = sizeof(struct voice_rtp);

    if (fd<0 || ioctl(fd, VOICE_IOCRTP_UPDATE, rtp) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_rtcp_start(int fd, struct voice_rtcp *rtcp)
{
    rtcp->rtcp_size = sizeof(struct voice_rtcp);

    if (fd<0 || ioctl(fd, VOICE_IOCRTCP_START, rtcp) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_rtcp_stop(int fd)
{
    if (fd<0 || ioctl(fd, VOICE_IOCRTCP_STOP, 0) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_rtcp_update(int fd, struct voice_rtcp *rtcp)
{
    rtcp->rtcp_size = sizeof(struct voice_rtcp);

    if (fd<0 || ioctl(fd, VOICE_IOCRTCP_UPDATE, rtcp) < 0)
        return -EINVAL;

    return 0;
}
static inline int voice_helper_srtp(int fd, struct voice_srtp *srtp)
{
    if(srtp)
    {
        srtp->srtp_size = sizeof(struct voice_srtp);

        if (fd<0 || ioctl(fd, VOICE_IOCSRTP_SETUP, srtp) < 0)
            return -EINVAL;
    }
    else
    {
        if (fd<0 || ioctl(fd, VOICE_IOCSRTP_CLOSE, 0) < 0)
            return -EINVAL;
    }
    return 0;
}

/*APIs for generic*/
static inline int voice_helper_send_dtmf(int fd, struct voice_dtmf_event *dtmf)
{
    if (!dtmf || fd<0 || ioctl(fd, VOICE_IOCDTMF_SEND, dtmf) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_recv_dtmf(int fd, struct voice_dtmf_event *dtmf)
{
    if (!dtmf || fd<0 || ioctl(fd, VOICE_IOCDTMF_RECV, dtmf) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_generic_event(int fd, struct voice_generic_event *event)
{
    if (!event || fd<0 || ioctl(fd, VOICE_IOCEVENT_GEN, event) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_flush(int fd)
{
    if (fd<0 || ioctl(fd, VOICE_IOCHWIO_FLUSH) < 0)
        return -EINVAL;

    return 0;
}


/*APIs for kernel poll*/
static inline int voice_helper_kmode_start(int fd, struct voice_kmode *kmode)
{
    kmode->size = sizeof(struct voice_kmode);

    if (fd<0 || ioctl(fd, VOICE_IOCKMODE_START, kmode) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_kmode_stop(int fd,int kmode)
{
    if (fd<0 || ioctl(fd, VOICE_IOCKMODE_STOP, kmode) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_kmode_update(int fd, struct voice_kmode *kmode)
{
    kmode->size = sizeof(struct voice_kmode);

    if (fd<0 || ioctl(fd, VOICE_IOCKMODE_UPDATE, kmode) < 0)
        return -EINVAL;

    return 0;
}

static inline int voice_helper_dec_latency(int fd)
{
    unsigned long latency = 0;

    if (fd<0 || ioctl(fd, VOICE_IOCGDEC_LATENCY, &latency) < 0)
        return -EINVAL;

    return (int)latency;
}

static inline int voice_helper_enc_latency(int fd)
{
    unsigned long latency = 0;

    if (fd<0 || ioctl(fd, VOICE_IOCGENC_LATENCY, &latency) < 0)
        return -EINVAL;

    return (int)latency;
}

#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* DECT_DRV_H */

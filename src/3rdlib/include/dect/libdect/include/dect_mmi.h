/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2010-10-21
*
*******************************************************************/
#ifndef __DECT_MMI_H__
#define __DECT_MMI_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define  DECT_MMI_TONE_MAX  8
#define  DECT_MMI_LINE_MAX  4
#define  DECT_MMI_RDWR_MAX  16

#define  DECT_MMI_LINE_INV  -1
/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* cmd
*/
#define DECT_MMI_CMD_START                 100
/*line independance*/
#define DECT_MMI_CMD_MODE                  0
#define DECT_MMI_CMD_PROFILE               1
#define DECT_MMI_CMD_CHANNEL               2
#define DECT_MMI_CMD_TONE                  3

/*line dependance*/
#define DECT_MMI_CMD_CALL_START            10
#define DECT_MMI_CMD_CALL_HOLD             11
#define DECT_MMI_CMD_CALL_RESUME           12
#define DECT_MMI_CMD_CALL_END              13
#define DECT_MMI_CMD_CALL_CONF             14

/*debug*/
#define DECT_MMI_CMD_AFE_RD                30
#define DECT_MMI_CMD_AFE_WR                31
#define DECT_MMI_CMD_DSP_RD                32
#define DECT_MMI_CMD_DSP_WR                33
#define DECT_MMI_CMD_REG_RD                34
#define DECT_MMI_CMD_REG_WR                35

#define DECT_MMI_CMD_MAX                   36


#define DECT_MMI_MODE_HANDSFREE            0
#define DECT_MMI_MODE_HANDSET              1
#define DECT_MMI_MODE_HEADSET              2
#define DECT_MMI_MODE_HANDSET_GL           3/*HANDSET & HANDSFREE*/
#define DECT_MMI_MODE_HEADSET_GL           4/*HEADSET & HANDSFREE*/

#define DECT_MMI_MODE_WB                   1
#define DECT_MMI_MODE_NB                   0

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of mmi_channel
*   action as standalone
*/
typedef struct
{
    D_INT   mode;
    D_INT   wbnb;
    D_INT   flags;
#define MMI_CFL_NONE           0x00000000
#define MMI_CFL_RESET          (1<<0)
#define MMI_CFL_SW_SPK_MUTE    (1<<1)
#define MMI_CFL_HW_SPK_MUTE    (1<<2)

#define MMI_CFL_SW_MIC_MUTE    (1<<3)
#define MMI_CFL_HW_MIC_MUTE    (1<<4)

#define MMI_CFL_SW_SPK_GAIN    (1<<5)
#define MMI_CFL_HW_SPK_GAIN    (1<<6)

#define MMI_CFL_SW_MIC_GAIN    (1<<7)
#define MMI_CFL_HW_MIC_GAIN    (1<<8)

    D_BYTE  spk_sw_mute;
    D_BYTE  spk_hw_mute;

    D_BYTE  mic_sw_mute;
    D_BYTE  mic_hw_mute;

    D_INT   spk_sw_gain;
    D_INT   spk_hw_gain;

    D_INT   mic_sw_gain;
    D_INT   mic_hw_gain;

} mmi_channel;


/*
* type of mmi_profile
*   action as profile defined
*/
typedef struct
{
    D_INT   mode;
    D_INT   wbnb;
    D_INT   flags;
#define MMI_PFL_NONE     0x00000000
#define MMI_PFL_CHAN     (1<<0)           /*switch AFE channel*/
#define MMI_PFL_GAIN     (1<<1)           /*apply gain table*/
#define MMI_PFL_MUTE     (1<<2)           /*apply mute setting*/
#define MMI_PFL_PROFILE  (1<<3)           /*apply profile as 'mask' asked*/
#define MMI_PFL_ALL      0xFFFFFFFF

    /*mask define in profile.h,PROF_FL_NONE*/
    D_INT   mask;

    D_INT   reserved[8];
} mmi_profile;


/*type of tone*/
typedef struct
{
    D_CHAR         line[DECT_MMI_LINE_MAX];/* target line array */
    D_WORD         flags;
#define MMI_TFL_NONE     0x00000000
#define MMI_TFL_STOP     (1<<0)      /* tone stop or start */
#define MMI_TFL_FREQ1    (1<<1)      /* freq1 valid */
#define MMI_TFL_FREQ2    (1<<2)      /* freq2 valid */
#define MMI_TFL_2NET     (1<<3)      /* play to network */
#define MMI_TFL_2MIX     (1<<4)      /* play to mix with talk */
#define MMI_TFL_2TEL     (1<<5)      /* play to local */

    D_WORD         gain;                   /* tone gain */

#define MMI_TONE_STOP     0
#define MMI_TONE_PAUSE    1
#define MMI_TONE_REPEAT   2
#define MMI_TONE_LOOP     3

    D_WORD         freq1_seq[DECT_MMI_TONE_MAX];
    D_WORD         freq2_seq[DECT_MMI_TONE_MAX];
    D_INT          freqx_dur[DECT_MMI_TONE_MAX];
} mmi_tone;


/*type of call-start*/
typedef struct
{
    D_INT   line;
    D_INT   flags;
#define MMI_CFL_NONE     0x00000000
#define MMI_CFL_CONF     (1<<0)
#define MMI_CFL_SPLT     (1<<1)

    D_INT   callid;
    D_INT   instance;
    D_INT   channel;
    D_CHAR  conf[DECT_MMI_LINE_MAX];
} mmi_call;


/*type of debug*/
typedef struct
{
    D_WORD size;
    D_WORD delay;
    D_INT  addr[DECT_MMI_RDWR_MAX];
    D_INT  value[DECT_MMI_RDWR_MAX];
} mmi_reg;

typedef struct
{
    D_WORD size;
    D_WORD delay;
    D_INT  addr[DECT_MMI_RDWR_MAX];
    D_INT  value[DECT_MMI_RDWR_MAX];
} mmi_afe;

typedef struct
{
    D_WORD size;
    D_WORD delay;
    D_INT  instance;
    D_INT  offset[DECT_MMI_RDWR_MAX];
    D_INT  value[DECT_MMI_RDWR_MAX];
} mmi_dsp;


/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* msg union
*/
typedef union
{
    mmi_channel     channel;
    mmi_profile     profile;
    mmi_tone        tone;
    mmi_call        call;

    /*debug only*/
    mmi_reg        dbg_reg;
    mmi_afe        dbg_afe;
    mmi_dsp        dbg_dsp;
} mmi_body;

/*
* msg struct
*/
typedef struct
{
    msg_hdr    hdr;
    mmi_body   body;
} mmi_msg;


/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_MMI_H__ */


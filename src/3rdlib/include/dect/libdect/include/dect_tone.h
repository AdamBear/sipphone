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
#ifndef __DECT_TONE_H__
#define __DECT_TONE_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <dect_type.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*local Tone*/
#define  DECT_TONE_OFF              0
#define  DECT_TONE_ERROR            1
#define  DECT_TONE_VALID            2
#define  DECT_TONE_WARNING          3
#define  DECT_TONE_LOWBAT           4
#define  DECT_TONE_HOLDTMOUT        5
#define  DECT_TONE_CHARGE           6
#define  DECT_TONE_NOSINAL          7
#define  DECT_TONE_SUCESS           8
#define  DECT_TONE_KEYCLICK         9
#define  DECT_TONE_VOICEMAILIDLE    10
#define  DECT_TONE_CURLIMIT         11

/*speech Tone*/
#define  DECT_TONE_DIAL             12 /*拨号音：已摘机，交换机工作并准备好接收呼叫指令*/
#define  DECT_TONE_RINGBACK         13 /* 回铃音：告知呼叫者连接已经建立并且对方正在响铃 */
#define  DECT_TONE_BUSY             14 /* 忙音：告知呼叫者连接处于忙碌状态 */
#define  DECT_TONE_CONGESTION       15 /* 拥塞音：告知呼叫者通信线路或设备等正在被占用而无法连接 */
#define  DECT_TONE_CALLWAITING      16 /* 呼叫等待音：告知正在占用通话的用户有人试图呼叫他 */
#define  DECT_TONE_DIALRECALL       17 /* 提醒拨号音：表明交换机已经准备好接收地址信息 */
#define  DECT_TONE_MESSAGE          18 /* 信息音：即将欠费 */
#define  DECT_TONE_INFO             19 /* 特殊消息音：区别于“忙音”、“拥塞音”的一种不可达信息，或者录音等特殊事件，通常在通话失败前响起 */
#define  DECT_TONE_INTERCEPT        20
#define  DECT_TONE_AUTOANSWER       21 /*  */
#define  DECT_TONE_STUTTER          22 /* 特殊拨号音：用来表明语音邮箱或者呼叫转移激活 */
#define  DECT_TONE_DAIL_SPECAIL_CONDITION   23 
#define  DECT_TONE_DAIL_MESSAGE_WAITING 24 
#define  DECT_TONE_VOICEMAIL        DECT_TONE_STUTTER 
#define  DECT_TONE_TRANSFER         DECT_TONE_STUTTER


/*tone id max*/
#define  DECT_TONE_MIN              0
#define  DECT_TONE_MAX              25

/*DTMF tone */
#define  DECT_TONE_DTMF_BASE        100
#define  DECT_TONE_DTMF_0           (DECT_TONE_DTMF_BASE+0)
#define  DECT_TONE_DTMF_1           (DECT_TONE_DTMF_BASE+1)
#define  DECT_TONE_DTMF_2           (DECT_TONE_DTMF_BASE+2)
#define  DECT_TONE_DTMF_3           (DECT_TONE_DTMF_BASE+3)
#define  DECT_TONE_DTMF_4           (DECT_TONE_DTMF_BASE+4)
#define  DECT_TONE_DTMF_5           (DECT_TONE_DTMF_BASE+5)
#define  DECT_TONE_DTMF_6           (DECT_TONE_DTMF_BASE+6)
#define  DECT_TONE_DTMF_7           (DECT_TONE_DTMF_BASE+7)
#define  DECT_TONE_DTMF_8           (DECT_TONE_DTMF_BASE+8)
#define  DECT_TONE_DTMF_9           (DECT_TONE_DTMF_BASE+9)

#define  DECT_TONE_DTMF_STAR        (DECT_TONE_DTMF_BASE+10)
#define  DECT_TONE_DTMF_POUND       (DECT_TONE_DTMF_BASE+11)

#define  DECT_TONE_DTMF_A           (DECT_TONE_DTMF_BASE+12)
#define  DECT_TONE_DTMF_B           (DECT_TONE_DTMF_BASE+13)
#define  DECT_TONE_DTMF_C           (DECT_TONE_DTMF_BASE+14)
#define  DECT_TONE_DTMF_D           (DECT_TONE_DTMF_BASE+15)

/*DTMF tone max*/
#define  DECT_TONE_DTMF_MAX         (16)


/*freq max*/
#define	 DECT_TONE_LEN 	            5

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*tone mask,for APIs*/
#define DECT_TONE_FL_NONE       0x0000

/*directions*/
#define DECT_TONE_FL_DIR        0x000F
#define DECT_TONE_FL_DIR_NET    0x0001 /*play to network*/
#define DECT_TONE_FL_DIR_SPK    0x0002 /*play to speaker*/
#define DECT_TONE_FL_DIR_HF     0x0004 /*fix play channel to hf,or follow current*/
#define DECT_TONE_FL_DIR_MIX    0x0008 /*mute valid on talk*/

/*volume valid*/
#define DECT_TONE_FL_VOL        0x0010 /*volume valid*/

/*value  skip*/
#define DECT_TONE_FL_VAL        0x0020 /*skip value even value is being set*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
	int            id;     /*tone ID*/
	int            flags;  /*valid mask*/
	float          volume; /*from -1000dB(mute) to +6dB*/
	const char    *name;   /*external name*/
	const char    *value;  /*internal rules*/
}DECT_TONE_T;


/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_TONE_H__ */

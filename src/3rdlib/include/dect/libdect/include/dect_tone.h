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
#define  DECT_TONE_DIAL             12 /*����������ժ����������������׼���ý��պ���ָ��*/
#define  DECT_TONE_RINGBACK         13 /* ����������֪�����������Ѿ��������ҶԷ��������� */
#define  DECT_TONE_BUSY             14 /* æ������֪���������Ӵ���æµ״̬ */
#define  DECT_TONE_CONGESTION       15 /* ӵ��������֪������ͨ����·���豸�����ڱ�ռ�ö��޷����� */
#define  DECT_TONE_CALLWAITING      16 /* ���еȴ�������֪����ռ��ͨ�����û�������ͼ������ */
#define  DECT_TONE_DIALRECALL       17 /* ���Ѳ������������������Ѿ�׼���ý��յ�ַ��Ϣ */
#define  DECT_TONE_MESSAGE          18 /* ��Ϣ��������Ƿ�� */
#define  DECT_TONE_INFO             19 /* ������Ϣ���������ڡ�æ��������ӵ��������һ�ֲ��ɴ���Ϣ������¼���������¼���ͨ����ͨ��ʧ��ǰ���� */
#define  DECT_TONE_INTERCEPT        20
#define  DECT_TONE_AUTOANSWER       21 /*  */
#define  DECT_TONE_STUTTER          22 /* ���Ⲧ������������������������ߺ���ת�Ƽ��� */
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

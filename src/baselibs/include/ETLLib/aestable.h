#ifndef _AESTABLE_H_
#define _AESTABLE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/******************************************************************************************
int pw_aes_encrypt(char *pKey, char *pSrc, char *pDest)
pKey:16 byte长的key
pSrc:16 byte长的明文输入
pDest:16 byte长的密文输出
******************************************************************************************/
void pw_aes_encrypt(char * pKey, char * pSrc, char * pDest);

/******************************************************************************************
int pw_aes_decrypt(char *pKey, char *pSrc, char *pDest)
pKey:16 byte长的key
pSrc:16 byte长的密文输入
pDest:16 byte长的明文输出
******************************************************************************************/
void pw_aes_decrypt(char * pKey, char * pSrc, char * pDest);


#ifdef  __cplusplus
}
#endif

#endif


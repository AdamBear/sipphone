#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H

#define DATA_LEN        160

#ifdef  __cplusplus
extern "C" {
#endif


/****************************************************************************
int pw_data_encrypt(const unsigned char *src, unsigned char dest[DATA_LEN])
说明：对输入字符串先进性AES加密，然后进行RC4加密，最后Base64编码。
(注KEY为内部固定值"YealinkPhone1105")
src:  输入明文(小于100byte)
dest: 输出密文
return:0:OK,-1:error
****************************************************************************/
int pw_data_encrypt(const unsigned char * src, unsigned char dest[DATA_LEN]);


/***************************************************************************
int pw_data_decrypt(const unsigned char *src, unsigned char dest[DATA_LEN])
说明：对输入字符串先进行Base64解码，然后RC4解密，最后进行AES解密。
(注KEY为内部固定值"YealinkPhone1105"，注意Base64要求src输入长度必须为4的倍数)
src:  输入密文(小于100byte)
dest: 输出明文
return:0:OK,-1:error
***************************************************************************/
int pw_data_decrypt(const unsigned char * src, unsigned char dest[DATA_LEN]);

// 增加指定key接口
int pw_data_encrypt_ex(const unsigned char * src, unsigned char dest[DATA_LEN], const char * lpKey);

// 增加指定key接口
int pw_data_decrypt_ex(const unsigned char * src, unsigned char dest[DATA_LEN], const char * lpKey);
#ifdef  __cplusplus
}
#endif

#endif


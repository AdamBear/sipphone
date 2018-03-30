#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef  __cplusplus
extern "C" {
#endif

//int base64_encode(const void *data, int size, char **str);
//int base64_decode(const char *str, void *data);
int PW_Base64Dec(unsigned char * buf, const unsigned char * text, int size);
int PW_Base64Enc(unsigned char * buf, const unsigned char * text, int size);

#ifdef  __cplusplus
}
#endif

#endif

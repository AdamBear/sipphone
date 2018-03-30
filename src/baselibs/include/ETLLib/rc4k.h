
#ifndef   _RC4K_H_
#define   _RC4K_H_

#ifdef  __cplusplus
extern "C" {
#endif

void  PW_RC4K(unsigned char * key,  int key_len,   unsigned char * plaintest,  int plaintest_len,
              unsigned char * result);

#ifdef  __cplusplus
}
#endif

#endif     /*   rc4k.h   */


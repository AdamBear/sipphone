#ifndef COMPRESS_IMP_H
#define COMPRESS_IMP_H

#ifdef  __cplusplus
extern "C" {
#endif

int compress(void * src, unsigned src_len, void * dst);
int decompress(void * src, unsigned src_len, void * dst);

#ifdef  __cplusplus
}
#endif

#endif

/*
*  Copyright (C) 2014 Red Hat Inc.
*
*  Author: Vivek Goyal <vgoyal@redhat.com>
*
* This source code is licensed under the GNU General Public License,
* Version 2.  See the file COPYING for more details.
*/

#ifndef SHA256_H
#define SHA256_H

#ifdef  __cplusplus
extern "C" {
#endif

#include   <stdio.h>
#include   <string.h>

#define u32 unsigned int
#define u64 unsigned long long
#define u8  unsigned char

#define SHA256_DIGEST_SIZE      32
#define SHA256_BLOCK_SIZE       64

struct sha256_state
{
    u32 state[SHA256_DIGEST_SIZE / 4];
    u64 count;
    u8 buf[SHA256_BLOCK_SIZE];
};

int sha256_init(struct sha256_state * sctx);
int sha256_update(struct sha256_state * sctx, const unsigned char * input, unsigned int length);
int sha256_final(struct sha256_state * sctx, unsigned char * hash);

#ifdef  __cplusplus
}
#endif

#endif /* SHA256_H */

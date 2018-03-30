#pragma once

#define __NR_gettid

#ifdef __cplusplus
extern "C" {
#endif

int syscall();

#ifdef __cplusplus
}
#endif
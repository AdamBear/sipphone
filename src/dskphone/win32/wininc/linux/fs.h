#pragma once

#define _POSIX_ARG_MAX      4096
#define _POSIX_CHILD_MAX    6
#define _POSIX_LINK_MAX     8
#define _POSIX_MAX_CANON    255
#define _POSIX_MAX_INPUT    255
#define _POSIX_NAME_MAX     14
#define _POSIX_NGROUPS_MAX  0
#define _POSIX_OPEN_MAX     16
#define _POSIX_PATH_MAX     255
#define _POSIX_PIPE_BUF     512
#define _POSIX_SSIZE_MAX    32767
#define _POSIX_STREAM_MAX   8
#define _POSIX_TZNAME_MAX   3

#define ARG_MAX             14500       /* 16k heap, minus overhead */
#define LINK_MAX            1024
#define MAX_CANON           _POSIX_MAX_CANON
#define MAX_INPUT           _POSIX_MAX_INPUT
#define NAME_MAX            255
#define NGROUPS_MAX         16
#define OPEN_MAX            32
#define PATH_MAX            512
#define PIPE_BUF            _POSIX_PIPE_BUF
#define SSIZE_MAX           _POSIX_SSIZE_MAX
#define STREAM_MAX          20
#define TZNAME_MAX          10

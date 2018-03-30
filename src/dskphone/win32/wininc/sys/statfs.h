#pragma once

#define fsid_t long

struct statfs
{
    long    f_type;
    long    f_bsize;
    long    f_blocks;
    long    f_bfree;
    long    f_bavail;
    long    f_files;
    long    f_ffree;
    fsid_t  f_fsid;
    long    f_namelen;
};

#define statfs(path, buf)
#define fstatfs(fd, buf)

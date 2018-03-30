#pragma once

static _inline int mount(const char * source, const char * target,
                         const char * filesystemtype, unsigned long mountflags, const void * data)
{
    return -1;
}

static _inline int umount(const char * target)
{
    return -1;
}

static _inline int umount2(const char * target, int flags)
{
    return -1;
}

//#define mount(source, target, filesystemtype, mountflags, data) -1
//#define umount(target) -1
//#define umount2(target, flags) -1
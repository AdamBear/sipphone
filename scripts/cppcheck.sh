#!/bin/bash
# 本脚本实现：使用cppcheck对本次commit的代码进行静态检查

# 任意命令执行失败则退出
set -e

# 获取本次commit(HEAD)新增(A)或修改(M)的源码文件列表
COMMIT_FILES=`git show --pretty='' --name-status HEAD | grep -E '[AM]\s.*\.(c|cpp|cc|cxx)$' | cut -f 2`

# 如果源码文件列表为空，则跳过cppcheck检查
if [ ! "$COMMIT_FILES" ]
then
    echo 'No source file changed, ignore cppcheck.'
    exit 0
fi

echo 'Changed source files:' $COMMIT_FILES

# cppcheck代码检查
LOGIC_CPUS=`cat /proc/cpuinfo | grep "processor" | wc -l`
cppcheck --error-exitcode=1 --quiet --force -j$LOGIC_CPUS $COMMIT_FILES

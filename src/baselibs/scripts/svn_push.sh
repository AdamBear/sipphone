#!/bin/bash

# 设置为UTF-8编码，避免svn提交信息带有中文引起编码异常
export LC_CTYPE=C.UTF-8

# 获取git当前版本提交信息
COMMIT_MESSAGE=`git log --format=%B -n 1 $CI_BUILD_REF | cat -`$'\n'dist@$CI_BUILD_REF

# 获取当前分支对应的svn地址
get_svn_var()
{
    local CI_BUILD_REF_NAME_ESC
    CI_BUILD_REF_NAME_ESC="${CI_BUILD_REF_NAME/\//_}"
    local VAR_NAME
    VAR_NAME="SVN_$CI_BUILD_REF_NAME_ESC"
    printf %s "${!VAR_NAME}"
}
SVN_URL=$(get_svn_var)
if [ ! $SVN_URL ]
then
    echo 'Cannot get svn URL, ignore.'
    exit 0
fi
echo "Commit to svn: $SVN_URL"

# 任意命令执行失败则退出
set -e

# 把include目录提交到svn
svn checkout --non-interactive --trust-server-cert --no-auth-cache --username $SVN_USER --password $SVN_PASS $SVN_URL svn-dist
rm -rf svn-dist/*
cp -r include/* svn-dist
cd svn-dist
svn add --force .
svn commit --non-interactive --trust-server-cert --no-auth-cache --username $SVN_USER --password $SVN_PASS -m "$COMMIT_MESSAGE"

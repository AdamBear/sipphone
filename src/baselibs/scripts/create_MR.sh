#!/bin/bash

# 任意命令执行失败则退出
set -e

# 获取当前分支对应的git仓库信息
get_git_var()
{
    local CI_BUILD_REF_NAME_ESC
    CI_BUILD_REF_NAME_ESC="${CI_BUILD_REF_NAME/\//_}"
    local VAR_NAME
    VAR_NAME="GIT_GUI_$CI_BUILD_REF_NAME_ESC"
    printf %s "${!VAR_NAME}"
}
GIT_GUI_INFO_STR=$(get_git_var)
if [ ! $GIT_GUI_INFO_STR ]
then
    echo "Cannot get git info of GUI!"
    exit 0
fi 

# ';'作为分隔符，拆分为列表
IFS=';' read -ra GIT_GUI_INFOS <<< $GIT_GUI_INFO_STR
for GIT_GUI_INFO in "${GIT_GUI_INFOS[@]}"
do
    # ','作为分隔符，拆分为变量。如：develop,30,ssh://git@192.168.6.185:10022/app-common/gui.git
    IFS=',' read GIT_GUI_BRANCH GIT_GUI_ID GIT_GUI_URL <<< $GIT_GUI_INFO
    echo "target branch: $GIT_GUI_BRANCH"
    echo "gui project ID: $GIT_GUI_ID"
    echo "gui repo URL: $GIT_GUI_URL"

    # 克隆gui代码库
    git clone --recursive -b $GIT_GUI_BRANCH $GIT_GUI_URL gui-repo
    cd gui-repo

    # 移除远端baselibs_update分支
    set +e
    git branch -a | grep -wq 'remotes/origin/baselibs_update'
    if [ $? -eq 0 ]
    then
        git push origin --delete baselibs_update
    fi
    set -e

    # 创建并切换到baselibs_update分支
    git checkout -b baselibs_update

    # 更新baselibs版本
    git submodule update --remote src/baselibs
    git add src/baselibs
    git config user.email "git_bot@yealink.com"
    git config user.name "bot"

    # 提交并推送
    set +e # 可能版本没有变化，git commit会失败
    git commit -m "update baselibs version"
    GIT_COMMIT_RESULT=$?
    if [ 0 -eq $GIT_COMMIT_RESULT ]
    then
        # 提交成功才需要推送
        git push origin baselibs_update
    elif [ 1 -eq $GIT_COMMIT_RESULT ]
    then
        # 版本没有变化
        echo 'Nothing to commit.'
    else
        # 其他错误
        echo "Commit fail [$GIT_COMMIT_RESULT]!"
        exit $GIT_COMMIT_RESULT
    fi
    set -e
    cd ..
    rm -r gui-repo

    # 创建Merge-Request
    curl --header "PRIVATE-TOKEN:$BOT_TOKEN" \
      --data "id=$GIT_GUI_ID" \
      --data "target_project_id=$GIT_GUI_ID" \
      --data "source_branch=baselibs_update" \
      --data "target_branch=$GIT_GUI_BRANCH" \
      --data "title=update baselibs version" \
      --data "description=更新基础库版本" \
      "$GIT_API_URL$GIT_GUI_ID/merge_requests"
done

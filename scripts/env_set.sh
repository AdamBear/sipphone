if [ "$CI_BUILD_TAG" ]
then
    #remove [*] in "82.251.1(-Dcmakepara = sth)"
    CI_BUILD_TAG="${CI_BUILD_TAG%(*}"

    DOT_COUNTS=$(echo $CI_BUILD_TAG | tr -cd '.' | wc -c)
    if [ $DOT_COUNTS == "3" ]
    then
        # 获取tag指定的机型
        VER_PREFIX="${CI_BUILD_TAG%%.*}"

        # 大小写不敏感比较
        ANY_DEVICE="x"
        ANY_DEVICE_TMP=$(echo $ANY_DEVICE | tr   [a-z]   [A-Z])
        VER_PREFIX_TMP=$(echo $VER_PREFIX | tr   [a-z]   [A-Z])

        DEV_NAME="${CI_BUILD_NAME%pack}"
        DEV_NAME_TMP=$(echo $DEV_NAME | tr   [a-z]   [A-Z])

        CI_BUILD_TAG="${CI_BUILD_TAG#*.}"

        if [ "$VER_PREFIX_TMP" != "$ANY_DEVICE_TMP" -a "$VER_PREFIX_TMP" != "$DEV_NAME_TMP" ]
        then
            echo "tag指定了机型为【$VER_PREFIX】，当前机型为$DEV_NAME_TMP，不执行打包"
            exit 0
        fi
    fi
fi

# checkout PyBuildServerHelper
set -e
mkdir pyscripts
cd pyscripts
git clone ssh://git@appgit.yealink.com:10022/suw/PyBuildServerHelper.git -b master
cd ..

# set pyhton env
virtualenv venv
source venv/bin/activate
pip install lxml requests

# set version prefix

GUI_MODULE_ID="26"
GUI_MODULE_NAME="gui"
LIB_MODULE_ID="27"
LIB_MODULE_NAME="lib"
GUI_BLACK_MODULE_ID="401"
GUI_BLACK_MODULE_NAME="gui-black"
GUI_BLACK_COMMIT_ID="1156"

T23VER="44.${CI_BUILD_TAG}"
T23GUI_GUI_COMMIT_ID="414"
T23GUI_LIB_COMMIT_ID="1887"

T40GVER="76.${CI_BUILD_TAG}"
T40G_GUI_COMMIT_ID="414"
T40G_LIB_COMMIT_ID="1890"

T27T42VER="45.${CI_BUILD_TAG}"
T27T42_GUI_COMMIT_ID="413"
T27T42_LIB_COMMIT_ID="1889"

T27GVER="69.${CI_BUILD_TAG}"
T27G_GUI_COMMIT_ID="413"
T27G_LIB_COMMIT_ID="1888"

T29T46GVER="46.${CI_BUILD_TAG}"
T29T46G_GUI_COMMIT_ID="415"
T29T46G_LIB_COMMIT_ID="166"

T48GVER="48.${CI_BUILD_TAG}"
T48G_GUI_COMMIT_ID="416"
T48G_LIB_COMMIT_ID="202"

CP920VER="78.${CI_BUILD_TAG}"
CP920_GUI_COMMIT_ID="1518"
CP920_LIB_COMMIT_ID="1891"

T4XSVER="65.${CI_BUILD_TAG}"
T4XS_GUI_COMMIT_ID="1207"
T4XS_LIB_COMMIT_ID="1206"

T5XVER="54.${CI_BUILD_TAG}"
T5X_GUI_COMMIT_ID="1586"
T5X_LIB_COMMIT_ID="1587"

REMARK="\"remark\": \"Auto commit from gitlab, git sha: $CI_BUILD_REF\""
DIST_DIR="dist_${CI_BUILD_NAME%pack}"
echo "$DIST_DIR"

if [ "$CI_BUILD_NAME" = "t23pack" ]
then
    GUI_COMMIT_ID="$T23GUI_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T23VER"
    LIB_COMMIT_ID="$T23GUI_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t27pack" ]
then
    GUI_COMMIT_ID="$T27T42_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T27T42VER"
    LIB_COMMIT_ID="$T27T42_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "cp920pack" ]
then
    GUI_COMMIT_ID="$CP920_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$CP920VER"
    LIB_COMMIT_ID="$CP920_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t27gpack" ]
then
    GUI_COMMIT_ID="$T27G_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T27GVER"
    LIB_COMMIT_ID="$T27G_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t40gpack" ]
then
    GUI_COMMIT_ID="$T40G_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T40GVER"	
    LIB_COMMIT_ID="$T40G_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t46pack" ]
then
    GUI_COMMIT_ID="$T29T46G_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T29T46GVER"
    LIB_COMMIT_ID="$T29T46G_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t48pack" ]
then
    GUI_COMMIT_ID="$T48G_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T48GVER"
    LIB_COMMIT_ID="$T48G_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t4xspack" ]
then
    GUI_COMMIT_ID="$T4XS_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T4XSVER"
    LIB_COMMIT_ID="$T4XS_LIB_COMMIT_ID"
elif [ "$CI_BUILD_NAME" = "t54pack" ]
then
    GUI_COMMIT_ID="$T5X_GUI_COMMIT_ID"
    GUI_COMMIT_VER="$T5XVER"
    LIB_COMMIT_ID="$T5X_LIB_COMMIT_ID"
fi

echo "GUI_COMMIT_ID is ${GUI_COMMIT_ID}"
echo "GUI_COMMIT_VER is ${GUI_COMMIT_VER}"

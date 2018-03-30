. scripts/env_set.sh
mkdir dist_t5xs
mkdir dist_t5xs/bin/
mkdir dist_t5xs/lib/

mkdir dist_t5xs/bin/t4x/
mkdir dist_t5xs/lib/t4x/

cp dist_t54/bin/dskPhone.exx dist_t5xs/bin/t4x/
cp -r dist_t54/lib/. dist_t5xs/lib/t4x/
#cp dist_t54/bin/expcolor.exx dist_t5xs/bin/
mv dist_t5xs/lib/t4x/liblogic.so dist_t5xs/lib/
mv dist_t5xs/lib/t4x/libdskservice.so dist_t5xs/lib/
mv dist_t5xs/lib/t4x/libpresence.so dist_t5xs/lib/
CONFIGJSON="\
{\
	\"commit\": \
    [\
        {\
		\"file\": \"dist_t5xs/bin/\",\
		\"module_name\": \"$GUI_MODULE_NAME\",\
        \"module_id\":\"$GUI_MODULE_ID\",\
		\"commit_mode_id\":\"$GUI_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    },\
        {\
		\"file\": \"dist_t5xs/lib/\",\
		\"module_name\": \"$LIB_MODULE_NAME\",\
        \"module_id\":\"$LIB_MODULE_ID\",\
		\"commit_mode_id\":\"$LIB_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    }\
    ]\
}\
"
echo "$CONFIGJSON"
echo "$CONFIGJSON" | python3 pyscripts/PyBuildServerHelper/__main__.py -vvv --username="$AUTO_COMMIT_USER_NAME" --password="$AUTO_COMMIT_PWD" -c - commit --force

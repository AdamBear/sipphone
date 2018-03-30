. scripts/env_set.sh


mkdir dist_t4xs
mkdir dist_t4xs/bin/
mkdir dist_t4xs/lib/
mkdir dist_t4xs/bin/t48/
mkdir dist_t4xs/bin/t4x/
mkdir dist_t4xs/lib/t48/
mkdir dist_t4xs/lib/t4x/

cp dist_t48s/bin/dskPhone.exx dist_t4xs/bin/t48/
cp dist_t46s/bin/dskPhone.exx dist_t4xs/bin/t4x/

cp -r dist_t48s/lib/. dist_t4xs/lib/t48/
cp -r dist_t46s/lib/. dist_t4xs/lib/t4x/

# move liblogic.so & libpresence.so to public dir
cp dist_t4xs/lib/t4x/liblogic.so dist_t4xs/lib/
cp dist_t4xs/lib/t4x/libpresence.so dist_t4xs/lib/
cp dist_t4xs/lib/t4x/libdskservice.so dist_t4xs/lib/

# remove  liblogic.so & libpresence.so in sub dir
rm dist_t4xs/lib/t48/liblogic.so
rm dist_t4xs/lib/t48/libpresence.so
rm dist_t4xs/lib/t48/libdskservice.so
rm dist_t4xs/lib/t4x/liblogic.so
rm dist_t4xs/lib/t4x/libpresence.so
rm dist_t4xs/lib/t4x/libdskservice.so

CONFIGJSON="\
{\
	\"commit\": \
    [\
        {\
		\"file\": \"dist_t4xs/bin/\",\
		\"module_name\": \"$GUI_MODULE_NAME\",\
        \"module_id\":\"$GUI_MODULE_ID\",\
		\"commit_mode_id\":\"$GUI_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    },\
        {\
		\"file\": \"dist_t4xs/lib/\",\
		\"module_name\": \"$LIB_MODULE_NAME\",\
        \"module_id\":\"$LIB_MODULE_ID\",\
		\"commit_mode_id\":\"$LIB_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    },\
        {\
		\"file\": \"dist_t42s/bin/Screen.exx\",\
		\"module_name\": \"$GUI_BLACK_MODULE_NAME\",\
        \"module_id\":\"$GUI_BLACK_MODULE_ID\",\
		\"commit_mode_id\":\"$GUI_BLACK_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    }\
    ]\
}\
"
echo "$CONFIGJSON" | python3 pyscripts/PyBuildServerHelper/__main__.py -vvv --username="$AUTO_COMMIT_USER_NAME" --password="$AUTO_COMMIT_PWD" -c - commit --force

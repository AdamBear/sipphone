. scripts/env_set.sh

CONFIGJSON="\
{\
	\"commit\": \
    [\
        {\
		\"file\": \"$DIST_DIR/bin/Screen.exx\",\
		\"module_name\": \"$GUI_MODULE_NAME\",\
        \"module_id\":\"$GUI_MODULE_ID\",\
		\"commit_mode_id\":\"$GUI_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    },\
        {\
		\"file\": \"$DIST_DIR/lib/libdskservice.so\",\
		\"module_name\": \"$LIB_MODULE_NAME\",\
        \"module\":\"$LIB_MODULE_ID\",\
		\"mode\":\"$LIB_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    }\
    ]\
}\
"
echo "$CONFIGJSON"
echo "$CONFIGJSON" | python3 pyscripts/PyBuildServerHelper/__main__.py -vvv --username="$AUTO_COMMIT_USER_NAME" --password="$AUTO_COMMIT_PWD" -c - commit --force

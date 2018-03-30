. scripts/env_set.sh

CONFIGJSON="\
{\
	\"commit\": \
    [\
        {\
		\"file\": \"$DIST_DIR/bin/dskPhone.exx\",\
		\"module_name\": \"$GUI_MODULE_NAME\",\
        \"module_id\":\"$GUI_MODULE_ID\",\
		\"commit_mode_id\":\"$GUI_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    },\
        {\
		\"file\": \"$DIST_DIR/lib/\",\
		\"module_name\": \"$LIB_MODULE_NAME\",\
        \"module_id\":\"$LIB_MODULE_ID\",\
		\"commit_mode_id\":\"$LIB_COMMIT_ID\",\
		\"version\": \"$GUI_COMMIT_VER\",\
		$REMARK\
	    }\
    ]\
}\
"
echo "$CONFIGJSON" | python3 pyscripts/PyBuildServerHelper/__main__.py -vvv --username="$AUTO_COMMIT_USER_NAME" --password="$AUTO_COMMIT_PWD" -c - commit --force


#Joyce Nology Simplesignal, Telstra,UC
#if(JOYCE OR NOLOGY OR SIMPLESIGNAL OR TELSTRA OR YUK_BSFT)
#    set(BroadTouchUC ON)
#endif()

#YUK Special
if(YUK)
    add_definitions(-DTYN)
endif()

if(M2 OR TELSTRA)
    set(FEATURE_CFG_PROTECT ON)
endif()

#Metaswitch
if(T46S OR T48S OR T42S OR T54 OR T27G OR T46 OR T40G)
    message ("-=-=-=-=  Metaswitch Version  =-=-=-=-")
    set(ENABLE_JSON ON)
    set(FEATURE_METASWITCH 1)
    unset(Metaswitch CACHE)
endif()

#Genband
if(T4X OR T27G)
	message ("Set On Genband PAB And GAB")
	set(FEATURE_GBBOOK 1)
endif()

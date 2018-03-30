if(_WIFI OR _BLUETOOTH)

# 蓝牙开关
option(ENABLE_NEW_BLUETOOTH "bluetooth模块使用新版代码" ON)
option(ENABLE_TEST_BT_LOGIC "bluetooth模块测试内部模块代码" OFF)
option(ENABLE_TEST_BT_DRIVER "bluetooth模块模拟测试驱动" OFF)
option(BT_NEW_CHANNEL_MODE "BT使用T49通道切换逻辑" ON)

#蓝牙测试代码
if(ENABLE_TEST_BT_LOGIC)
	set(ENABLE_BT_TEST_LOGIC ON)
	add_definitions(-DIF_BT_TEST_LOGIC)
endif()

#模拟驱动代码
if(ENABLE_TEST_BT_DRIVER)
	set(ENABLE_BT_TEST_LOGIC ON)
	add_definitions(-DIF_BT_TEST_LOGIC)

	set(ENABLE_BT_SIMULATION_DRIVER ON)
	add_definitions(-DIF_BT_SIMULATION_DRIVER)
endif()

if(BT_NEW_CHANNEL_MODE)
	message("** Bluetooth info: New channel mode(the same as t49 used) ")
	add_definitions(-DIF_NEW_CHANNEL_MODE)
endif()

#包含文件
include_directories(
    .
    wireless
    )

add_all_subdirectories()
else()
	add_fake_directory()
endif()

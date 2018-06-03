cmake_minimum_required(VERSION 2.8)

project(mj)

if(UNIX)
	add_definitions(-D_LINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -fPIC -m64 -std=c++11")
else()
	add_definitions(-DCURL_STATICLIB)		#windows下使用curl静态库时需要此宏定义
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /wd\"4819\" /wd\"4267\"")			#忽略文件编码警告4819 size_t转int4267
endif(UNIX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

message("src path:${PROJECT_SOURCE_DIR}")
message("bin path:${EXECUTABLE_OUTPUT_PATH}")
message("lib path:${LIBRARY_OUTPUT_PATH}")

option(heartbeat "check heartbeat" ON)
if(NOT heartbeat)
	add_definitions(-DUNCHECKHEARTBEAT)
endif()

#include_directories(${PROJECT_SOURCE_DIR}/app/mysql_connect_c++/include)

#link_directories(${PROJECT_SOURCE_DIR}/app/mysql_connect_c++/lib64)mysqlcppconn

set(LBASE_NAME "base")

if(UNIX)
	set(PT_LIB iconv pthread dl curl crypto boost_serialization boost_thread boost_system boost_regex boost_filesystem)
else()
	set(PT_LIB libcurl ssleay32 libeay32)
endif()

set(LINK_LIBS ${LBASE_NAME} ${PT_LIB} mysqlclient)

set(PREFIX_NAME "XL")
set(CENTER_SERVER_NAME "${PREFIX_NAME}CenterServer")
set(LOGIC_DB_NAME "${PREFIX_NAME}LogicDB")
set(LOGIC_MANAGER_NAME "${PREFIX_NAME}LogicManager")
set(LOGIC_SERVER_NAME "${PREFIX_NAME}LogicServer")
set(GATE_SERVER_NAME "${PREFIX_NAME}GateServer")
set(LOGIN_GATE_NAME "${PREFIX_NAME}LoginServer")
set(COINS_SERVER_NAME "${PREFIX_NAME}CoinsServer")
set(DDZ_SERVER_NAME "${PREFIX_NAME}DDZServer")

include_directories(extlib/include)
include_directories(LBase)

add_subdirectory(LBase)
add_subdirectory(CenterServer)
add_subdirectory(LogicDB)
add_subdirectory(LogicManager)
add_subdirectory(LogicServer)
add_subdirectory(GateServer)
add_subdirectory(LoginServer)
#add_subdirectory(CoinsServer)
add_subdirectory(DDZServer)
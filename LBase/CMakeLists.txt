#LBase

set(JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extlib/src/lib_json)

set(JSON_CODE	${JSON_DIR}/json_value.cpp
				${JSON_DIR}/json_reader.cpp
				${JSON_DIR}/json_writer.cpp
)

if(UNIX)
	set(PT_CODE		Func.cpp)
else()
	set(PT_CODE		MiniDump.cpp)
endif(UNIX)

set(LBASE_CODE	LActive.cpp
				LCharge.cpp
				LIni.cpp
				LLog.cpp
				LMsg.cpp
				LNet.cpp
				LRunnable.cpp
				LSocket.cpp
				LTabFile.cpp
				LTime.cpp
				LTool.cpp
				LUser.cpp
				LVideo.cpp
				LVipLog.cpp
				BaseCommonFunc.cpp
				IPFilter.cpp
				LDBSession.cpp
				LMemeoryRecycle.cpp
				LRedisClient.cpp
				RLogHttp.cpp
)

add_library(${LBASE_NAME} STATIC ${JSON_CODE} ${LBASE_CODE} ${PT_CODE})
#LogicDB

set(LOGIC_DB_CODE	Work.cpp
					UserMessageMsg.cpp
					UserMessage.cpp
					main.cpp
					InsideNet.cpp
					DbServerManager.cpp
					DbServer.cpp
					Config.cpp
					ActiveManager.cpp
)

add_executable(${LOGIC_DB_NAME} ${LOGIC_DB_CODE})
target_link_libraries(${LOGIC_DB_NAME} ${LINK_LIBS})
#LogicManager

set(LOGIC_MANAGER_CODE	Work.cpp
						UserMessageMsg.cpp
						UserMessage.cpp
						UserManager.cpp
						User.cpp
						RuntimeInfoMsg.cpp
						main.cpp
						InsideNet.cpp
						DeskManager.cpp
						Config.cpp
						ActiveManager.cpp
)

add_executable(${LOGIC_MANAGER_NAME} ${LOGIC_MANAGER_CODE})
target_link_libraries(${LOGIC_MANAGER_NAME} ${LINK_LIBS})
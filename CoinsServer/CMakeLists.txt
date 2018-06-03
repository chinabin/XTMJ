#CoinsServer

set(COINS_SERVER_CODE	Work.cpp
						UserMessageMsg.cpp
						UserMessage.cpp
						UserManager.cpp
						User.cpp
						RobotManager.cpp
						Robot.cpp
						main.cpp
						InsideNet.cpp
						DeskManager.cpp
						Desk.cpp
						Config.cpp
)

add_executable(${COINS_SERVER_NAME} ${COINS_SERVER_CODE})
target_link_libraries(${COINS_SERVER_NAME} ${LINK_LIBS})
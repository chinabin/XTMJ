#CenterServer

set(CENTER_SERVER_CODE	Work.cpp
						UserMessageMsg.cpp
						UserMessage.cpp
						UserManager.cpp
						NewUserVerify.cpp
						main.cpp
						InsideNet.cpp
						HttpRequest.cpp
						DUser.cpp
						DbServerManager.cpp
						Config.cpp
						DbServer.cpp
						NewUserVerifyManager.cpp
)

add_executable(${CENTER_SERVER_NAME} ${CENTER_SERVER_CODE})
target_link_libraries(${CENTER_SERVER_NAME} ${LINK_LIBS})
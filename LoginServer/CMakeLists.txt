#LoginServer

set(LOGIN_GATE_CODE		Work.cpp
						InsideNet.cpp
						main.cpp
						HttpRequest.cpp
						Config.cpp
)

add_executable(${LOGIN_GATE_NAME} ${LOGIN_GATE_CODE})
target_link_libraries(${LOGIN_GATE_NAME} ${LINK_LIBS})

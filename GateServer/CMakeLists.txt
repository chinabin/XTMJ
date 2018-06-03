#GateServer

set(GATE_SERVER_CODE	Work.cpp
						OutsideNet.cpp
						main.cpp
						GateUser.cpp
						Config.cpp
)

add_executable(${GATE_SERVER_NAME} ${GATE_SERVER_CODE})
target_link_libraries(${GATE_SERVER_NAME} ${LINK_LIBS})
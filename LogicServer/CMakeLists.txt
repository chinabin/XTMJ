#LogicServer

set(LOGIC_SERVER_CODE	Work.cpp
						User.cpp
						RoomVip.cpp
						RobotManager.cpp
						InsideNet.cpp
						Card.cpp
						CardBase.cpp
						GameHandler.cpp
						Desk.cpp
						CounterManager.cpp
						Config.cpp
						ActiveManager.cpp
						UserManager.cpp
						Robot.cpp
						main.cpp
						Counter.cpp
)

option(ZhuanZhuan "ZhuanZhuan" ON)
if(ZhuanZhuan)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_ZhuanZhuan.cpp GH_HeNan_ZhuanZhuan.cpp)
endif()

option(ZhengZhou "ZhengZhou" OFF)
if(ZhengZhou)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_ZhengZhou.cpp GH_HeNan_ZhengZhou.cpp)
endif()

option(XinYang "XinYang" OFF)
if(XinYang)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_XinYang.cpp GH_HeNan_XinYang.cpp)
endif()

option(XinXiang "XinXiang" OFF)
if(XinXiang)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_XinXiang.cpp GH_HeNan_XinXiang.cpp)
endif()

option(TuiDaoHu "TuiDaoHu" OFF)
if(TuiDaoHu)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_TuiDaoHu.cpp GH_HeNan_TuiDaoHu.cpp)
endif()

option(LuoYang "LuoYang" OFF)
if(LuoYang)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_LuoYang.cpp GH_HeNan_LuoYang.cpp)
endif()

option(LuoHe "LuoHe" OFF)
if(LuoHe)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_LuoHe.cpp GH_HeNan_LuoHe.cpp)
endif()

option(KaiFeng "KaiFeng" OFF)
if(KaiFeng)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_KaiFeng.cpp GH_HeNan_KaiFeng.cpp)
endif()

option(HuaShui "HuaShui" OFF)
if(HuaShui)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_HuaShui.cpp GH_HeNan_HuaShui.cpp)
endif()

option(ShangQiu "ShangQiu" ON)
if(ShangQiu)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} CH_HN_ShangQiu.cpp GH_HeNan_ShangQiu.cpp)
endif()

option(ZhouKou "ZhouKou" ON)
if(ZhouKou)
	set(LOGIC_SERVER_CODE ${LOGIC_SERVER_CODE} HeNan_ZhouKou_CH.cpp HeNan_ZhouKou_GH.cpp)
endif()

add_executable(${LOGIC_SERVER_NAME} ${LOGIC_SERVER_CODE})
target_link_libraries(${LOGIC_SERVER_NAME} ${LINK_LIBS})
CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

IF(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  SET(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
ENDIF()

MACRO(QT_BEGIN)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
ENDMACRO(QT_BEGIN)

MACRO(QT_END)
SET(CMAKE_AUTOMOC OFF)
SET(CMAKE_AUTOUIC OFF)
SET(CMAKE_AUTORCC OFF)
ENDMACRO(QT_END)

PROJECT(Atrc)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Source/Lib/Utils/Src/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Source/)

SET(CMAKE_CXX_STANDARD 17)

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

FILE(GLOB_RECURSE LIB_SRC "Source/Atrc/Lib/*.cpp" "Source/Atrc/Lib/*.h")
ADD_LIBRARY(Lib STATIC ${LIB_SRC})
FOREACH(_SRC IN ITEMS ${LIB_SRC})
    GET_FILENAME_COMPONENT(LIB_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Atrc/Lib" "" _GRP_PATH "${LIB_SRC}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

FILE(GLOB_RECURSE MGR_SRC "Source/Atrc/Mgr/*.cpp" "Source/Atrc/Mgr/*.h")
ADD_LIBRARY(Mgr STATIC ${MGR_SRC})
FOREACH(_SRC IN ITEMS ${MGR_SRC})
    GET_FILENAME_COMPONENT(MGR_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Atrc/Mgr" "" _GRP_PATH "${MGR_SRC}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

FILE(GLOB_RECURSE IMGUI_SRC "Source/Lib/imgui/*.cpp" "Source/Lib/imgui/*.h")
ADD_LIBRARY(ImGui STATIC ${IMGUI_SRC})
TARGET_INCLUDE_DIRECTORIES(ImGui PRIVATE
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS})

FILE(GLOB_RECURSE AGZUTILS_SRC "Source/Lib/AGZUtils/*.cpp" "Source/Lib/Utils/Src/*.h" "Source/Lib/Utils/Src/*.cpp")
ADD_LIBRARY(AGZUtils STATIC ${AGZUTILS_SRC})
FOREACH(_SRC IN ITEMS ${AGZUTILS_SRC})
    GET_FILENAME_COMPONENT(AGZUTILS_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Lib/Utils/Src/AGZUtils" "Header" _GRP_PATH_0 "${AGZUTILS_SRC}")
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Lib/AGZUtils" "Impl" _GRP_PATH "${_GRP_PATH_0}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()
TARGET_INCLUDE_DIRECTORIES(AGZUtils PRIVATE
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS})

FILE(GLOB_RECURSE LAUNCHER_SRC "Source/Atrc/Launcher/*.cpp" "Source/Atrc/Launcher/*.h")
ADD_EXECUTABLE(Launcher ${LAUNCHER_SRC})
TARGET_LINK_LIBRARIES(Launcher Lib Mgr AGZUtils)

FILE(GLOB_RECURSE EDITOR_SRC "Source/Atrc/Editor/*.cpp" "Source/Atrc/Editor/*.h")
ADD_EXECUTABLE(Editor ${EDITOR_SRC})
FOREACH(_SRC IN ITEMS ${EDITOR_SRC})
    GET_FILENAME_COMPONENT(EDITOR_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Atrc/Editor" "" _GRP_PATH "${EDITOR_SRC}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

TARGET_INCLUDE_DIRECTORIES(Editor PRIVATE
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(Editor PRIVATE
	ImGui GLEW::GLEW glfw ${OPENGL_LIBRARIES}
	Lib Mgr AGZUtils)

FILE(GLOB_RECURSE SH2D_SRC "Source/Atrc/SH2D/*.cpp" "Source/Atrc/SH2D/*.h")
ADD_EXECUTABLE(SH2D ${SH2D_SRC})
FOREACH(_SRC IN ITEMS ${SH2D_SRC})
    GET_FILENAME_COMPONENT(SH2D_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Atrc/SH2D" "" _GRP_PATH "${SH2D_SRC}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()
TARGET_LINK_LIBRARIES(SH2D PRIVATE Lib Mgr AGZUtils)

QT_BEGIN()

FILE(GLOB_RECURSE ATRC_SRC "Source/Atrc/Atrc/*.cpp" "Source/Atrc/Atrc/*.h"
						   "Source/Atrc/Atrc/*.ui" "Source/Atrc/Atrc/*.qrc")
ADD_EXECUTABLE(Atrc ${ATRC_SRC})
FOREACH(_SRC IN ITEMS ${ATRC_SRC})
    GET_FILENAME_COMPONENT(ATRC_SRC "${_SRC}" PATH)
    STRING(REPLACE "${CMAKE_SOURCE_DIR}/Source/Atrc/Atrc" "" _GRP_PATH "${ATRC_SRC}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()
TARGET_LINK_LIBRARIES(Atrc PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

if(WIN32)
	ADD_CUSTOM_COMMAND(TARGET Atrc POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core>    $<TARGET_FILE_DIR:Atrc>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui>     $<TARGET_FILE_DIR:Atrc>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:Atrc>)
endif(WIN32)

QT_END()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PTZ_CLIENT_SRCS)

SET(CMAKE_AUTOMOC ON)


find_package(Boost REQUIRED COMPONENTS system filesystem thread regex timer chrono)
if(NOT Boost_FOUND)
	message(WARNING "fail to find Boost")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	SET(LIBS ${LIBS} ${Boost_LIBRARIES})
	message("Boost include: ${Boost_INCLUDE_DIRS}")
	message("Boost library: ${Boost_LIBRARY_DIRS}")
	message("Boost lib: ${Boost_LIBRARIES}")
endif()

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
	message(WARNING "fail to find OpenCV")
else()
	include_directories(${OpenCV_INCLUDE_DIRS})
	SET(LIBS ${LIBS} ${OpenCV_LIBS})
	message(STATUS "OpenCV library status:")
	message(STATUS "    version: ${OpenCV_VERSION}")
endif()


find_package(VISCA REQUIRED)
if(NOT VISCA_FOUND)
	message("fail to find OpenCV")
else()
	include_directories(${VISCA_INCLUDE_DIRS})
	SET(LIBS ${LIBS} ${VISCA_LIBRARY})
	message(STATUS "find VISCA library")
endif()

find_package(REMOTE_CAMERA REQUIRED)
if(NOT REMOTE_CAMERA_FOUND)
	message("fail to find remote_camera")
else()
	include_directories(${REMOTE_CAMERA_INCLUDE_DIRS})
	SET(LIBS ${LIBS} ${REMOTE_CAMERA_LIBRARY})
	message(STATUS "find remote_camera library")
endif()


FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)

set(QRC_SOURCE_FILES ptz_client.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

qt5_wrap_ui(UI_HEADERS ptz_client.ui)
ADD_EXECUTABLE(ptz_client ${PTZ_CLIENT_SRCS} ${UI_HEADERS} ${QRC_FILES})
TARGET_LINK_LIBRARIES(ptz_client 
	ptz_track
	${LIBS}
	${Qt5Core_QTMAIN_LIBRARIES})
qt5_use_modules(ptz_client Widgets Gui)


INSTALL(TARGETS ptz_client DESTINATION bin)


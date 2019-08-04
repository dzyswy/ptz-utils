########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.14.2)
PROJECT(ptz_track CXX)
ENABLE_TESTING()


INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../ptz_track_version.txt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ptz)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pid)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PTZ_TRACK_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ptz PTZ_TRACK_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pid PTZ_TRACK_SRCS)

find_package(Boost REQUIRED COMPONENTS system filesystem thread regex timer chrono)
if(NOT Boost_FOUND)
	message(WARNING "fail to find Boost")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
#	SET(LIBS ${LIBS} ${Boost_LIBRARIES})
	message("Boost include: ${Boost_INCLUDE_DIRS}")
	message("Boost library: ${Boost_LIBRARY_DIRS}")
	message("Boost lib: ${Boost_LIBRARIES}")
endif()

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
	message(WARNING "fail to find OpenCV")
else()
	include_directories(${OpenCV_INCLUDE_DIRS})
#	SET(LIBS ${LIBS} ${OpenCV_LIBS})
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


ADD_LIBRARY(ptz_track SHARED ${PTZ_TRACK_SRCS})
ADD_LIBRARY(ptz_track_static STATIC ${PTZ_TRACK_SRCS})
TARGET_LINK_LIBRARIES(ptz_track ${LIBS})
set_target_properties(ptz_track_static PROPERTIES OUTPUT_NAME "ptz_track")
set_target_properties(ptz_track PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ptz_track_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ptz_track PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set_target_properties(ptz_track_static PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

IF(UNIX)
    SET_PROPERTY(TARGET ptz_track PROPERTY VERSION ${PTZ_TRACK_VERSION})
    SET_PROPERTY(TARGET ptz_track PROPERTY SOVERSION ${PTZ_TRACK_VERSION_MAJOR})
ENDIF(UNIX)
INSTALL(TARGETS ptz_track DESTINATION lib)
#INSTALL(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ptz_track.h DESTINATION include)
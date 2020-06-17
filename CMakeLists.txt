cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0079 NEW)

project(SVM)

find_package(ABY QUIET)
if(ABY_FOUND)
	message(STATUS "FOUND ABY")
elseif(NOT ABY_FOUND AND NOT TARGET ABY::aby)
	message("ABY was not found: add ABY subdirectory")
	add_subdirectory(ABY)
endif()

find_package(RELIC QUIET)
if(RELIC_FOUND)
	message(STATUS "FOUND RELIC")
elseif(NOT RELIC_FOUND AND NOT TARGET RELIC::relic)
	message("RELIC was not found: add RELIC subdirectory")
	add_subdirectory(extern/relic)
endif()

find_package(OpenCV QUIET)
if(OpenCV_FOUND)
	message(STATUS "FOUND OpenCV")
elseif(NOT OpenCV_FOUND AND NOT TARGET OpenCV::opencv)
	message("OpenCV was not found: add OpenCV subdirectory")
	add_subdirectory(opencv)
endif()

find_package(Threads REQUIRED)
find_package(Boost 1.66.0 REQUIRED COMPONENTS thread system)
find_package(ENCRYPTO_utils QUIET)

add_library(RELIC::relic STATIC IMPORTED)
set_target_properties(RELIC::relic PROPERTIES 
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/relic/build/lib/librelic_s.a"
)

add_executable(SVM svm_data_test.cpp)
target_link_libraries(SVM ABY::aby)
target_link_libraries(SVM RELIC::relic)
# target_link_libraries(SVM OpenCV::opencv)

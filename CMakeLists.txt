project(resotto)
cmake_minimum_required(VERSION 3.0)

set(RESOTTO_VERSION_MAJOR "1")
set(RESOTTO_VERSION_MINOR "0")
add_definitions( -DRESOTTO_VERSION_MAJOR=\"${RESOTTO_VERSION_MAJOR}\" -DRESOTTO_VERSION_MINOR=\"${RESOTTO_VERSION_MINOR}\")


option(RESOTTO_UNIT_TESTS "Enable or disable unit tests execution" TRUE)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)


include(CTest)
include(GNUInstallDirs)
include(ReleaseDebugAutoFlags)


## Dependencies
find_package(Threads QUIET)
find_package(Boost REQUIRED COMPONENTS system)


file(GLOB RESOTTO_HEADERS_1 "${CMAKE_CURRENT_SOURCE_DIR}/include/resotto/*.hpp")
file(GLOB RESOTTO_HEADERS_2 "${CMAKE_CURRENT_SOURCE_DIR}/include/resotto/*/*.hpp")


include_directories(SYSTEM ${BOOST_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/  )

if(NOT DEFINED HADOKEN_INCLUDE_DIRS)
    set(HADOKEN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/deps/hadoken/include)
endif()
include_directories( ${HADOKEN_INCLUDE_DIRS} )

set(resotto_version_src "${CMAKE_CURRENT_SOURCE_DIR}/src/resotto_demo.cpp")
add_executable(resotto_demo ${resotto_version_src} )
target_link_libraries(resotto_demo Boost::boost Boost::system ${CMAKE_THREAD_LIBS_INIT})






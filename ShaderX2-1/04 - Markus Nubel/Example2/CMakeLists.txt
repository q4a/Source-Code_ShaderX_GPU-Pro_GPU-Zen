cmake_minimum_required(VERSION 3.5)
project(example2)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

### Set up output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/utils.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(conan)
### for CONAN_PKG::directx
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)
conan_cmake_run(CONANFILE conanfile.py
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

# Source files

list(APPEND PROJECT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND PROJECT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../base")
add_dir("${PROJECT_DIRS}" "${PROJECT_NAME}")

add_executable(${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCE} ${${PROJECT_NAME}_HEADER})

# Dependencies

if (WIN32)
    message("Using Windows D3D9 API")
    set(NATIVE_D3D9_LIBS "CONAN_PKG::directx")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    comctl32
    winmm
    ${NATIVE_D3D9_LIBS}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../media/height1025.raw ${PROJECT_BINARY_DIR}/media/height1025.raw COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../media/terrain.jpg ${PROJECT_BINARY_DIR}/media/terrain.jpg COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ExpFog.vsh ${PROJECT_BINARY_DIR}/bin/ExpFog.vsh COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ExpFog.psh ${PROJECT_BINARY_DIR}/bin/ExpFog.psh COPYONLY)

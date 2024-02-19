cmake_minimum_required(VERSION 3.25)
project(HWIDSpoof)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library (${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_compile_definitions(PROJECT_NAME="HWIDSpoof")
add_compile_definitions(PROJECT_NAME_STRICT="HWIDSpoof")
add_compile_definitions(PROJECT_NAME_STRICT_LOWER="hwid-spoof")
add_compile_definitions(PROJECT_NAME_SHORT="hSpoof")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(_USE_MATH_DEFINES)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".asi"
)

include(FindRakHook)
add_subdirectory ("3rdparty/kthook")
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        RakHook
        kthook
        OpenSSL::SSL OpenSSL::Crypto
)
cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 14)

project(alpg)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(PostgreSQL REQUIRED)
find_package(CURL REQUIRED)

include_directories(
    ${CURL_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}
    ${PostgreSQL_SERVER_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(vendor/cjson)

target_link_libraries(alpg PRIVATE ${CURL_LIBRARIES})
target_link_libraries(alpg PRIVATE cjson)
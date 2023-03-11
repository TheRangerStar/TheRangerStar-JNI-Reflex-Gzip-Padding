
cmake_minimum_required(VERSION 3.22.1)


project("http")


add_library(
        http
        SHARED
        native-lib.cpp)
add_library(cryptor SHARED cryptor/jni_tool.cpp cryptor/base64.c)
find_library(
        log-lib
        log)

target_link_libraries(
        http
        ${log-lib}
        z
)
target_link_libraries(
        cryptor
        ${log-lib}
        z
)
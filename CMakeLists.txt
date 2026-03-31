cmake_minimum_required(VERSION 3.20)
project(bindings)

find_package(ortools CONFIG REQUIRED)

add_executable(bindings bindings.cpp)
target_compile_options(bindings PUBLIC -pthread)
set_target_properties(bindings PROPERTIES LINK_FLAGS "-lembind -pthread --no-entry --emit-tsd bindings.d.ts -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=web,webview,worker -sSINGLE_FILE=1 -sINITIAL_MEMORY=4080271360 -sPTHREAD_POOL_SIZE_STRICT=1 -sPTHREAD_POOL_SIZE='2 * navigator.hardwareConcurrency + 2'")

target_link_libraries(bindings ortools::ortools)
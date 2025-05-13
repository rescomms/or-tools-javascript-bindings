cmake_minimum_required(VERSION 3.20)
project(bindings)

find_package(ortools CONFIG REQUIRED)

add_executable(bindings bindings.cpp)
set_target_properties(bindings PROPERTIES LINK_FLAGS "-lembind --no-entry --emit-tsd bindings.d.ts -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=web,webview,worker -sSINGLE_FILE=1 -sALLOW_MEMORY_GROWTH=1")

target_link_libraries(bindings ortools::ortools)
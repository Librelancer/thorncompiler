cmake_minimum_required (VERSION 3.8)

project ("thorncompiler") 

add_library (thorncompiler SHARED
    "lua/src/luac/dump.c"
    "lua/src/luac/opcode.c"
    "lua/src/luac/stubs.c"
    "lua/src/lbuffer.c"
    "lua/src/lfunc.c"
    "lua/src/llex.c"
    "lua/src/lmem.c"
    "lua/src/lobject.c"
    "lua/src/lparser.c"
    "lua/src/lstate.c"
    "lua/src/lstring.c"
    "lua/src/lzio.c"
    "lua/src/lundump.c"
    "src/thorncompiler.c"
)
target_include_directories(thorncompiler PUBLIC "include" "lua/include" "lua/src")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    # link libgcc/libstdc++ into our .dll
    target_link_options(thorncompiler PRIVATE -static-libgcc -static)
endif()


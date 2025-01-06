cmake_minimum_required(VERSION 3.21)
project(toolbox)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")

# 针对 Debug 模式设置编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# 针对 Release 模式设置编译选项
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")


# -----------------------头文件 三方库-----------------------
include_directories(third/httpLib)
include_directories(third/zlib)
include_directories(third/libzip)
include_directories(third/json)

# ----------------------头文件 业务-------------------------
include_directories(src/rc)
include_directories(src/tray)
include_directories(src/template)
include_directories(src/log)
include_directories(src/staticFile)
include_directories(src/httpServer)
include_directories(src/setting)

# ------------------------源代码------------------------
if (WIN32)
    set(SRC_LIST src/rc/resource.rc)
endif ()
aux_source_directory(src SRC_LIST)
aux_source_directory(src/tray SRC_LIST)
aux_source_directory(src/template SRC_LIST)
aux_source_directory(src/log SRC_LIST)
aux_source_directory(src/staticFile SRC_LIST)
aux_source_directory(src/httpServer SRC_LIST)
aux_source_directory(src/setting SRC_LIST)

# ------------------------生成程序------------------------
if (WIN32)
    add_executable(toolbox WIN32 ${SRC_LIST}) # WIN32 无控制台
else ()
    add_executable(toolbox ${SRC_LIST})
endif ()

# ------------------------链接库------------------------
target_link_libraries(toolbox
        ${CMAKE_SOURCE_DIR}/third/zlib/zlibstatic.lib
        ${CMAKE_SOURCE_DIR}/third/libzip/zip.lib)


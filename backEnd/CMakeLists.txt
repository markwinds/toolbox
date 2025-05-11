cmake_minimum_required(VERSION 3.21)
project(toolbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 禁用安全函数相关警告
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# 使用外部传入的版本信息
if (DEFINED TOOLBOX_VERSION)
    # 将宏传递给编译器
    add_definitions(-DTOOLBOX_VERSION="${TOOLBOX_VERSION}")
endif ()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections") # 函数级别切块
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto") # 链接时优化

add_definitions(-DCURL_STATICLIB)  # 添加 CURL_STATICLIB 宏定义

# 针对 Debug 模式设置编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# 针对 Release 模式设置编译选项
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")


# -----------------------头文件 三方库-----------------------
include_directories(third/inc)
include_directories(third/inc/zlib)
include_directories(third/inc/libzip)
include_directories(third/inc/json)
include_directories(third/inc/jsoncpp)
include_directories(third/inc/drogon)
include_directories(third/inc/curl)

# ----------------------头文件 业务-------------------------
include_directories(src/rc)
include_directories(src/tray)
include_directories(src/template)
include_directories(src/log)
include_directories(src/staticFile)
include_directories(src/httpServer)
include_directories(src/setting)
include_directories(src/util)

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
aux_source_directory(src/util SRC_LIST)

# ------------------------生成程序------------------------
if (WIN32)
    add_executable(toolbox WIN32 ${SRC_LIST}) # WIN32 无控制台
else ()
    add_executable(toolbox ${SRC_LIST})
endif ()

# ------------------------链接库------------------------
target_link_libraries(toolbox
        ${CMAKE_SOURCE_DIR}/third/lib/drogon.lib
        ${CMAKE_SOURCE_DIR}/third/lib/jsoncpp.lib
        ${CMAKE_SOURCE_DIR}/third/lib/trantor.lib
        ${CMAKE_SOURCE_DIR}/third/lib/zlibstatic.lib
        ${CMAKE_SOURCE_DIR}/third/lib/zip.lib
        ${CMAKE_SOURCE_DIR}/third/lib/libcurl.lib
        ${CMAKE_SOURCE_DIR}/third/lib/libssl.lib
        ${CMAKE_SOURCE_DIR}/third/lib/libcrypto.lib
        crypt32
        ws2_32
        rpcrt4)


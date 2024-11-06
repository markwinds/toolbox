cmake_minimum_required(VERSION 3.21)
project(toolbox)

set(CMAKE_CXX_STANDARD 14)

# -----------------------头文件 三方库-----------------------
include_directories(third/httpLib)

# ----------------------头文件 业务-------------------------
include_directories(src/rc)
include_directories(src/tray)
include_directories(src/template)
include_directories(src/log)

# ------------------------源代码------------------------
if (WIN32)
    set(SRC_LIST src/rc/resource.rc)
endif ()
aux_source_directory(src SRC_LIST)
aux_source_directory(src/tray SRC_LIST)
aux_source_directory(src/template SRC_LIST)
aux_source_directory(src/log SRC_LIST)

# ------------------------生成程序------------------------
if (WIN32)
    add_executable(toolbox WIN32 ${SRC_LIST}) # WIN32 无控制台
else ()
    add_executable(toolbox ${SRC_LIST})
endif ()

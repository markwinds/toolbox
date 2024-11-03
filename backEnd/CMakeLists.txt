cmake_minimum_required(VERSION 3.21)
project(toolbox)

set(CMAKE_CXX_STANDARD 14)

# 头文件 三方库
include_directories(third/httpLib)

# 头文件 业务
include_directories(src/rc)

# 源代码
set(SRC_LIST src/rc/resource.rc)
aux_source_directory(src SRC_LIST)
aux_source_directory(src/rc SRC_LIST)

add_executable(toolbox ${SRC_LIST})

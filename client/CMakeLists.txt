# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.10)

#项目信息
project(client)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

#添加子目录
add_subdirectory(source)

# 指定生成目标 
add_executable(client main.cpp)

#添加lpthread链接库
TARGET_LINK_LIBRARIES(client pthread)

# 生成链接库
TARGET_LINK_lIBRARIES(client source_lib)


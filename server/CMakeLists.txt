# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.10)

# 项目信息
project (server)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加子目录
add_subdirectory(source)

# 指定生成目标 
add_executable(server main.cpp)

#添加lpthread链接库
TARGET_LINK_LIBRARIES(server pthread)

# 添加链接库
target_link_libraries(server source_lib)



cmake_minimum_required(VERSION 3.16)
project(ChatCraftServer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置可执行文件和库的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")  # 设置可执行文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")  # 设置动态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")  # 设置静态库输出目录

add_subdirectory(common)
add_subdirectory(GateServer)
add_subdirectory(StatusServer)
add_subdirectory(ChatServer)



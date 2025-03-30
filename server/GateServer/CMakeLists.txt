project(GateServer)

# 项目基础目录配置
set(project_root ${CMAKE_CURRENT_SOURCE_DIR})  # 设置项目根目录
set(include_path ${project_root}/include)  # 设置头文件目录
set(src_path ${project_root}/src)  # 设置源文件目录
include_directories(${include_path})


# 生成可执行文件
file(GLOB src_list ${src_path}/*.cpp)
add_executable(${PROJECT_NAME} ${src_list})


# 链接库
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(jsoncpp CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    ServerCommon
    pthread
    Boost::system
    jsoncpp_lib
)
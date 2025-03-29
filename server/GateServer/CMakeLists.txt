set(output_name GateServer)  # 生成的可执行文件名称

# 项目基础目录配置
set(project_root ${CMAKE_CURRENT_SOURCE_DIR})  # 设置项目根目录
set(common_path ${project_root}/../common)  # 设置公共文件目录
set(include_path ${project_root}/include)  # 设置头文件目录
set(src_path ${project_root}/src)  # 设置源文件目录
set(bin_path ${project_root}/bin)  # 设置可执行文件目录
set(common_include_path ${common_path}/include)  # 设置公共头文件目录
set(common_src_path ${common_path}/src)  # 设置公共源文件目录
set(common_proto_path ${common_path}/proto)  # 设置公共proto文件目录
include_directories(${include_path})
include_directories(${common_include_path})
include_directories(${common_proto_path})

# 生成可执行文件
file(GLOB src_list ${src_path}/*.cpp)
file(GLOB common_src_list ${common_src_path}/*.cpp)
file(GLOB proto_src_list ${common_proto_path}/*.cc)
set(src_list ${src_list} ${common_src_list} ${proto_src_list})  # 将所有源文件添加到列表中
set(EXECUTABLE_OUTPUT_PATH ${bin_path})
add_executable(${output_name} ${src_list})


# 链接库
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED 
    PATHS "/usr/local/grpc1_34_0"
)

target_link_libraries(
    ${output_name} 
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
    pthread
    Boost::system
    Boost::filesystem
    jsoncpp_lib
)
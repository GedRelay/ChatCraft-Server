project(ServerCommon)

set(project_root ${CMAKE_CURRENT_SOURCE_DIR})  # 设置项目根目录
set(include_path ${project_root}/include)  # 设置头文件目录
set(proto_path ${project_root}/proto)  # 设置proto文件目录
set(src_path ${project_root}/src)  # 设置源文件目录

set(proto_files ${proto_path}/message.proto)

file(GLOB src_list ${src_path}/*.cpp)

add_library(${PROJECT_NAME} SHARED
    ${src_list}
    ${proto_files}
)

# 添加proto生成的头文件和include目录
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${include_path}
    PUBLIC /usr/local/hiredis/include
)


# 搜索库
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED 
    PATHS "/usr/local/grpc1_34_0"
)


target_link_directories(${PROJECT_NAME}
    PUBLIC /usr/local/hiredis/lib
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
    Boost::system
    Boost::filesystem
    hiredis
)


# 获取gRPCcpp插件路径, 生成proto文件
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET ${PROJECT_NAME}
    LANGUAGE cpp
)

protobuf_generate(TARGET ${PROJECT_NAME}
    LANGUAGE grpc
    GENERATE_EXTENSIONS ".grpc.pb.h;.grpc.pb.cc"
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)
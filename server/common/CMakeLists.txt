project(ServerCommon)

set(project_root ${CMAKE_CURRENT_SOURCE_DIR})  # 设置项目根目录
set(include_path ${project_root}/include)  # 设置头文件目录
set(proto_path ${project_root}/proto)  # 设置proto文件目录
set(src_path ${project_root}/src)  # 设置源文件目录

set(proto_files
    ${proto_path}/message.proto
)

file(GLOB src_list ${src_path}/*.cpp)

add_library(${PROJECT_NAME} SHARED
    ${src_list}
    ${proto_files}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${include_path}
)
message(STATUS "Generated proto files: ${CMAKE_CURRENT_BINARY_DIR}/proto")

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
    Boost::system
    Boost::filesystem
)

# 生成proto文件
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED 
    PATHS "/usr/local/grpc1_34_0"
)

# 获取gRPC插件路径
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET ${PROJECT_NAME}
    LANGUAGE cpp
)

protobuf_generate(TARGET ${PROJECT_NAME}
    LANGUAGE grpc
    GENERATE_EXTENSIONS ".grpc.pb.h;.grpc.pb.cc"
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)
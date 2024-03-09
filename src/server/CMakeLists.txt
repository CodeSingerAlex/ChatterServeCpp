aux_source_directory(. SOURCE_FILE)

add_executable(ChatServer ${SOURCE_FILE})

target_link_libraries(ChatServer muduo_net muduo_base pthread)
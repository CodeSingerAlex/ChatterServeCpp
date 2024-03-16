aux_source_directory(. SOURCE_FILE)
aux_source_directory(./db DB_LIST)
add_executable(ChatServer ${SOURCE_FILE} ${DB_LIST}) 

target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)

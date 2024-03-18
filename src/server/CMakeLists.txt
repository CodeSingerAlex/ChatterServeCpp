aux_source_directory(. SOURCE_FILE)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)
add_executable(ChatServer ${SOURCE_FILE} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST}) 

target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)

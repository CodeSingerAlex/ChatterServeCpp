aux_source_directory(. DIR_LIB_SRCS)

add_executable(ChatClient ${DIR_LIB_SRCS})

target_link_libraries(ChatClient pthread)
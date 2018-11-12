project(commands)

file(GLOB SERV_SRC "serv/*.c" "serv/*.cpp")
add_executable(owd_serv ${SERV_SRC})

target_link_libraries(owd_serv owd_core)
target_link_libraries(owd_serv owd_http)

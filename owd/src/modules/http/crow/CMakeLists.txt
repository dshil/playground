project(crow)

file(GLOB http_src "*.cpp")

add_library(owd_http STATIC ${http_src})

target_link_libraries(owd_http owd_core)
target_link_libraries(owd_http ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(owd_http ${OPENSSL_LIBRARIES})
target_link_libraries(owd_http ${Boost_LIBRARIES})

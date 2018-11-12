project(cpprest)

file(GLOB RESTSDK_HTTP_SRC "*.cpp")

add_library(owd_http STATIC ${RESTSDK_HTTP_SRC})

target_link_libraries(owd_http owd_core)
target_link_libraries(owd_http
    "${PROJECT_DEPS_PATH}/${PROJECT_NAME}/build/Binaries/libcpprest.a")
target_link_libraries(owd_http ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(owd_http ${OPENSSL_LIBRARIES})
target_link_libraries(owd_http ${Boost_LIBRARIES})

target_include_directories(owd_http PUBLIC
    "${PROJECT_DEPS_PATH}/${PROJECT_NAME}/Release/include")

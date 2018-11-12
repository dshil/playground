project(libuv)

file(GLOB LIBUV_HTTP_SRC "*.cpp")

add_library(owd_http ${LIBUV_HTTP_SRC})
target_link_libraries(owd_http uv)
target_link_libraries(owd_http owd_core)
target_link_libraries(owd_http "${PROJECT_DEPS_PATH}/http-parser/libhttp_parser.a")


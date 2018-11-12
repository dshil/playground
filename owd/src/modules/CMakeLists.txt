project(modules)

if (OWD_BUILD_HTTP_LIBUV)
    add_subdirectory(http/libuv)
elseif (OWD_BUILD_HTTP_CPPREST)
    add_subdirectory(http/cpprest)
else ()
    add_subdirectory(http/crow)
endif (OWD_BUILD_HTTP_LIBUV)

file(GLOB CORE_SRC
    "core/*.cpp"
)

add_library(owd_core ${CORE_SRC})

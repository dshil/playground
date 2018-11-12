project(tests)

file(GLOB TEST_SRC
    "http/*"
    "core/*"
    "types/*"
    "storage/*"
)

add_executable(test_main test_main.cpp ${TEST_SRC})
list(APPEND libs "owd_core" "owd_http" ${CPR_LIBRARIES})

foreach(lib IN LISTS libs)
    target_link_libraries(test_main PRIVATE ${lib})
endforeach()

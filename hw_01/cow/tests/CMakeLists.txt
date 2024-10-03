include_directories(${GTEST_INCLUDE_DIRECTORIES})


set(COW_TEST_EXEC test_cow)
set(COW_TEST_SRC test_cow.cc)
add_executable(${COW_TEST_EXEC})


target_sources(${COW_TEST_EXEC} PRIVATE ./${COW_TEST_SRC})
set(COMMON_FLAGS -Wall -Wextra)
target_compile_options(${COW_TEST_EXEC} PRIVATE ${COMMON_FLAGS})
add_dependencies(${COW_TEST_EXEC} cow_string)
target_link_libraries(${COW_TEST_EXEC} cow_string GTest::Main)

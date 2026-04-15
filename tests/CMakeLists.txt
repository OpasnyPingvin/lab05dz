find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(test_account test_account.cpp)
target_link_libraries(test_account banking GTest::gtest)

add_executable(test_transaction test_transaction.cpp)
target_link_libraries(test_transaction banking GTest::gtest GTest::gmock)

target_include_directories(test_account PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(test_transaction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(ENABLE_COVERAGE)
    target_compile_options(test_account PRIVATE -O0 -g --coverage)
    target_link_options(test_account PRIVATE --coverage)
    target_compile_options(test_transaction PRIVATE -O0 -g --coverage)
    target_link_options(test_transaction PRIVATE --coverage)
endif()

gtest_discover_tests(test_account)
gtest_discover_tests(test_transaction)

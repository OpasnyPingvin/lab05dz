find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(test_account test_account.cpp)
target_link_libraries(test_account banking GTest::gtest GTest::gtest_main pthread)

add_executable(test_transaction test_transaction.cpp)
target_link_libraries(test_transaction banking GTest::gtest GTest::gmock GTest::gtest_main pthread)

target_include_directories(test_account PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(test_transaction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

gtest_discover_tests(test_account)
gtest_discover_tests(test_transaction)

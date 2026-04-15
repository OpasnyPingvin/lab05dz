add_library(banking STATIC
    Account.cpp
    Transaction.cpp
)

target_include_directories(banking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(banking PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

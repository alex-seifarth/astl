
set(TEST rlp-tests)

set(SRCS
    test_runloop.cpp
)

add_executable(${TEST} ${SRCS})

target_link_libraries(${TEST}
    PRIVATE core rlp GTest::Main GTest::GTest
)

target_compile_options(${TEST}
    PRIVATE -Wall -Wextra -pedantic -Werror
)

add_test(${TEST} ${TEST})

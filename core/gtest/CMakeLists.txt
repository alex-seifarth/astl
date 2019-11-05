set(SRCS
    test-event.cpp
    test-recursive_event.cpp
    test-slot_holder.cpp
)

add_executable(core-tests ${SRCS})

target_link_libraries(core-tests
    PRIVATE core GTest::Main GTest::GTest
)

target_compile_options(core-tests
    PRIVATE -Wall -Wextra -pedantic -Werror
)

add_test(core-tests core-tests)

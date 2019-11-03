
set(COMPONENT ut-astl-core)

set(SRCS
    core/test-signalslot.h.cpp
)

add_executable(${COMPONENT} ${SRCS})

target_link_libraries(${COMPONENT}
    PRIVATE astl GTest::Main GTest::GTest
)

add_test(${COMPONENT} ${COMPONENT})

target_compile_options(${COMPONENT}
    PRIVATE -Wall -Wextra -pedantic -Werror
)
